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

#define BUFFER_SIZE 1000
#define PACKET_SIZE 100
#define SERVER_PORT_R1 5002
#define SERVER_PORT_R2 5003
#define R1_PORT 5000
#define R2_PORT 5001
#define PDR 0.1
#define WINDOW_SIZE 5
#define RETRANSMISSION_TIME 5
#define MAX_SEQ_NO 1000
#define MAXPENDING 2
typedef struct
{
    int seq_no_0;
    int final_packet;
    int channelid;
    int seq_no;
    int ack_or_data;
    char payload[PACKET_SIZE + 1];
    int size;
} PACKET;

void die(char *s)
{
    perror(s);
    exit(1);
}

int dropPacket()
{
    double random_num = (double)rand() / RAND_MAX;

    // DROP PACKETS WITH PROBABILTY PDR
    if (random_num <= PDR)
    {
        return 1;
    }

    return 0;
}

void randomDelay()
{
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = rand() % 2000000;

    // printf("ADDED A DELAY OF %ld milliseconds\n", delay.tv_nsec);
    nanosleep(&delay, NULL);
}

int incrementSeqNo(int seqNo)
{
    seqNo = (seqNo + 1) % (MAX_SEQ_NO + 1);
    return seqNo;
}