#include <stdio.h>
#include <sys/socket.h> //SOCKET, CONNECT, SEND, RECV
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //CLOSE
#define BUFSIZE 32
#define SERVER_PORT_NO 12346

int main()
{
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        printf("ERROR IN OPENING A SOCKET\n");
        exit(1);
    }

    printf("CLIENT SOCKET CREATED\n");

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT_NO);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("ADDRESS ASSIGNED\n");

    // ESTABLISH CONNECTION
    int c = connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (c < 0)
    {
        printf("ERROR WHILE ESTABLISHING CONNECTION\n");
        exit(1);
    }

    printf("CONNECTION ESTABLISHED\n");

    // SEND DATA
    printf("ENTER NUMBER: \n");
    int num;
    scanf("%d", &num);
    char numStr[20];

    for (int i = 0; i < 20; i ++ ){
        numStr[i] = '\0';
    }
        sprintf(numStr, "%d", num);

    int bytesSent = send(sock, numStr, strlen(numStr), 0);

    if (bytesSent != strlen(numStr))
    {
        printf("ERROR WHILE SENDING A MESSAGE\n");
        exit(1);
    }
    printf("DATA SENT\n");

    // RECEIVE DATA
    char recvBuffer[BUFSIZE];
    int bytesRecvd = recv(sock, recvBuffer, BUFSIZE - 1, 0);
    recvBuffer[bytesRecvd] = '\0';

    if (bytesRecvd < 0)
    {
        printf("ERROR WHILE RECEIVING DATA FROM SERVER\n");
        exit(1);
    }

    recvBuffer[bytesRecvd] = '\0';
    printf("\n%s\n", recvBuffer);
    close(sock);
    exit(0);
}
