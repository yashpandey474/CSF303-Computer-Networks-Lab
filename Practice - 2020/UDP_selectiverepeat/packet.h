#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>

#define BUFFER_SIZE 1000
#define PACKET_SIZE 100
#define SERVER_PORT_R1 5001
#define SERVER_PORT_R2 5002
#define R1_PORT 5001
#define R2_PORT 5002
#define PDR 0.1
#define WINDOW_SIZE BUFFER_SIZE/PACKET_SIZE
#define RETRANSMISSION_TIME 2
#define MAX_SEQ_NO 1000
/*
The size (number of bytes) of the payload
b. Whether the packet is the last packet or not?
c.  You  should  also  decide  how  you  wish  to  implement  and  keep  track  of  sequence
numbers  and  ACKs,  and  include  the  necessary  information  in  the  header  (or  in  the
packet structure). The packet structure should be declared in a separate header file
“packet.h”. Both client and the server programs will make use of this structure.

*/
typedef struct{
    int final_packet;
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
    double random_num = (double) rand() / RAND_MAX;

    //DROP PACKETS WITH PROBABILTY PDR
    if (random_num < PDR){
        return 1;
    }

    return 0;
}

void randomDelay(){
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = rand() % 2000000;
    nanosleep(&delay, NULL);
}