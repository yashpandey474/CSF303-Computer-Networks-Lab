#include "packet.h"

int main(){
    int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0)
    {
        printf("ERROR WHILE SERVER SOCKET CREATION\n");
        exit(1);
    }
    printf("SERVER SOCKET CREATED\n");

    struct sockaddr_in serverAddress, clientAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT_R1);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("SERVER ADDRESS ASSIGNED\n");

    int temp = bind(serverSocket, (struct sockaddr *)&serverAddress,
                    sizeof(serverAddress));
    if (temp < 0)
    {
        printf("Error while binding\n");
        exit(0);
    }

    printf("Binding successful\n");
    int temp1 = listen(serverSocket, MAXPENDING);
    if (temp1 < 0)
    {
        printf("Error in listen");
        exit(0);
    }
    printf("Now Listening\n");

    int clientLength = sizeof(clientAddress);

    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);

    FILE *fp = fopen("output.txt", "w");

    PACKET *pkt = (PACKET *)malloc(sizeof(PACKET));
    PACKET *ack_pkt = (PACKET *)malloc(sizeof(PACKET));

    int nextSeq = 0;

    while (1)
    {
        // fflush(stdout);
        // RECEIVE FROM CLIENT
        int temp2 = recv(clientSocket, pkt, sizeof(*pkt), 0);

        if (temp2 < 0)
        {
            printf("problem in temp 2");
            exit(0);
        }

        // printf("RECEIVED PKT WITH SEQ NO %d\n", pkt->seq_no);

        //RANDOM PACKET DROP
        if(dropPacket()){
            printf("DROPPED PKT WITH SEQ NO: %d\n", pkt->seq_no);
            continue;
        }

        if (pkt->seq_no == nextSeq)
        {
            nextSeq += pkt->size;

            //WRITE CONTENT TO FILE
            fputs(pkt->payload, fp);
            fflush(fp);
        }

        //SEND AN ACK
        ack_pkt->seq_no = pkt->seq_no;

        int bytesSent = send(clientSocket, ack_pkt, sizeof(*ack_pkt), 0);

        printf("SENT ACK FOR PKT WITH SEQ NO %d\n", ack_pkt->seq_no);
    }
}