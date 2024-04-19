
/*Now, has 4 states instead of 2

1 - Receive packet from C1 with Seq No 0
2 - Receive packet from C1 with Seq No 1
3 - Receive packet from C1 with Seq No 0
4 - Receive packet from C1 with Seq No 1

 */
#include "packet.h"



int main()
{
    srand(time(NULL));

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
            {
                die("recvfrom()");
            }

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
                               slen) == -1)
                    {
                        die("sendto()");
                    }
                    // printf("ACK %d SENT\n", ack_pkt.seq_no);
                    state = next_state;

                    //NEW DATA -> WRITE TO FILE
                    if (ftell(fp) != 0){
                        fputs(",", fp);
                    }
                    fputs(rcv_pkt.data, fp);
                    fflush(fp);

                    printf("WROTE TO THE FILE\n");
                }

                else
                {
                    // RETRANSMIT OLD ACK
                    if (sendto(s, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other,
                               slen) == -1)
                    {
                        die("sendto()");
                    }

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
            {
                die("recvfrom()");
            }

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
                }

                else
                {
                    // RETRANSMIT OLD ACK
                    if (sendto(s2, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other_2,
                               slen2) == -1)
                    {
                        die("sendto()");
                    }

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
}