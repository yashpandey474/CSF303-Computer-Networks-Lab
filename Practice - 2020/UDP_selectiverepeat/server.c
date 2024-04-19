#include "packet.h"

//UDP SERVER [SELECTIVE REPEAT]
int main(){
    //CONNECT TO BOTH R1 & R2
    
    struct sockaddr_in si_me_r1, si_me_r2, si_r1, si_r2;

    //ZERO OUT OWN ADDRESS
    memset((char *)&si_me_r1, 0, sizeof(si_me_r1));
    si_me_r1.sin_family = AF_INET;
    si_me_r1.sin_port = htons(SERVER_PORT_R1);
    si_me_r1.sin_addr.s_addr = htonl(INADDR_ANY);

    memset((char *)&si_me_r2, 0, sizeof(si_me_r2));
    si_me_r2.sin_family = AF_INET;
    si_me_r2.sin_port = htons(SERVER_PORT_R2);
    si_me_r2.sin_addr.s_addr = htonl(INADDR_ANY);

    int slen1 = sizeof(si_me_r1), slen2 = sizeof(si_me_r2);

    //CREATE SOCKETS
    int *s = (int *)malloc(sizeof(int) * 2);
    if ((s[0] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    if ((s[1] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    printf("CREATED SOCKETS\n");


    //TRY TO FIX ADDRESS ALREADY IN USE
    int opt = 1;
    setsockopt(s[0], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(s[1], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    // BIND SOCKETS TO DIFFERENT PORTS [UDP; 2 TUPLE FOR IDENTIFICATION]
    if (bind(s[0], (struct sockaddr *)&si_me_r1, sizeof(si_me_r1)) == -1)
    {
        die("bind()");
    }

    if (bind(s[1], (struct sockaddr *)&si_me_r2, sizeof(si_me_r2)) == -1)
    {
        die("bind()");
    }

    printf("SOCKETS BINDED\n");

    //SELECTIVE REPEAT
    int rcv_base = 0;
    int ackd[WINDOW_SIZE];

    memset(ackd, 0, sizeof(ackd));

    fd_set rfds;
    int maxfd, activity, recv_len;
    PACKET rcv_pkt, ack_pkt;

    //OPEN FILE FOR WRITING
    FILE *fp = fopen("output.txt", "wb");

    if (fp == NULL){
        die("fopen()");
    }

    while (1)
    {
        //CHECK BOTH SOCKETS FOR WHICH ONE IS SET
        FD_ZERO(&rfds);
        FD_SET(s[0], &rfds);
        FD_SET(s[1], &rfds);

        maxfd = (s[0] > s[1]) ? s[0] : s[1];

        //SELECT
        // printf("WAITING FOR A PACKET");
        fflush(stdout);
        activity = select(maxfd + 1, &rfds, NULL, NULL, NULL);

        if (activity == -1){
            die("select()");
        }

        //IF R1 IS READY
        if (FD_ISSET(s[0], &rfds)){
            memset(rcv_pkt.payload, 0, sizeof(rcv_pkt.payload));
            // RECEIVE FROM R1
            if ((recv_len = recvfrom(s[0], &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_r1, &slen1)) == -1)
            {
                die("recvfrom()");
            }

            // printf("RECEIVED FROM R1");
            printf("RECEIVED PACKET WITH SEQ NO %d\n", rcv_pkt.seq_no);
            printf("WINDOW: %d to %d\n", rcv_base, rcv_base + WINDOW_SIZE);


            

            // CHECK IS SEQ NUMBER IS IN CURRENT OR PREVIOUS WINDOW
            if ((rcv_pkt.seq_no >= rcv_base && rcv_pkt.seq_no < rcv_base + WINDOW_SIZE) || (rcv_pkt.seq_no <= rcv_base - 1 && rcv_pkt.seq_no >= rcv_base - WINDOW_SIZE))
            {
                //SEND ACK TO R1
                ack_pkt.seq_no = rcv_pkt.seq_no;

                if (sendto(s[0], &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_r1, sizeof(si_r1)) == -1){
                    die("sendto()");
                }

                //SET ACK
                if (rcv_pkt.seq_no >= rcv_base && rcv_pkt.seq_no < rcv_base + WINDOW_SIZE && ackd[rcv_pkt.seq_no % WINDOW_SIZE] == 0){
                    ackd[rcv_pkt.seq_no % WINDOW_SIZE] = 1;

                    // WRITE TO FILE
                    // fputs(rcv_pkt.payload, fp);
                    fwrite(rcv_pkt.payload, 1, recv_len, fp);
                    fflush(fp);
                }

                printf("SENT ACK FOR SEQ NO %d\n", ack_pkt.seq_no);
            
            }
            //ELSE IGNORE PACKET
        }

        //IF R2 IS READY
        if (FD_ISSET(s[1], &rfds))
        {
            memset(rcv_pkt.payload, 0, sizeof(rcv_pkt.payload));
            // RECEIVE FROM R2
            if ((recv_len = recvfrom(s[1], &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_r2, &slen2)) == -1)
            {
                die("recvfrom()");
            }

            // printf("RECEIVED FROM R2");
            printf("RECEIVED PACKET WITH SEQ NO: %d\n", rcv_pkt.seq_no);
            printf("WINDOW: %d to %d\n", rcv_base, rcv_base + WINDOW_SIZE);

            // CHECK IS SEQ NUMBER IS IN CURRENT OR PREVIOUS WINDOW
            if ((rcv_pkt.seq_no >= rcv_base && rcv_pkt.seq_no < rcv_base + WINDOW_SIZE) || (rcv_pkt.seq_no <= rcv_base - 1 && rcv_pkt.seq_no >= rcv_base - WINDOW_SIZE))
            {
                // SEND ACK TO R2
                ack_pkt.seq_no = rcv_pkt.seq_no;
                if (sendto(s[1], &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&si_r2, sizeof(si_r2)) == -1){
                    die("sendto()");
                }

                //SET ACK
                if (rcv_pkt.seq_no >= rcv_base && rcv_pkt.seq_no < rcv_base + WINDOW_SIZE && ackd[rcv_pkt.seq_no % WINDOW_SIZE] == 0)
                {
                    ackd[rcv_pkt.seq_no % WINDOW_SIZE] = 1;

                    // WRITE TO FILE
                    // fputs(rcv_pkt.payload, fp);
                    fwrite(rcv_pkt.payload, 1, recv_len, fp);
                    fflush(fp);
                }

                // printf("SENT ACK TO R2");
                printf("SENT ACK FOR SEQ NO %d\n", ack_pkt.seq_no);
            }
            // ELSE IGNORE PACKET
            
        }

        //INCREMENT BASE
        while (ackd[rcv_base % WINDOW_SIZE] == 1){

            // ENSURE NO INFINITE LOOP [SET TO 0]
            ackd[rcv_base % WINDOW_SIZE] = 0;

            rcv_base = incrementSeqNo(rcv_base);
        }

        // printf("ADJUSTED BASE TO: %d\n", rcv_base);
    }
}