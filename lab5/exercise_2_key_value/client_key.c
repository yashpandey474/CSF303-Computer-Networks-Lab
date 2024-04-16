#include <stdio.h>
#include <sys/socket.h> //SOCKET, CONNECT, SEND, RECV
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //CLOSE
#define BUFSIZE 32
#define SERVER_PORT_NO 12345

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
    int c = connect(sock,
                    (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    printf("%d\n", c);

    if (c < 0)
    {
        printf("ERROR WHILE ESTABLISHING CONNECTION\n");
        exit(1);
    }
    printf("CONNECTION ESTABLISHED\n");

    int command;
    char msg[BUFSIZE];

    char goodbye[8] = "Goodbye";
    char okay[3] = "OK";

    while (1)
    {
        // GET THE COMMAND
        printf("ENTER COMMAND (get val/put key value/delete key)\n");

        gets(msg);

        // SEND THE COMMAND
        int bytesSent = send(sock, msg, strlen(msg), 0);

        if (bytesSent != strlen(msg))
        {
            printf("ERROR WHILE SENDING A MESSAGE\n");
            exit(1);
        }
        printf("COMMAND SENT: %s\n", msg);

        // RECEIVE DATA
        char recvBuffer[BUFSIZE];
        int bytesRecvd = recv(sock, recvBuffer, BUFSIZE - 1, 0);

        if (bytesRecvd < 0)
        {
            printf("ERROR WHILE RECEIVING DATA FROM SERVER\n");
            exit(1);
        }

        recvBuffer[bytesRecvd] = '\0';

        if (strcmp(goodbye, recvBuffer) == 0)
        {
            printf("Server terminated connection\n");
            exit(1);
        }

        printf("RESPONSE RECEIVED: %s\n", recvBuffer);
    }

    close(sock);
    exit(0);
}
