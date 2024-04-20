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

#define PACKET_SIZE 100
#define BUFFER_SIZE 1000
#define SERVER_PORT 5001
#define WINDOW_SIZE 5
#define MAX_SEQ_NO 1000
#define PDR 0.3
#define TIMEOUT_PERIOD 5

typedef struct{
    int size;
    int isAck;
    int finalPacket;
    int seqNo;
    char payload[PACKET_SIZE + 1];
} PACKET;


void die(char *s)
{
    perror(s);
    exit(1);
}

int dropPacket(){
    double random_num = (double)rand() / RAND_MAX;

    if (random_num <= PDR){
        return 1;
    }

    return 0;
}

void randomDelay()
{
    struct timespec delay;
    delay.tv_sec = 0;

    //DELAY OF ATMOST 2MS
    delay.tv_nsec = rand() % 2000000;

    // printf("ADDED A DELAY OF %ld milliseconds\n", delay.tv_nsec);
    nanosleep(&delay, NULL);
}