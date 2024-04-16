#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAXPENDING 3
#define BUFFERSIZE 32
#define SERVER_PORT_NO 12343

//ACCEPT CONNECTIONS FROM MULTIPLE CLIENTS
int main(){

    //CREATE A TCP SOCKET TO LISTEN TO CONNECTIONS
    int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0){
        printf("ERROR WHILE SERVER SOCKET CREATION\n");
        exit(1);
    }
    printf("SERVER SOCKET CREATED\n");

    
    struct sockaddr_in serverAddress, clientAddress;
    
    //ZERO OUT ADDRESS
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT_NO);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("SERVER ADDRESS ASSIGNED\n");

    //BIND
    int temp = bind(serverSocket, (struct sockaddr*) &serverAddress,
    sizeof(serverAddress));
    if (temp < 0)
    {   printf ("Error while binding\n");
        exit (0);
    }

    printf ("Binding successful\n");
    int temp1 = listen(serverSocket, MAXPENDING);
    if (temp1 < 0)
    { 
        printf ("Error in listen");
        exit (0);
    }
    printf ("Now Listening at port: %d\n", SERVER_PORT_NO);


    char msg[BUFFERSIZE];
    int clientLength = sizeof(clientAddress), childpid, clientSocket;
    memset(msg, 0, BUFFERSIZE);
    int terminate = 0;

    while (!terminate)
    {
        //ACCEPT A CONNECTION
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientLength);

        if (clientSocket < 0)
        {
            printf("Error in client socket\n");
            exit(1);
        }


        printf("Connection accepted from %s:%d\n", inet_ntoa(clientAddress.sin_addr),
        ntohs(clientAddress.sin_port)
        );

        //NEW PROCESS CREATED
        if ((childpid = fork()) == 0){  

            //CLOSE SERVER SOCK FOR THIS PROCESS
            close(serverSocket);

            //CONTINOUSLY ECHO BACK MESSAGE
            while (1){

                // SET MSG TO EMPTY
                memset(msg, 0, BUFFERSIZE);

                //RECEIVE FROM CLIENT
                int temp2 = recv(clientSocket, msg, BUFFERSIZE, 0);
                if (temp2 < 0)
                {
                    printf("problem in temp 2");
                    exit(0);
                }


                printf("RECEIVED MESSAGE: %s\n", msg);

                // CLIENT WANTS TO TERMINATE CONNECTION
                if (strcmp(msg, "bye") == 0)
                {
                    terminate = 1;
                    break;
                }

                printf("ENTER MESSAGE FOR CLIENT\n");
                gets(msg);

                //SEND TO CLIENT
                int bytesSent = send(clientSocket, msg, strlen(msg), 0);
                if (bytesSent != strlen(msg))
                {
                    printf("Error while sending message to client");
                    exit(0);
                }
            }
        }else{
            printf("NEW PROCESS SPAWNED: %d\n", childpid);
        }
        close(clientSocket);
    }
    printf("I HAVE TERMINATED: %d\n", getpid());
    
    close (serverSocket);
}