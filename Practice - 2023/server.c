<<<<<<< HEAD

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
#include <time.h>

#define PORT 5002
#define BUFLEN 256

// PROBABILITY OF PACKET LOSS [BETWEEN 0 AND 1]
#define PDR 0.1 // 10% PACKET LOSS FOR TESTING

typedef struct
{
    char data[BUFLEN];
    // BYTE SEQUENCE NUMBER OF FIRST BYTE OF PACKET
    int seq_no;
    // NUMBER OF BYTES
    int len;
    // DATA OR ACK
    int type;

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
>>>>>>> aed9db2 (Initial practice commit)

/*Now, has 4 states instead of 2

1 - Receive packet from C1 with Seq No 0
2 - Receive packet from C1 with Seq No 1
3 - Receive packet from C1 with Seq No 0
4 - Receive packet from C1 with Seq No 1

 */
<<<<<<< HEAD
#include "packet.h"


=======

int createAndBindSocket(int *s, struct sockaddr_in *si_me, int port)
{
    // create a UDP socket
    if ((*s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    // zero out the structure
    memset(si_me, 0, sizeof(*si_me));
    si_me->sin_family = AF_INET;
    si_me->sin_port = htons(port);
    si_me->sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to port
    if (bind(*s, (struct sockaddr *)si_me, sizeof(*si_me)) == -1)
    {
        die("bind");
    }
}
>>>>>>> aed9db2 (Initial practice commit)

int main()
{
    srand(time(NULL));

<<<<<<< HEAD
    struct sockaddr_in si_me, si_me_2, si_other, si_other_2;
    int s, s2, slen = sizeof(si_other), slen2 = sizeof(si_other_2), recv_len;
    // char buf[BUFLEN];
    PACKET rcv_pkt, ack_pkt;
    // create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    if ((s2 = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    // zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT1);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    memset((char *)&si_me_2, 0, sizeof(si_me_2));
    si_me_2.sin_family = AF_INET;
    si_me_2.sin_port = htons(PORT2);
    si_me_2.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind socket to port
    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
    {
        die("bind");
    }
    if (bind(s2, (struct sockaddr *)&si_me_2, sizeof(si_me_2)) == -1)
    {
        die("bind");
    }

    int state = 0, expected_seq,next_state;

    //OPEN FILE TO WRITE DATA TO
    FILE *fp = fopen("list.txt", "w");
    int haveWritten = 0, completed = 0;
    

    while (!completed)
    {
        expected_seq = (state / 2);
        next_state = (state + 1) % 4;

        switch (state)
        {

        case 2:
        case 0:
            printf("STATE %d NEXT_STATE %d EXPECTED :%d\n", state, next_state, expected_seq);

            // printf("Waiting for packet 0 from sender...\n");
            fflush(stdout);

            // try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(s, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_other, &slen)) == -1)
=======
    // FOR CLIENT - 1
    struct sockaddr_in si_me_1, si_me_2, si_client_1, si_client_2;
    int s1, s2, i, slen = sizeof(si_client_1), recv_len;
    PACKET rcv_pkt_1, rcv_pkt_2, ack_pkt_1, ack_pkt_2;

    int state = 0;
    while (1)
    {
        switch (state)
        {
        case 0:
        {
            printf("Waiting for packet 0 from Client 1...\n");
            fflush(stdout);
            // try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(s1, &rcv_pkt_1, sizeof(rcv_pkt_1), 0, (struct sockaddr *)&si_client_1, &slen)) == -1)
            {
                die("recvfrom()");
            }

            printf("RECEIVED PACKET FROM CLIENT 1");

            // DROP PACKET WITH PROB PDR
            if (dropPacket())
            {
                printf("PACKET WITH SEQ NO %d RANDOMLY LOST\n", rcv_pkt_1.seq_no);
                break;
            }
            else
            {
                printf("PACKET NOT LOST\n");
                if (rcv_pkt_1.seq_no == 0)
                {
                    printf("Packet received with seq. no. %d and Packet content is = %s\n", rcv_pkt_1.seq_no, rcv_pkt_1.data);
                    ack_pkt_1.seq_no = 0;
                    if (sendto(s1, &ack_pkt_1, sizeof(ack_pkt_1), 0, (struct sockaddr *)&si_client_1,
                               slen) == -1)
                    {
                        die("sendto()");
                    }
                    printf("ACK %d SENT\n", ack_pkt_1.seq_no);
                    state = 1;
                    break;
                }

                else
                {
                    // RETRANSMIT OLD ACK
                    ack_pkt_1.seq_no = 1;
                    if (sendto(s, &ack_pkt_1, sizeof(ack_pkt_1), 0, (struct sockaddr *)&si_client_1,
                               slen) == -1)
                    {
                        die("sendto()");
                    }

                    printf("RETRANSMITTED ACK %d\n", ack_pkt_1.seq_no);

                    break;
                }
            }
        }

        case 1:
        {
            printf("Waiting for packet 0 from Client 2...\n");
            fflush(stdout);
            // try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(s2, &rcv_pkt_2, sizeof(rcv_pkt_2), 0, (struct sockaddr *)&si_client_2, &slen)) == -1)
            {
                die("recvfrom()");
            }

            printf("RECEIVED PACKET");

            // DROP PACKET WITH PROB PDR
            if (dropPacket())
            {
                printf("PACKET WITH SEQ NO %d RANDOMLY LOST\n", rcv_pkt_2.seq_no);
                break;
            }
            else
            {
                printf("PACKET NOT LOST");
                if (rcv_pkt_2.seq_no == 0)
                {
                    printf("Packet received with seq. no. %d and Packet content is = %s\n", rcv_pkt_2.seq_no, rcv_pkt_2.data);
                    ack_pkt_2.seq_no = 0;
                    if (sendto(s2, &ack_pkt_2, sizeof(ack_pkt_2), 0, (struct sockaddr *)&si_client_2,
                               slen) == -1)
                    {
                        die("sendto()");
                    }
                    printf("ACK %d SENT\n", ack_pkt_2.seq_no);
                    state = 2;
                    break;
                }

                else
                {
                    // RETRANSMIT OLD ACK
                    ack_pkt_2.seq_no = 1;
                    if (sendto(s2, &ack_pkt_2, sizeof(ack_pkt_2), 0, (struct sockaddr *)&si_client_2,
                               slen) == -1)
                    {
                        die("sendto()");
                    }

                    printf("RETRANSMITTED ACK %d\n", ack_pkt_2.seq_no);

                    break;
                }
            }
        }

        case 2:
        {
            printf("Waiting for packet 1 from client C1...\n");
            fflush(stdout);
            // try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(s1, &rcv_pkt_1, sizeof(rcv_pkt_1), 0, (struct sockaddr *)&si_client_1, &slen)) == -1)
>>>>>>> aed9db2 (Initial practice commit)
            {
                die("recvfrom()");
            }

<<<<<<< HEAD
            // printf("RECEIVED PACKET");

            if (dropPacket())
            {
                // printf("PACKET WITH SEQ NO %d RANDOMLY LOST\n", rcv_pkt.seq_no);
                printf("DROP DATA:Seq.No. %d of size %d Bytes\n", rcv_pkt.seq_no, rcv_pkt.len);
            }
            else
            {
                // printf("PACKET NOT LOST");
                printf("RCVD DATA: %s Seq.No %d of size %d Bytes\n", rcv_pkt.data, rcv_pkt.seq_no, rcv_pkt.len);
                ack_pkt.seq_no = rcv_pkt.seq_no;
                if (rcv_pkt.seq_no == expected_seq)
                {
                    if (sendto(s, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other,
=======
            printf("PACKET RECEIVED WITH SEQ NO: %d AND DATA: %s\n", rcv_pkt_1.seq_no, rcv_pkt_1.data);

            if (dropPacket())
            {
                printf("PACKET WITH SEQ NO %d RANDOMLY LOST\n", rcv_pkt_1.seq_no);
                break;
            }
            else
            {

                if (rcv_pkt_1.seq_no == 1)
                {
                    printf("Packet received with seq. no.= %d and Packet content is= %s\n", rcv_pkt_1.seq_no, rcv_pkt_1.data);
                    ack_pkt_1.seq_no = 1;
                    if (sendto(s1, &ack_pkt_1, sizeof(ack_pkt_1), 0, (struct sockaddr *)&si_client_1,
>>>>>>> aed9db2 (Initial practice commit)
                               slen) == -1)
                    {
                        die("sendto()");
                    }
<<<<<<< HEAD
                    // printf("ACK %d SENT\n", ack_pkt.seq_no);
                    state = next_state;

                    //NEW DATA -> WRITE TO FILE
                    if (ftell(fp) != 0){
                        fputs(",", fp);
                    }
                    fputs(rcv_pkt.data, fp);
                    fflush(fp);

                    printf("WROTE TO THE FILE\n");
=======
                    state = 3;
                    break;
>>>>>>> aed9db2 (Initial practice commit)
                }

                else
                {
                    // RETRANSMIT OLD ACK
<<<<<<< HEAD
                    if (sendto(s, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other,
=======
                    ack_pkt_1.seq_no = 0;
                    if (sendto(s1, &ack_pkt_1, sizeof(ack_pkt_1), 0, (struct sockaddr *)&si_client_1,
>>>>>>> aed9db2 (Initial practice commit)
                               slen) == -1)
                    {
                        die("sendto()");
                    }

<<<<<<< HEAD
                    // printf("RETRANSMITTED ACK %d\n", ack_pkt.seq_no);
                }
                printf("SENT ACK: for PKT with Seq.No. %d\n", rcv_pkt.seq_no);
            }
            break;

        case 3:
        case 1:
            printf("STATE %d NEXT_STATE %d EXPECTED :%d\n", state, next_state, expected_seq);

            // printf("Waiting for packet 0 from sender...\n");
            fflush(stdout);

            // try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(s2, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_other_2, &slen2)) == -1)
=======
                    printf("RETRANSMITTED ACK %d\n", ack_pkt_1.seq_no);
                    break;
                }
            }
        }

        case 3:
        {
            printf("Waiting for packet 1 from client C2...\n");
            fflush(stdout);
            // try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(s, &rcv_pkt_1, sizeof(rcv_pkt_1), 0, (struct sockaddr *)&si_client_1, &slen)) == -1)
>>>>>>> aed9db2 (Initial practice commit)
            {
                die("recvfrom()");
            }

<<<<<<< HEAD
            // printf("RECEIVED PACKET");

            if (dropPacket())
            {
                // printf("PACKET WITH SEQ NO %d RANDOMLY LOST\n", rcv_pkt.seq_no);
                printf("DROP DATA:Seq.No. %d of size %d Bytes\n", rcv_pkt.seq_no, rcv_pkt.len);
            }
            else
            {
                // printf("PACKET NOT LOST");
                printf("RCVD DATA: %s Seq.No %d of size %d Bytes\n", rcv_pkt.data, rcv_pkt.seq_no, rcv_pkt.len);
                ack_pkt.seq_no = rcv_pkt.seq_no;
                if (rcv_pkt.seq_no == expected_seq)
                {
                    if (sendto(s2, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other_2,
                               slen2) == -1)
                    {
                        die("sendto()");
                    }
                    // printf("ACK %d SENT\n", ack_pkt.seq_no);
                    state = next_state;

                    // NEW DATA -> WRITE TO FILE
                    if (ftell(fp) != 0)
                    {
                        fputs(",", fp);
                    }
                    fputs(rcv_pkt.data, fp);
                    fflush(fp);


                    printf("WROTE TO THE FILE\n");
=======
            printf("PACKET RECEIVED WITH SEQ NO: %d AND DATA: %s\n", rcv_pkt_1.seq_no, rcv_pkt_1.data);

            if (dropPacket())
            {
                printf("PACKET WITH SEQ NO %d RANDOMLY LOST\n", rcv_pkt_1.seq_no);
                break;
            }
            else
            {

                if (rcv_pkt_1.seq_no == 1)
                {
                    printf("Packet received with seq. no.= %d and Packet content is= %s\n", rcv_pkt_1.seq_no, rcv_pkt_1.data);
                    ack_pkt_1.seq_no = 1;
                    if (sendto(s, &ack_pkt_1, sizeof(ack_pkt_1), 0, (struct sockaddr *)&si_client_1,
                               slen) == -1)
                    {
                        die("sendto()");
                    }
                    state = 0;
                    break;
>>>>>>> aed9db2 (Initial practice commit)
                }

                else
                {
                    // RETRANSMIT OLD ACK
<<<<<<< HEAD
                    if (sendto(s2, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other_2,
                               slen2) == -1)
=======
                    ack_pkt_1.seq_no = 0;
                    if (sendto(s, &ack_pkt_1, sizeof(ack_pkt_1), 0, (struct sockaddr *)&si_client_1,
                               slen) == -1)
>>>>>>> aed9db2 (Initial practice commit)
                    {
                        die("sendto()");
                    }

<<<<<<< HEAD
                    // printf("RETRANSMITTED ACK %d\n", ack_pkt.seq_no);
                }
                printf("SENT ACK: for PKT with Seq.No. %d\n", rcv_pkt.seq_no);
            }
            break;
        }
        }

        fputs(".", fp);
        fclose(fp);
        close(s);
        close(s2);
        return 0;
=======
                    printf("RETRANSMITTED ACK %d\n", ack_pkt_1.seq_no);
                    break;
                }
            }
        }
        }
    }
    close(s);
    return 0;
    // // PACKET LOST WITH 10% PROBABILITY
    // if (rand() % 10 == 0)
    // {
    //     printf("PACKET %d RANDOMLY LOST\n", rcv_ack.sq_no);
    //     break;
    // }

    // else
    // {
    // }
>>>>>>> aed9db2 (Initial practice commit)
}