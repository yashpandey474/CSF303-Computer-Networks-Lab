/* Simple udp server with stop and wait functionality */
#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define BUFLEN 512
#define PORT 8886
#define PDR 0.1
// Max length of buffer
// The port on which to listen for incoming data

#include "packet.h"

int main(void)
{
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;
    // char buf[BUFLEN];
    DATA_PKT rcv_pkt, ack_pkt;
    // create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    // zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind socket to port
    if (bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
    {
        die("bind");
    }
    int state = 0;
    while (1)
    {
        switch (state)
        {
        case 0:
        {
            printf("Waiting for packet 0 from sender...\n");
            fflush(stdout);
            // try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(s, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_other, &slen)) == -1)
            {
                die("recvfrom()");
            }

            if (dropPacket())
            {
                printf("PACKET WITH SEQ NO %d RANDOMLY LOST\n", rcv_pkt.sq_no);
                break;
            }
            else
            {
                if (rcv_pkt.sq_no == 0)
                {
                    printf("Packet received with seq. no. %d and Packet content is = %s\n", rcv_pkt.sq_no, rcv_pkt.data);
                    ack_pkt.sq_no = 0;
                    ack_pkt.ack_or_data = 0;
                    if (sendto(s, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other,
                               slen) == -1)
                    {
                        die("sendto()");
                    }

                    printf("ACK %d SENT\n", ack_pkt.sq_no);
                    state = 1;
                    break;
                }

                else
                {
                    // RETRANSMIT OLD ACK
                    ack_pkt.sq_no = 1;
                    ack_pkt.ack_or_data = 0;
                    if (sendto(s, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other,
                               slen) == -1)
                    {
                        die("sendto()");
                    }

                    printf("RETRANSMITTED ACK %d\n", ack_pkt.sq_no);

                    break;
                }
            }
        }
        case 1:
        {
            printf("Waiting for packet 1 from sender...\n");
            fflush(stdout);
            // try to receive some data, this is a blocking call
            if ((recv_len = recvfrom(s, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_other, &slen)) == -1)
            {
                die("recvfrom()");
            }

            if (dropPacket())
            {
                printf("PACKET WITH SEQ NO %d RANDOMLY LOST\n", rcv_pkt.sq_no);
                break;
            }
            else
            {

                if (rcv_pkt.sq_no == 1)
                {
                    printf("Packet received with seq. no.= %d and Packet content is= %s\n", rcv_pkt.sq_no, rcv_pkt.data);
                    ack_pkt.sq_no = 1;
                    if (sendto(s, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other,
                               slen) == -1)
                    {
                        die("sendto()");
                    }
                    state = 0;
                    break;
                }

                else
                {
                    // RETRANSMIT OLD ACK
                    ack_pkt.sq_no = 0;
                    if (sendto(s, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other,
                               slen) == -1)
                    {
                        die("sendto()");
                    }

                    printf("RETRANSMITTED ACK %d\n", ack_pkt.sq_no);
                    break;
                }
            }
        }
        }
    }
    close(s);
    return 0;
}