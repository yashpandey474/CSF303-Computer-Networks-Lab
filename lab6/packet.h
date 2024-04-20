#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFLEN 512
#define PORT 8886

// PROBABILITY OF PACKET LOSS AS 10%
#define PDR 0.1

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

typedef struct
{
    int sq_no;
    int ack_or_data;
    char data[BUFLEN];
} DATA_PKT;
