#include "packet.h"

int main()
{
    srand(time(NULL));

    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    int slen = sizeof(clientAddr);

    if ((sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        die("socket()");
    }




    memset((char *)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    slen = sizeof(serverAddr);

    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        die("bind()");
    }

    int expectedSeqNo = 1;
    FILE *fp = fopen("output.txt", "w");

    PACKET *snd_pkt = (PACKET *)malloc(sizeof(PACKET)), *pkt;
    snd_pkt->seqNo = 0;
    snd_pkt->isAck = 1;

    //ACCEPT A SINGLE CONNECTION BEFORE STARTING WRITING
    if (listen(sockfd, 1) == -1){
        die("listen()");
    }

    int clientSockfd = accept(sockfd, (struct sockaddr *)&clientAddr, &slen);

    while (1)
    {
        if (recv(clientSockfd, pkt, sizeof(*pkt), 0) == -1)
        {
            die("recvfrom()");
        }

        // RANDOM DROP
        if (dropPacket())
        {
            printf("DROPPED PKT WITH SEQ NO: %d\n", pkt->seqNo);
            continue;
        }

        if (pkt->seqNo == expectedSeqNo)
        {
            fputs(pkt->payload, fp);
            fflush(fp);
            snd_pkt->seqNo = expectedSeqNo;
            expectedSeqNo += 1;
        }

        if (send(clientSockfd, snd_pkt, sizeof(*snd_pkt), 0) == -1)
        {
            die("sendto()");
        }
        printf("SENT ACK WITH SEQ NO: %d FOR PKT WITH SEQ NO: %d %d\n", snd_pkt->seqNo, pkt->seqNo, pkt->finalPacket);
    }
}