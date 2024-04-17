#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>

#define PORT 5002
#define MAX_DATA_SIZE 256
#define TIMEOUT_SEC 5
#define PDR 0.1

typedef struct
{
    int ACK_OR_DATA;
    char data[MAX_DATA_SIZE];
    int seq_no;
    int len;
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

