#include "packet.h"
int currseq = 0;
int buffcount = -1;
char inputBuffer[BUFFER_SIZE + 1];
PACKET *lastUnack1, *lastUnack2;
int lastseq1, lastseq2;
FILE *fp;

FILE *getfilestream(FILE *fp, char *buf, int buff_len)
{
    int lenofinputread;

    if (fp == NULL)
    {
        exit(1);
    }

    if (feof(fp))
    {
        fclose(fp);
        return NULL;
    }

    if ((lenofinputread = fread(buf, sizeof(char), buff_len - 1, fp)) > 0)
    {
        buf[lenofinputread] = '\0';
        return fp;
    }
    fclose(fp);
    return NULL;
}
void inputIntoBuffer()
{
    int i;

    for (i = 0; i < BUFFER_SIZE; i++)
    {
        inputBuffer[i] = '\0';
    }
    fp = getfilestream(fp, inputBuffer, BUFFER_SIZE);

    buffcount += 1;
}

PACKET *createPacket(int datalength, int seqno, int channelID)
{
    PACKET *newpkt = (PACKET *)malloc(sizeof(PACKET));

    newpkt->size = datalength;
    newpkt->ACK_OR_DATA = 1;
    newpkt->channel_id = channelID;
    newpkt->seq_no = seqno + (BUFFER_SIZE)*buffcount;

    for (int i = currseq; i < currseq + PACKET_SIZE; i++)
    {
        newpkt->data[i - currseq] = inputBuffer[i];
    }
    return newpkt;
}

int compute()
{
    if (currseq == BUFFER_SIZE)
    {
        inputIntoBuffer();
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
    struct sockaddr_in si_server;

    int sockfd, slen;

    //TWO SOCKETS FOR TWO CHANNELS
    int *s = (int *)malloc(sizeof(int) * 2);
    s[0]= socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    s[1]= socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (s[0] < 0 || s[1] < 0)
    {
        die("socket()");
    }

    //SET ADDRESS
    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(SERVER_PORT);
    si_server.sin_addr.s_addr = inet_addr("127.0.0.1");


    int c1 = connect(s[0], (struct sockaddr *)&si_server, sizeof(si_server));
    int c2 = connect(s[1], (struct sockaddr *)&si_server, sizeof(si_server));

    if (c1 < 0 || c2 < 0)
    {
        die("connect()");
    }

    fp = fopen("input.txt", "r");
    inputIntoBuffer();

    if (fp == NULL)
    {
        die("fopen()");
    }
    fd_set rfds;
    PACKET *pkt1, *pkt2;
    int state = 0, retval, maxsock;
    while (1)
    {

        switch (state)
        {
        //CREATE AND TRANSMIT TWO NEW PACKETS
        case 0:
        {
            // CREATE A PACKET FROM THE FILE
            pkt1 = createPacket(PACKET_SIZE, currseq, 0);

            // UDPATE STUFF
            lastUnack1 = pkt1;
            lastseq1 = pkt1->seq_no;

            currseq += PACKET_SIZE;

            //UPDATE IF FINAL PACKET
            if (compute())
            {
                pkt1->last_packet = 1;
            }
            pkt1->last_packet = 0;

            // TRANSMIT THE PACKET
            send(sockfd, pkt1, sizeof(*pkt1), 0);

            printf("TRANSMITTED PACKET WITH SEQ NO (BYTE): %d ON CHANNEL %d\n", pkt1->seq_no, 0);

            pkt2 = createPacket(PACKET_SIZE, currseq, 1);

            // UDPATE STUFF
            lastUnack2 = pkt2;
            lastseq2 = pkt2->seq_no;

            currseq += PACKET_SIZE;

            // UPDATE IF FINAL PACKET
            if (compute())
            {
                pkt2->last_packet = 1;
            }
            pkt2->last_packet = 0;

            // TRANSMIT THE PACKET
            send(sockfd, pkt2, sizeof(*pkt2), 0);

            printf("TRANSMITTED PACKET WITH SEQ NO: %d ON CHANNEL: %d\n", pkt2->seq_no, 1);

            state = 1;
            break;
        }

        //CHECK BOTH SOCKETS FOR AN ACK
        case 1:
        {
            // WAIT FOR ACK FROM SERVER
            FD_ZERO(&rfds);

            FD_SET(s[0], &rfds);
            FD_SET(s[1], &rfds);

            struct timeval tv;
            tv.tv_sec = 5;
            tv.tv_usec = 0;

            //GET MAXIMUM SOCKET VALUE
            maxsock = (s[0] > s[1]) ? s[0] : s[1];

            // SELECT
            retval = select(maxsock + 1, &rfds, NULL, NULL, &tv);

            if (retval == 0)
            {
                printf("TIMEOUT AT SENDER: RESENDING PKT WITH SEQ NO %d\n", lastUnack->seq_no);
                state = 4; //RETRANSMIT BOTH
                break;
            }
            //CHECK FROM WHERE TO RECEIVE
            else if (retval > 0)
            {
                for (int i = 0; i < 2; i ++){
                    if (FD_ISSET(s[i], &rfds)){
                        recv(sockfd, pkt, sizeof(*pkt), 0);
                }
                    

                if (pkt->seq_no == lastseq)
                {
                    state = 0;
                }

                printf("RECEIVED ACK FOR PKT WITH SEQ NO %d\n", pkt->seq_no);
            }
            else
            {
                die("select()");
            }
        }
        }
    }
}