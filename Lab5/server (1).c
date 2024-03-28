#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>

#define MAXPENDING 3
#define BUFFERSIZE 32
#define SERVER_PORT_NO 12345

int main()
{

    // CREATE A TCP SOCKET
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
    serverAddress.sin_port = htons(SERVER_PORT_NO);
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

    char msg[BUFFERSIZE];
    int clientLength = sizeof(clientAddress);
    int childpid, clientSocket;

    char get[4] = "get";
    char put[4] = "put";
    char del[4] = "del";
    char bye[4] = "bye";

    char okay[3] = "OK";

    char substring[4];

    char goodbye[] = "goodbye";

    int key, value;

    FILE *fp = fopen("database.txt", "ab");
    if (fp == NULL)
    {
        printf("File open error");
        return 1;
    }

    while (1)
    {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);

        if (clientLength < 0)
        {
            printf("Error in client socket");
            exit(0);
        }
        printf("Handling Client %s\n", inet_ntoa(clientAddress.sin_addr));

        if ((childpid = fork()) == 0)
        {
            close(serverSocket);

            while (1)
            {
                int temp2 = recv(clientSocket, msg, BUFFERSIZE, 0);
                if (temp2 < 0)
                {
                    printf("problem in temp 2");
                    exit(0);
                }
                printf("RECEIVED FROM CLIENT %s\n", msg);

                strncpy(substring, msg, 3);

                printf("SUBSTRING: %s\n", substring);

                if ((strcmp(substring, put)) == 0)
                {
                    printf("PUT COMMAND\n");

                    strncpy(substring, msg + 4, 3);

                    key = atoi(substring);

                    strncpy(substring, msg + 8, 3);

                    value = atoi(substring);

                    FILE *fp = fopen("database.txt", "ab");
                    if (fp == NULL)
                    {
                        printf("File open error");
                        return 1;
                    }

                    fseek(fp, key, SEEK_SET);

                    fprintf(fp, "%d", value);
                }

                if ((strcmp(substring, get)) == 0)
                {
                    printf("GET COMMAND\n");

                    strncpy(substring, msg + 4, 3);

                    key = atoi(substring);

                    FILE *fp = fopen("database.txt", "rb");
                    if (fp == NULL)
                    {
                        printf("File open error");
                        return 1;
                    }

                    fseek(fp, key, SEEK_SET);

                    if (fscanf(fp, "%d", value) == -1)
                    {
                        printf("ERROR WHILE READING FROM FILE\n");
                    }

                    if (value == -1)
                    {
                        printf("KEY WAS PREVIOUSLY DELETED\n");
                    }

                    int bytesSent = send(clientSocket, &value, sizeof(value), 0);
                    if (bytesSent != sizeof(value))
                    {
                        printf("Error while sending message to client");
                        exit(0);
                    }

                    printf("VALUE SENT TO CLIENT\n");
                }

                if ((strcmp(substring, del)) == 0)
                {
                    printf("DEL COMMAND\n");

                    strncpy(substring, msg + 4, 3);

                    key = atoi(substring);

                    FILE *fp = fopen("database.txt", "wb");
                    if (fp == NULL)
                    {
                        printf("File open error");
                        return 1;
                    }

                    fseek(fp, key, SEEK_SET);

                    fprintf(fp, "%d", -1);
                }

                if ((strcmp(substring, bye)) == 0)
                {
                    printf("BYE COMMAND\n");

                    int bytesSent = send(clientSocket, goodbye, strlen(goodbye), 0);
                    if (bytesSent != strlen(goodbye))
                    {
                        printf("Error while sending message to client");
                        exit(0);
                    }

                    printf("SENT GOODBYE REPLY\nTERMINATING CONNECTION\n");
                    break;
                }

                // printf ("ENTER MESSAGE FOR CLIENT\n");
                // gets (msg);
            }
        }

        close(clientSocket);
    }

    close(serverSocket);
}