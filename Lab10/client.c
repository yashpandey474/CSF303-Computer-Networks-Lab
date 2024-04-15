<<<<<<< HEAD

#include "packet.h"
//FUNCTION FOR CHECKING SOCKET WITH TIMEOUT 
int is_ready(int sockfd)
{
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;

    return select(sockfd + 1, &rfds, NULL, NULL, &tv);
}

void read_packet(FILE *fp, PACKET *pkt)
{
    char value_str[MAX_DATA_SIZE];
    if (fscanf(fp, "%255[^,.\n]%*[,.\n]", value_str) == 1)

    { // Limit the length of read string
        printf("READ DATA: %s\n", value_str);

        // Copy data to packet structure
        strncpy(pkt->data, value_str, MAX_DATA_SIZE - 1); // Prevent buffer overflow
        pkt->data[MAX_DATA_SIZE - 1] = '\0';              // Ensure null-termination
        pkt->len = strlen(pkt->data);                     // Calculate length
        pkt->seq_no = 0;                                  // Set sequence number
    }
    else
    {
        // End of file reached or invalid input, set length to 0
        pkt->len = 0;
    }
}

void close_system(FILE *fp, int sockfd)
{
    if (fp != NULL)
    {
        fclose(fp);
    }
    if (sockfd != -1)
    {
        close(sockfd);
    }
}

void handle_timeout(int sockfd, const struct sockaddr_in *dest_addr, socklen_t slen, PACKET *pkt, int *state, int ack_no)
{
    // printf("TIMEOUT AT SENDER FOR SEQ NO. %d\n", ack_no);
    if (sendto(sockfd, pkt, sizeof(PACKET), 0, (const struct sockaddr *)dest_addr, slen) == -1)
    {
        die("sendto()");
    }
    printf("RESENT DATA: Seq No %d OF SIZE %d Bytes\n", pkt->seq_no, pkt->len);
}

void handle_ack(int sockfd, struct sockaddr_in *si_other, socklen_t slen, PACKET *rcv_ack, int *state, int ack_no, int next_state)
{
    // printf("READING FROM SOCKET\n");
    if (recvfrom(sockfd, rcv_ack, sizeof(PACKET), 0, (struct sockaddr *)si_other, &slen) == -1)
    {
        die("recvfrom()");
    }
    // printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack->seq_no, rcv_ack->data);

    if (rcv_ack->seq_no == ack_no)
    {
        printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack->seq_no);
        *state =  next_state;
        
    }
    else if (rcv_ack->seq_no == 1 && *state == 3)
    {
        printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack->seq_no);
        *state = 0;
    }

    
}

void transmit_packet(int sockfd, const struct sockaddr_in *dest_addr, PACKET *pkt)
{
    sendto(sockfd, pkt, sizeof(PACKET), 0, (const struct sockaddr *)dest_addr, sizeof(struct sockaddr_in));
    // printf("Transmitted Packet with Sequence No. %d\n", pkt->seq_no);
}


