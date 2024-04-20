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
#define DATABASE_FILE "database.txt"
#define TEMP_FILE "temp.txt"

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

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(1);
    }

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
    char * substring;
    int keytofind, key, value;

    FILE *fp;
    FILE *tfp;

    char line[100];
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
                printf("WAITING FOR COMMAND..\n");
                for (int i = 0; i < BUFFERSIZE; i += 1){
                    msg[i] = '\0';
                }
                
                
                int temp2 = recv(clientSocket, msg, BUFFERSIZE, 0);
                if (temp2 < 0)
                {
                    printf("problem in temp 2");
                    exit(0);
                }
                printf("RECEIVED FROM CLIENT %s\n", msg);

                substring = strtok(msg, " ");
                

                // strncpy(substring, msg, 3);

                printf("SUBSTRING: %s\n", substring);

                if ((strcmp(substring, "put")) == 0)
                {
                    printf("PUT COMMAND\n");

                    substring = strtok(NULL, " ");

                    key = atoi(substring);

                    substring = strtok(NULL, " ");

                    value = atoi(substring);

                    printf("KEY: %d VALUE: %d\n", key, value);

                    fp = fopen(DATABASE_FILE, "a");
                    if (fp == NULL)
                    {
                        printf("File open error");
                        return 1;
                    }

                    fprintf(fp, "%d %d\n", key, value);
                    printf("WROTE NEW PAIR TO FILE\n");
                    fflush(fp);

                    int bytesSent = send(clientSocket, "Pair added to database", strlen("Pair added to database"), 0);
                    if (bytesSent != strlen("Pair added to database"))
                    {
                        printf("Error while sending message to client");
                        exit(0);
                    }

                }

                if ((strcmp(substring, "get")) == 0)
                {
                    printf("GET COMMAND\n");

                    substring = strtok(NULL, " ");

                    keytofind = atoi(substring);

                    FILE *fp = fopen("database.txt", "r");
                    if (fp == NULL)
                    {
                        printf("File open error");
                        return 1;
                    }

                    //READ FROM FILE LINE-BY-LINE
                    int found = 0;
                    while (fgets(line, sizeof(line), fp) != NULL)
                    {
                        if (sscanf(line, "%d %d", &key, &value) != 2){
                            exit(1);
                        }

                        if (key == keytofind){
                            sprintf(line, "%d", value);
                            int bytesSent = send(clientSocket, line, strlen(line), 0);
                            if (bytesSent != strlen(line))
                            {
                                printf("Error while sending message to client");
                                exit(0);
                            }

                            printf("VALUE SENT TO CLIENT\n");
                            found = 1;
                            break;
                        }
                    }

                    if (!found){
                        int bytesSent = send(clientSocket, "KEY NOT FOUND", strlen("KEY NOT FOUND"), 0);
                        if (bytesSent != strlen("KEY NOT FOUND"))
                        {
                            printf("Error while sending message to client");
                            exit(0);
                        }
                    }

                    
                }

                if ((strcmp(substring, "del")) == 0)
                {
                    printf("DEL COMMAND\n");
                    printf("MESSAGE: %s\n", msg);

                    // if ((sscanf(msg, "del %d", &keytofind)) != 1){
                    //     printf("COULD NOT PARSE STRING\n");
                    // }
                    substring = strtok(NULL, " ");
                    keytofind = atoi(substring);
                    printf("SUBSTRING %s KEY %d\n", substring, keytofind);

                    fp = fopen(DATABASE_FILE, "r");
                    if (fp == NULL)
                    {
                        printf("File open error");
                        return 1;
                    }

                    tfp = fopen(TEMP_FILE, "w");


                    // READ FROM FILE LINE-BY-LINE
                    int found = 0;
                    while (fgets(line, sizeof(line), fp) != NULL)
                    {
                        if (sscanf(line, "%d", &key) != 1)
                        {
                            exit(1);
                        }

                        if (key == keytofind)
                        {
                            printf("FOUND KEY");
                            continue;
                        }

                        fputs(line, tfp);

                    }

                    fclose(fp);
                    fclose(tfp);

                    // Replace the original database file with the temporary file
                    if (rename(TEMP_FILE, DATABASE_FILE) != 0)
                    {
                        perror("Error renaming file");
                        return 1;
                    }
                    int bytesSent = send(clientSocket, "key deleted", strlen("key deleted"), 0);
                    if (bytesSent != strlen("key deleted"))
                    {
                        printf("Error while sending message to client");
                        exit(0);
                    }
                    printf("Key %d deleted successfully.\n", keytofind);
                }

                if ((strcmp(substring, "bye")) == 0)
                {
                    printf("BYE COMMAND\n");

                    int bytesSent = send(clientSocket, "goodbye", strlen("goodbye"), 0);
                    if (bytesSent != strlen("goodbye"))
                    {
                        printf("Error while sending message to client");
                        exit(0);
                    }

                    printf("SENT GOODBYE REPLY\nTERMINATING CONNECTION\n");
                    close(clientSocket);
                    break;
                }
            }
        }

        close(clientSocket);
    }

    close(serverSocket);
}