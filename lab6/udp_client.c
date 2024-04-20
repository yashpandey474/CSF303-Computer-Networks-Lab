/*
Simple udp client with stop and wait functionality
*/

// Max length of buffer
// The port on which to send data
#include "packet.h"
int main(void)
{
    struct sockaddr_in si_other;
    int s, i, slen = sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];
    DATA_PKT send_pkt, rcv_ack;
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");
    int state = 0;

    struct timeval timer;
    timer.tv_sec = 5;
    timer.tv_usec = 0;

    int retval;
    fd_set rfds;

    while (1)
    {

        printf("CURRENT STATE: %d\n", state);
        FD_ZERO(&rfds);
        FD_SET(s, &rfds);

        switch (state)
        {
        case 0:
            printf("Enter message 0: ");
            gets(send_pkt.data);

            printf("INPUT MESSAGE: %s\n", send_pkt.data);

            send_pkt.sq_no = 0;

            send_pkt.ack_or_data = 1;
            if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                die("sendto()");
            }
            state = 1;

        
            break;

        case 1:
            printf("WAITING ON SELECT FOR ACK 0\n");

            FD_ZERO(&rfds);
            FD_SET(s, &rfds);

            // START TIMER
            timer.tv_sec = 5;
            timer.tv_usec = 0;


            retval = select(s + 1, &rfds, NULL, NULL, &timer);

            printf("RETVAL FROM SELECT FOR 0: %d", retval);
            // ERROR ON SELECT
            if (retval == -1)
            {

                die("select()");
            }

            // NON-ZERO: READY TO BE READ
            else if (retval)
            {
                printf("READING FROM SOCKET\n");
                // waiting for ACK 0
                if (recvfrom(s, &rcv_ack, sizeof(rcv_ack), 0, (struct sockaddr *)&si_other, &slen) == -1)
                {
                    die("recvfrom()");
                }

                // THIS SIMULATES ACK LOSS
                if (dropPacket())
                {
                    printf("PACKET %d RANDOMLY LOST\n", rcv_ack.sq_no);
                    break;
                }

                else
                {
                    if (rcv_ack.sq_no == 0)
                    {
                        printf("Received ack seq. no. %d\n", rcv_ack.sq_no);
                        state = 2;
                        break;
                    }

                    break;
                }
            }

            // TIMEOUT OCCURED! [RETVAL = 0]
            else
            {
                printf("TIMEOUT AT SENDER!\n");

                // RETRANSMIT THE PACKET
                if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                {
                    die("sendto()");
                }

                printf("RETRANSMITTED PACKET\n");

                // RESTART TIMER
                timer.tv_sec = 5;
                timer.tv_usec = 0;

                break;
            }

        case 2:
            printf("Enter message 1: ");
            // wait for sending packet with seq. no. 1
            gets(send_pkt.data);
            send_pkt.sq_no = 1;
            if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                die("sendto()");
            }
            state = 3;

            // START TIMER
            timer.tv_sec = 5;
            timer.tv_usec = 0;

            break;

        case 3:
            FD_ZERO(&rfds);
            FD_SET(s, &rfds);

            // START TIMER
            timer.tv_sec = 5;
            timer.tv_usec = 0;


            printf("WAITING ON SELECT FOR ACK 1\n");
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

                if (dropPacket())
                {
                    printf("PACKET RANDOMLY LOST");
                    break;
                }

                else
                {
                    if (rcv_ack.sq_no == 1)
                    {
                        printf("Received ack seq. no. %d\n", rcv_ack.sq_no);
                        state = 0;

                        // RESET TIMER
                        timer.tv_sec = 5;
                        timer.tv_usec = 0;

                        break;
                    }

                    break;
                }
            }

            // TIMEOUT OCCURED! [RETVAL = 0]
            else
            {
                printf("TIMEOUT AT SENDER!\n");

                // RETRANSMIT THE PACKET
                if (sendto(s, &send_pkt, sizeof(send_pkt), 0, (struct sockaddr *)&si_other, slen) == -1)
                {
                    die("sendto()");
                }

                break;
            }
        }
    }
    close(s);
    return 0;
}