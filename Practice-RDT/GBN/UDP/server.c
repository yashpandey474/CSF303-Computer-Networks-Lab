#include "packet.h"


int main(){
    srand(time(NULL));

    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    int slen = sizeof(clientAddr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        die("socket()");
    }

    memset((char *)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    slen = sizeof(serverAddr);

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1){
        die("bind()");
    }

    int expectedSeqNo = 1;
    FILE *fp = fopen("output.txt", "w");

    PACKET *snd_pkt = (PACKET *)malloc(sizeof(PACKET)), *pkt;
    snd_pkt->seqNo = 0;
    snd_pkt->isAck = 1;

    while (1){
        if (recvfrom(sockfd, pkt, sizeof(*pkt), 0, (struct sockaddr *)&clientAddr, &slen) == -1){
            die("recvfrom()");
        }

        //RANDOM DROP
        if (dropPacket()){
            printf("DROPPED PKT WITH SEQ NO: %d\n", pkt->seqNo);
            continue;
        }

        if (pkt -> seqNo == expectedSeqNo){
            fputs(pkt->payload, fp);
            fflush(fp);
            snd_pkt->seqNo = expectedSeqNo;
            expectedSeqNo += 1;
        }
        if(sendto(sockfd, snd_pkt, sizeof(*snd_pkt), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr)) == -1){
            die("sendto()");
        }
        printf("SENT ACK WITH SEQ NO: %d FOR PKT WITH SEQ NO: %d\n", snd_pkt->seqNo, pkt->seqNo);
    }
}