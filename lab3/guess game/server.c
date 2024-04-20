#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAXPENDING 3
#define BUFFERSIZE 32
#define SERVER_PORT_NO 12345

int generateRandomNumber(int lowerBound, int upperBound){
    int number = lowerBound + rand() % (upperBound - lowerBound + 1);

    printf("Generated: %d\n", number);

    return number;
}
int main(){

    //CREATE A TCP SOCKET
    int serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket < 0){
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
    printf ("Now Listening\n");


    char msg[BUFFERSIZE];
    int clientLength = sizeof(clientAddress);
    

    while (1)
    {
        int clientSocket = accept (serverSocket, (struct sockaddr*)
        &clientAddress, &clientLength);

        int number = generateRandomNumber(1, 6);

        if (clientLength < 0) {
            printf ("Error in client socket"); exit(0);}
        printf ("Handling Client %s\n", inet_ntoa(clientAddress.sin_addr));
        
        
        int temp2 = recv(clientSocket, msg, BUFFERSIZE, 0);
        if (temp2 < 0)
        { 
            printf ("problem in temp 2");
            exit (0);
        }

        int guess = atoi(msg);

        printf("Actual number: %d\n", number);

        printf("Client's guess is: %d\n", guess);

        if (guess == number){
            printf("Client guessed successfully\n");
            sprintf(msg, "correct", number);
        }
        else{
            printf("Client guessed incorrectly\n");
            sprintf(msg, "Incorrect %d", number);
            
        }

        int bytesSent = send(clientSocket, msg, strlen(msg), 0);
        if (bytesSent != strlen(msg))
        {
            printf("Error while sending message to client");
            exit(0);
        }


        close (clientSocket);
    }
    close (serverSocket);
    

}