<<<<<<< HEAD
#include "packet.h"

// FUNCTION FOR CHECKING SOCKET WITH TIMEOUT
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
    printf("TIMEOUT AT SENDER FOR SEQ NO. %d\n", ack_no);
    if (sendto(sockfd, pkt, sizeof(PACKET), 0, (const struct sockaddr *)dest_addr, slen) == -1)
    {
        die("sendto()");
    }
    printf("RESENT DATA: %s Seq No %d OF SIZE %d Bytes\n", pkt->data, pkt->seq_no, pkt->len);
}

void handle_ack(int sockfd, struct sockaddr_in *si_other, socklen_t slen, PACKET *rcv_ack, int *state, int ack_no, int next_state)
{
    printf("READING FROM SOCKET\n");
    if (recvfrom(sockfd, rcv_ack, sizeof(PACKET), 0, (struct sockaddr *)si_other, &slen) == -1)
    {
        die("recvfrom()");
    }
    printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack->seq_no, rcv_ack->data);

    if (rcv_ack->seq_no == ack_no)
    {
        printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack->seq_no);
        *state = next_state;
    }
    else if (rcv_ack->seq_no == 1 && *state == 3)
    {
        printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack->seq_no);
        *state = 0;
    }
}
 
void transmit_packet(int sockfd, const struct sockaddr_in *dest_addr, PACKET *pkt)
{
    sendto(sockfd, pkt, sizeof(*pkt), 0, (const struct sockaddr *)dest_addr, sizeof(struct sockaddr_in));
    printf("Transmitted Packet with Sequence No. %d And Data: %s\n", pkt->seq_no, pkt->data);
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
#define MAX_DATA_SIZE 256
#define TIMEOUT_SEC 5

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

// FUNCTION FOR CHECKING SOCKET WITH TIMEOUT
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
    if (fscanf(fp, "%[^,]", value_str) != EOF)
    {
        // Convert string to integer value
        strncpy(pkt->data, value_str, MAX_DATA_SIZE);
        pkt->len = strlen(value_str);
        pkt->seq_no = 0;
    }
    else
    {
        // End of file reached, set length to 0
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
    printf("TIMEOUT AT SENDER FOR SEQ NO. %d\n", ack_no);
    if (sendto(sockfd, pkt, sizeof(PACKET), 0, (const struct sockaddr *)dest_addr, slen) == -1)
    {
        die("sendto()");
    }
    printf("RESENT DATA: %s Seq No %d OF SIZE %d Bytes\n", pkt->data, pkt->seq_no, pkt->len);
}

void handle_ack(int sockfd, struct sockaddr_in *si_other, socklen_t slen, PACKET *rcv_ack, int *state, int ack_no, int next_state)
{
    printf("READING FROM SOCKET\n");
    if (recvfrom(sockfd, rcv_ack, sizeof(PACKET), 0, (struct sockaddr *)si_other, &slen) == -1)
    {
        die("recvfrom()");
    }
    printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack->seq_no, rcv_ack->data);

    if (rcv_ack->seq_no == ack_no)
    {
        printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack->seq_no);
        *state = next_state;
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
    printf("Transmitted Packet with Sequence No. %d\n", pkt->seq_no);
}

