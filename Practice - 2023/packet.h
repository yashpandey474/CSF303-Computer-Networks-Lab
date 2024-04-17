#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

#define PORT1 5002
#define PORT2 5003
#define BUFLEN 256
#define MAX_DATA_SIZE 256
#define TIMEOUT_SEC 5
// PROBABILITY OF PACKET LOSS [BETWEEN 0 AND 1]
#define PDR 0.1 // 10% PACKET LOSS FOR TESTING

typedef struct
{
    int ACK_OR_DATA;

    char data[BUFLEN];
    // BYTE SEQUENCE NUMBER OF FIRST BYTE OF PACKET
    int seq_no;
    // NUMBER OF BYTES
    int len;
    // DATA OR ACK
    int type;
    //FINAL PACKET IF 1
    int final_packet;

} PACKET;

void die(char *s)
{
    perror(s);
    exit(1);
}

int dropPacket()
{
    double random_num = (double)rand() / RAND_MAX;

    if (random_num < PDR)
    {
        return 1;
    }
    return 0;
}