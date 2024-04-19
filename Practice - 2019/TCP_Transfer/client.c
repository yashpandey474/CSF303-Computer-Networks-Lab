#include "packet.h"
int currseq = 0;
int buffcount = -1;
char inputBuffer[BUFFER_SIZE + 1];
PACKET *lastUnack;
int lastseq, lastseq2;
FILE *fp;

FILE* getfilestream(FILE* fp, char* buf, int buff_len){
    int lenofinputread;

    if(fp == NULL){
        exit(1);
    }

    if (feof(fp)){
        fclose(fp);
        return NULL;
    }

    if ((lenofinputread = fread(buf, sizeof(char), buff_len - 1, fp)) > 0){
        buf[lenofinputread] = '\0';
        return fp;
    }
    fclose(fp);
    return NULL;
}
void inputIntoBuffer()
{
    int i;

    for (i = 0; i < BUFFER_SIZE; i++){
        inputBuffer[i] = '\0';
    }
    fp = getfilestream(fp, inputBuffer, BUFFER_SIZE);

    buffcount += 1;
}

PACKET* createPacket(int datalength, int seqno, int seqno0){
    PACKET *newpkt = (PACKET *)malloc(sizeof(PACKET));

    newpkt->size = datalength;
    newpkt->ack_or_data = 1;
    newpkt->seq_no_0 = seqno0;
    newpkt->seq_no = seqno + (BUFFER_SIZE)*buffcount;

    for (int i = currseq; i < currseq + PACKET_SIZE; i ++){
        newpkt->payload[i - currseq] = inputBuffer[i];
    }
    return newpkt;
}

int compute(){
    if (currseq == BUFFER_SIZE){
        inputIntoBuffer();
        currseq = 0;
    }

    else if(inputBuffer[currseq] == '\0'){
        return 1;
    }
    return 0;
}
int main()
{
    struct sockaddr_in si_server;

    int sockfd, slen;

    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd < 0){
        die("socket()");
    }

    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(SERVER_PORT_R1);
    si_server.sin_addr.s_addr = inet_addr("127.0.0.1");
    int c = connect(sockfd, (struct sockaddr *)&si_server, sizeof(si_server));

    if (c < 0){
        die("connect()");
    }

    fp = fopen("input.txt", "r");
    inputIntoBuffer();

    if (fp == NULL){
        die("fopen()");
    }
    fd_set rfds;
    PACKET *pkt;
    int state = 0, retval;
    while (1)
    {

        switch(state){
            case 0: {
                //CREATE A PACKET FROM THE FILE
                pkt = createPacket(PACKET_SIZE, currseq, state/2);

                //UDPATE STUFF
                lastUnack = pkt;
                lastseq = pkt->seq_no;

                currseq += PACKET_SIZE;

                if (compute()){
                    pkt->final_packet = 1;
                }
                pkt->final_packet = 0;

                //TRANSMIT THE PACKET
                send(sockfd, pkt, sizeof(*pkt), 0);

                state = 1;

                printf("TRANSMITTED PACKET WITH SEQ NO (BYTE): %d\n", pkt->seq_no);

                break;
            }

            case 1: {
                //WAIT FOR ACK FROM SERVER
                FD_ZERO(&rfds);

                FD_SET(sockfd, &rfds);

                struct timeval tv;
                tv.tv_sec = 5;
                tv.tv_usec = 0;

                //SELECT
                retval = select(sockfd + 1, &rfds, NULL, NULL, &tv);

                if (retval == 0){
                    printf("TIMEOUT AT SENDER: RESENDING PKT WITH SEQ NO %d\n", lastUnack->seq_no);
                    send(sockfd, lastUnack, sizeof(*lastUnack), 0);
                }
                else if (retval > 0){
                    recv(sockfd, pkt, sizeof(*pkt), 0);

                    if (pkt->seq_no == lastseq){
                        state = 0;
                    }

                    printf("RECEIVED ACK FOR PKT WITH SEQ NO %d\n", pkt->seq_no);
                }
                else{
                    die("select()");
                }
            }
        }

    }
}