int main()
{
    // DEFINE PACKET
    PACKET pkt, rcv_ack;

    struct sockaddr_in si_other;
    int s;
=======
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
#define BUFFLEN 256;

typedef struct
{
    int ACK_OR_DATA;
    char data[256];
    int seq_no;
    int len;
} PACKET;

void die(char *s)
{
    perror(s);
    exit(1);
}

int main()
{
    // DEFINE PACKET
    PACKET pkt;
    PACKET rcv_ack;

    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other);
>>>>>>> d933fc2 (Add lab10 and labsheets)

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
<<<<<<< HEAD

=======
>>>>>>> d933fc2 (Add lab10 and labsheets)
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");

<<<<<<< HEAD
    // OPEN FILE
    FILE *fp;
    fp = fopen("name.txt", "r");
=======
    // READ FROM FILE
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    // SET A TIMER
    struct timeval timer;
    timer.tv_sec = 5;
    timer.tv_usec = 0;
    int retval;
    fd_set rfds;

    // OPEN FILE
    fp = fopen("input.txt", "r");
>>>>>>> d933fc2 (Add lab10 and labsheets)

    if (fp == NULL)
    {
        printf("COULD NOT OPEN FILE");
        exit(1);
    }

<<<<<<< HEAD

    // INITIAL STATE
    int state = 0, retval = 0, ack_no = 0, next_state = 1, slen = sizeof(si_other);

    //TIMER
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(s, &rfds);

    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;


    while (1)
    {

        FD_ZERO(&rfds);
        FD_SET(s, &rfds);

        tv.tv_sec = TIMEOUT_SEC;
        tv.tv_usec = 0;

        // printf("CURRENT STATE: %d\n", state);

        switch (state)
        {
        
        case 2:
        case 0:
            read_packet(fp, &pkt);
            if (pkt.len == 0)
            {
                close_system(fp, s);
                exit(0);
            }

            // printf("DATA READ: %s of size %d Bytes\n", pkt.data, pkt.len);

            // SET SEQ. NO.
            pkt.seq_no = state / 2;

            //SEND THE PACKET
            transmit_packet(s, &si_other, &pkt);

            printf("SENT DATA: Data: %s Seq.No %d of size %d Bytes\n", pkt.data, pkt.seq_no, pkt.len);

            // CHANGE STATE
            state += 1;

            FD_ZERO(&rfds);
            FD_SET(s, &rfds);

            tv.tv_sec = TIMEOUT_SEC;
            tv.tv_usec = 0;

            break;
        

        case 3:
        case 1:
            FD_ZERO(&rfds);
            FD_SET(s, &rfds);

            tv.tv_sec = TIMEOUT_SEC;
            tv.tv_usec = 0;
            
            ack_no = (state - 1) / 2;
            next_state = (state + 1) % 4;
            // printf("WAITING ON SELECT FOR ACK %d\n", ack_no);

            //CHECK SOCKET AND TIMEOUT
            retval = select(s + 1, &rfds, NULL, NULL, &tv);

            // printf("RETVAL FROM SELECT FOR 0: %d\n", retval);


=======
    // INITIAL STATE
    int state = 0;

    // READ FROM FILE
    while (1)
    {

        printf("CURRENT STATE: %d\n", state);
        FD_ZERO(&rfds);
        FD_SET(s, &rfds);

        switch (state)
        {

        case 0:
            // READ FROM FILE
            if ((read = getline(&pkt.data, &len, fp)) == -1)
            {
                break;
            }

            printf("DATA READ: %s\n", pkt.data);

            printf("SENT DATA Seq.No 0 of size %d Bytes\n", len);

            // SET SEQ. NO.
            pkt.seq_no = 0;

            // SET LENGTH
            pkt.len = len;

            // SEND THE PACKET
            if (sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                die("sendto()");
            }

            // CHANGE STATE
            state = 1;

            // START TIMER
            timer.tv_sec = 5;
            timer.tv_usec = 0;

            break;

        case 1:
            printf("WAITING ON SELECT FOR ACK 0\n");

            FD_ZERO(&rfds);
            FD_SET(s, &rfds);

            retval = select(s + 1, &rfds, NULL, NULL, &timer);

            printf("RETVAL FROM SELECT FOR 0: %d", retval);
>>>>>>> d933fc2 (Add lab10 and labsheets)
            // ERROR ON SELECT
            if (retval == -1)
            {

                die("select()");
            }

            // NON-ZERO: READY TO BE READ
            else if (retval)
            {
<<<<<<< HEAD
                handle_ack(s, &si_other, slen, &rcv_ack, &state, ack_no, next_state);
=======
                printf("READING FROM SOCKET\n");
                // waiting for ACK 0
                if (recvfrom(s, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *)&si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }

                // CHECK SEQ NO OF ACK
                if (rcv_ack.seq_no == 0)
                {
                    printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack.seq_no);
                    state = 2;

                    // RESTART TIMER
                    timer.tv_sec = 5;
                    timer.tv_usec = 0;

                    break;
                }

                break;
>>>>>>> d933fc2 (Add lab10 and labsheets)
            }

            // TIMEOUT OCCURED! [RETVAL = 0]
            else
            {
<<<<<<< HEAD
                handle_timeout(s, &si_other, slen, &pkt, &state, ack_no);
            }
            break;
        }
    }
=======
                // RETRANSMIT THE PACKET
                if (sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                {
                    die("sendto()");
                }

                printf("RESENT DATA: Seq No %d OF SIZE %d Bytes\n", pkt.seq_no, pkt.len);

                // RESTART TIMER
                timer.tv_sec = 5;
                timer.tv_usec = 0;

                break;
            }

        case 2:
            // READ FROM FILE
            if ((read = getline(&pkt.data, &len, fp)) == -1)
            {
                break;
            }

            // SET LENGTH
            pkt.len = len;

            // SET SEQ NO
            pkt.seq_no = 1;

            if (sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                die("sendto()");
            }

            // CHANGE STATE
            state = 3;

            // START TIMER
            timer.tv_sec = 5;
            timer.tv_usec = 0;

            break;

        case 3:

            // printf("WAITING ON SELECT FOR ACK 1\n");

            retval = select(s + 1, &rfds, NULL, NULL, &timer);

            // ERROR ON SELECT
            if (retval == -1)
            {
                die("select()");
            }

            // NON-ZERO: READY TO BE READ
            else if (retval)
            {
                // waiting for ACK 0
                if (recvfrom(s, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *)&si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }

                if (rcv_ack.seq_no == 1)
                {
                    printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack.seq_no);
                    state = 0;

                    // RESET TIMER
                    timer.tv_sec = 5;
                    timer.tv_usec = 0;

                    break;
                }

                break;
            }

            // TIMEOUT OCCURED! [RETVAL = 0]
            else
            {
                printf("TIMEOUT AT SENDER!\n");

                // RETRANSMIT THE PACKET
                if (sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                {
                    die("sendto()");
                }

                // RESET TIMER
                timer.tv_sec = 5;
                timer.tv_usec = 0;

                break;
            }
        }
    }

    close(s);
    fclose(fp);
>>>>>>> d933fc2 (Add lab10 and labsheets)
    return 0;
}