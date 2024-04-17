#include "packet.h"

int main()
{
    srand(time(NULL));

    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;
    // char buf[BUFLEN];
    PACKET rcv_pkt, ack_pkt;
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

        case 1:
        case 0:
        {
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
                break;
            }
            else
            {
                // printf("PACKET NOT LOST");
                printf("RCVD DATA: %s Seq.No %d of size %d Bytes\n", rcv_pkt.data, rcv_pkt.seq_no, rcv_pkt.len);
                ack_pkt.seq_no = rcv_pkt.seq_no;
                if (rcv_pkt.seq_no == state)
                {
                    if (sendto(s, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_other,
                               slen) == -1)
                    {
                        die("sendto()");
                    }
                    // printf("ACK %d SENT\n", ack_pkt.seq_no);
                    state = (state + 1) % 2;
                    
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
                break;
            }
        }
        
        }
    }
    close(s);
    return 0;
}