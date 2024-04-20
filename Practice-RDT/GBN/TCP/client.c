#include "packet.h"

FILE *fp;
int currseq = 0, buffcount = -1;
char inputBuffer[BUFFER_SIZE + 1];

FILE *getfilestream(FILE *fp, char *inputBuffer, int buffersize)
{
    int lenofinputread;

    if (fp == NULL)
    {
        die("fp == NULL");
    }

    else if (feof(fp))
    {
        fclose(fp);
        return NULL;
    }

    if ((lenofinputread = (fread(inputBuffer, sizeof(char), buffersize, fp))) > 0)
    {
        inputBuffer[lenofinputread] = '\0';
        return fp;
    }
    fclose(fp);
    return NULL;
}

void inputintobuffer()
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        inputBuffer[i] = '\0';
    }
    fp = getfilestream(fp, inputBuffer, BUFFER_SIZE);
    buffcount += 1;
}
PACKET *createPacket(int datalength, int seqno)
{
    PACKET *newpkt = (PACKET *)malloc(sizeof(PACKET));
    newpkt->size = datalength;
    newpkt->isAck = 0;
    newpkt->seqNo = seqno;

    for (int i = currseq; i < currseq + PACKET_SIZE; i++)
    {
        newpkt->payload[i - currseq] = inputBuffer[i];
    }

    return newpkt;
}
int compute()
{
    if (currseq >= BUFFER_SIZE)
    {
        inputintobuffer();
        currseq = 0;
    }

    else if (inputBuffer[currseq] == '\0')
    {
        return 1;
    }
    return 0;
}
int main()
{

    // SEED RANDOM
    srand(time(NULL));
    int sockfd;
    struct sockaddr_in serverAddr;

    memset((char *)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        die("socket()");
    }

    //CONNECT: TCP
    int c = connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (c < 0){
        die("connect()");
    }



    // STORE PACKETS IN WINDOW
    PACKET sent_packets[WINDOW_SIZE];
    PACKET *pkt = (PACKET *)malloc(sizeof(PACKET));
    int check = 0;
    PACKET resend_pkt;
    int base = 1, slen = sizeof(serverAddr);
    int nextseqnum = 1, retval;

    // COMMON TIMER VARIABLES
    fd_set rfds;
    struct timeval tv, start_time, end_time;
    long elapsed_time, rem_time;
    tv.tv_sec = TIMEOUT_PERIOD;
    tv.tv_usec = 0;

    fp = fopen("input.txt", "r");
    inputintobuffer();
    if (fp == NULL)
    {
        die("fopen()");
    }

    while (!check || (base < nextseqnum))
    {
        while (nextseqnum < base + WINDOW_SIZE && !check)
        {
            pkt = createPacket(PACKET_SIZE, nextseqnum);
            currseq += PACKET_SIZE;
            pkt->seqNo = nextseqnum;

            if (compute())
            {
                pkt->finalPacket = 1;
                check = 1;
            }
            pkt->finalPacket = 0;

            // SEND THE PACKET
            if (send(sockfd, pkt, sizeof(*pkt), 0) == -1)
            {
                die("sendto()");
            }

            printf("SENT PKT WITH SEQ NO %d\n", pkt->seqNo);

            // STORE THE PACKET IN WINDOW
            sent_packets[pkt->seqNo % WINDOW_SIZE] = *pkt;
            printf("ASSIGNED %d SEQ NO PKT TO %d SPACE\n", pkt->seqNo, pkt->seqNo % WINDOW_SIZE);

            // INCREMENT NEXT SEQ NUM
            nextseqnum += 1;
        }

        // RECEIVE ACKS
        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);

        // SELECT CALL
        gettimeofday(&start_time, NULL);
        printf("WAITING FOR: %ds", tv.tv_sec);
        retval = select(sockfd + 1, &rfds, NULL, NULL, &tv);
        gettimeofday(&end_time, NULL);

        elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000000 + (end_time.tv_usec - start_time.tv_usec);
        rem_time = (TIMEOUT_PERIOD * 1000000) - elapsed_time;

        if (retval == 0)
        {
            // TRANSMIT ALL PACKETS AND REINITIALISE TIMER
            tv.tv_sec = TIMEOUT_PERIOD;
            tv.tv_usec = 0;

            for (int i = base; i < nextseqnum; i++)
            {
                resend_pkt = sent_packets[i % WINDOW_SIZE];

                // SEND THE PACKET
                if (send(sockfd, &resend_pkt, sizeof(resend_pkt), 0) == -1)
                {
                    die("sendto()");
                }

                printf("BASE: %d RESENT PKT WITH SEQNO: %d\n",  base, resend_pkt.seqNo);
            }
        }

        // RECEIVE ACK
        else if (retval > 0)
        {
            // RECEIVE THE ACK
            if (recv(sockfd, pkt, sizeof(*pkt), 0) == -1)
            {
                die("recvfrom()");
            }

            printf("BASE: %d RCVD ACK WITH SEQ NO: %d\n", base, pkt->seqNo);

            if (pkt->isAck == 1 && pkt->seqNo >= base)
            {
                base = pkt->seqNo + 1;

                // REINITIALISE TIMER
                printf("REINITIALISED TIMER\n");
                tv.tv_sec = TIMEOUT_PERIOD;
                tv.tv_usec = 0;
            }

            // ELSE: UPDATE THE TIMER FOR OLDEST PACKET
            else
            {
                printf("UPDATING TIMER FOR OLDEST PACKET\n");
                tv.tv_sec = rem_time / 1000000;
                tv.tv_usec = rem_time % 1000000;
            }
        }
        else
        {
            die("select()");
        }
    }
}