int main()
{
    // DEFINE PACKET
    PACKET pkt, rcv_ack;

    struct sockaddr_in si_other;
<<<<<<< HEAD
    int s, slen = sizeof(si_other);
>>>>>>> aed9db2 (Initial practice commit)
=======
    int s;
>>>>>>> e3bd83d (Try)

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
<<<<<<< HEAD
    si_other.sin_port = htons(PORT1);
=======
    si_other.sin_port = htons(PORT);
>>>>>>> aed9db2 (Initial practice commit)
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");

    // OPEN FILE
    FILE *fp;
<<<<<<< HEAD
<<<<<<< HEAD
    fp = fopen("name.txt", "r");
=======
    fp = fopen("input.txt", "r");
>>>>>>> aed9db2 (Initial practice commit)
=======
    fp = fopen("name.txt", "r");
>>>>>>> f132a61 (Tryr)

    if (fp == NULL)
    {
        printf("COULD NOT OPEN FILE");
        exit(1);
    }

<<<<<<< HEAD
<<<<<<< HEAD
    // INITIAL STATE
    int state = 0, retval = 0, ack_no = 0, next_state = 1, slen = sizeof(si_other);

    // TIMER
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(s, &rfds);

    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
=======
    // SET A TIMER
    struct timeval timer;
    timer.tv_sec = 5;
    timer.tv_usec = 0;

    int retval;
    fd_set rfds;

    // INITIAL STATE
    int state = 0;
>>>>>>> aed9db2 (Initial practice commit)
=======
    // INITIAL STATE
    int state = 0, retval = 0, ack_no = 0, next_state = 1, slen = sizeof(si_other);

    // TIMER
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(s, &rfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;
>>>>>>> e3bd83d (Try)

    while (1)
    {

        printf("CURRENT STATE: %d\n", state);
<<<<<<< HEAD
<<<<<<< HEAD
=======
        FD_ZERO(&rfds);
        FD_SET(s, &rfds);
>>>>>>> aed9db2 (Initial practice commit)
=======
>>>>>>> e3bd83d (Try)

        switch (state)
        {

<<<<<<< HEAD
<<<<<<< HEAD
        case 2:
        case 0:
            read_packet(fp, &pkt);
            if (pkt.len == 0)
            {
                close_system(fp, s);
=======
=======
        case 2:
>>>>>>> e3bd83d (Try)
        case 0:
            read_packet(fp, &pkt);
            if (pkt.len == 0)
            {
<<<<<<< HEAD
                // Remove newline character if it exists
                int length = strlen(pkt.data);
                if (pkt.data[length - 1] == '\n')
                {
                    pkt.data[length - 1] = '\0';
                    length--; // Adjust length if newline was removed
                }
                pkt.len = length;

                char *token = strtok(pkt.data, ",");
                while (token != NULL) {
                    // Send each token as a UDP packet
                    sendto(s, token, strlen(token), 0, (const struct sockaddr *)&si_other, sizeof(si_other));
                    printf("Sent: %s\n", token);

                    // Wait for ACK
                    char ack;
                    recvfrom(s, &ack, sizeof(ack), 0, NULL, NULL);

                    // Get next token
                    token = strtok(NULL, ",");
                }

                printf("Read data: %s, Length: %d\n", pkt.data, pkt.len); // Debug statement
            }
            else
            {
                // End of file or error occurred

                // CLOSE THE FILE
                fclose(fp);
                // CLOSE THE SOCKET
                close(s);
                // EXIT PROGRAM
>>>>>>> aed9db2 (Initial practice commit)
=======
                close_system(fp, s);
>>>>>>> e3bd83d (Try)
                exit(0);
            }

            printf("DATA READ: %s of size %d Bytes\n", pkt.data, pkt.len);

            // SET SEQ. NO.
<<<<<<< HEAD
<<<<<<< HEAD
            pkt.seq_no = state / 2;

            // SEND THE PACKET
            if((sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, sizeof(si_other))) == -1){
                die("sendto()");
            }
            printf("Transmitted Packet with Sequence No. %d And Data: %s\n", pkt.seq_no, pkt.data);
            // CHANGE STATE
            state += 1;

            break;

        case 3:
        case 1:
            FD_ZERO(&rfds);
            FD_SET(s, &rfds);

            tv.tv_sec = TIMEOUT_SEC;
            tv.tv_usec = 0;

            ack_no = (state - 1) / 2;
            next_state = (state + 1) % 4;

            // printf("TIMEOUT VAL: %d\n", tv.tv_sec);
            printf("WAITING ON SELECT FOR ACK %d\n", ack_no);

            // CHECK SOCKET AND TIMEOUT
            retval = select(s + 1, &rfds, NULL, NULL, &tv);

            printf("RETVAL FROM SELECT FOR 0: %d\n", retval);

=======
            pkt.seq_no = 0;
=======
            pkt.seq_no = state / 2;
>>>>>>> e3bd83d (Try)

            // SEND THE PACKET
            transmit_packet(s, &si_other, &pkt);

            // CHANGE STATE
            state += 1;

            break;

        case 3:
        case 1:

            tv.tv_sec = 5;
            tv.tv_usec = 0;

            ack_no = (state - 1) / 2;
            next_state = (state + 1) % 4;

            printf("TIMEOUT VAL: %d\n", tv.tv_sec);
            printf("WAITING ON SELECT FOR ACK %d\n", ack_no);

            // CHECK SOCKET AND TIMEOUT
            retval = select(s + 1, &rfds, NULL, NULL, &tv);

            printf("RETVAL FROM SELECT FOR 0: %d\n", retval);
<<<<<<< HEAD
>>>>>>> aed9db2 (Initial practice commit)
=======

>>>>>>> e3bd83d (Try)
            // ERROR ON SELECT
            if (retval == -1)
            {

                die("select()");
            }

            // NON-ZERO: READY TO BE READ
            else if (retval)
            {
<<<<<<< HEAD
<<<<<<< HEAD
                handle_ack(s, &si_other, slen, &rcv_ack, &state, ack_no, next_state);
=======
                printf("READING FROM SOCKET\n");
                // waiting for ACK 0
                if (recvfrom(s, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *)&si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }

                printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack.seq_no, rcv_ack.data);

                // CHECK SEQ NO OF ACK
                if (rcv_ack.seq_no == 0)
                {
                    printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack.seq_no);
                    state = 2;

                    // RESTART TIMER
                    timer.tv_sec = 5;
                    timer.tv_usec = 0;
                }
>>>>>>> aed9db2 (Initial practice commit)
=======
                handle_ack(s, &si_other, slen, &rcv_ack, &state, ack_no, next_state);
>>>>>>> e3bd83d (Try)
            }

            // TIMEOUT OCCURED! [RETVAL = 0]
            else
            {
<<<<<<< HEAD
<<<<<<< HEAD
                handle_timeout(s, &si_other, slen, &pkt, &state, ack_no);
=======
                printf("TIMEOUT AT SENDER FOR SEQ NO. 0\n");
                // RETRANSMIT THE PACKET
                if (sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                {
                    die("sendto()");
                }

                printf("RESENT DATA: Seq No %d OF SIZE %d Bytes\n", pkt.seq_no, pkt.len);

                // RESTART TIMER
                timer.tv_sec = 5;
                timer.tv_usec = 0;
            }
            break;

        case 2:
            if (fgets(pkt.data, MAX_DATA_SIZE, fp) != NULL)
            {
                // Remove newline character if it exists
                int length = strlen(pkt.data);
                if (pkt.data[length - 1] == '\n')
                {
                    pkt.data[length - 1] = '\0';
                    length--; // Adjust length if newline was removed
                }
                pkt.len = length;

                printf("Read data: %s, Length: %d\n", pkt.data, pkt.len); // Debug statement
            }
            else
            {
                // End of file or error occurred
                fclose(fp);
                close(s);
                exit(0);
            }

            printf("DATA READ: %s of size %d Bytes\n", pkt.data, pkt.len);
            printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack.seq_no, rcv_ack.data);

            // SET SEQ NO
            pkt.seq_no = 1;

            if (sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                die("sendto()");
            }

            printf("Transmitted Packet with Sequence No. 1\n");

            // CHANGE STATE
            state = 3;

            // START TIMER
            timer.tv_sec = 5;
            timer.tv_usec = 0;
            printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack.seq_no, rcv_ack.data);

            break;

        case 3:
            FD_ZERO(&rfds);
            FD_SET(s, &rfds);

            // printf("WAITING ON SELECT FOR ACK 1\n");
            retval = select(s + 1, &rfds, NULL, NULL, &timer);

            printf("Retval from select: %d\n", retval);

            // ERROR ON SELECTprintf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack.seq_no, rcv_ack.data);

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

                printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack.seq_no, rcv_ack.data);

                if (rcv_ack.seq_no == 1)
                {
                    printf("RCVD ACK: for PKT with seq. no. %d\n", rcv_ack.seq_no);
                    state = 0;

                    // RESET TIMER
                    timer.tv_sec = 5;
                    timer.tv_usec = 0;
                }
            }

            // TIMEOUT OCCURED! [RETVAL = 0]
            else
            {
                printf("TIMEOUT AT SENDER FOR SEQ NO. 1 PACKET!\n");

                // RETRANSMIT THE PACKET
                if (sendto(s, &pkt, sizeof(pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                {
                    die("sendto()");
                }

                // RESET TIMER
                timer.tv_sec = 5;
                timer.tv_usec = 0;
>>>>>>> aed9db2 (Initial practice commit)
=======
                handle_timeout(s, &si_other, slen, &pkt, &state, ack_no);
>>>>>>> e3bd83d (Try)
            }
            break;
        }
    }
    return 0;
}