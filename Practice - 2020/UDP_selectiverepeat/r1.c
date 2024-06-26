#include "packet.h"

int main(){
    srand(time(NULL));

    struct sockaddr_in si_me, si_client, si_server;

    

    //CREATE SOCKETS [TO CLIENT AND TO SERVER]
    int *s = (int *)malloc(sizeof(int) * 2);

    //SOCKET TO CONNECT TO CLIENT
    if ((s[0] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    //SOCKET TO CONNECT TO SERVER
    if ((s[1] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    

    //ZERO OUT OWN ADDRESS [OF CONNECTION TO CLIENT]
    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(R1_PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //DEFINE ADDRESS OF SERVER
    memset((char *)&si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(SERVER_PORT_R1);
    si_server.sin_addr.s_addr = inet_addr("127.0.0.1");

    int slencl = sizeof(si_client), slense = sizeof(si_server), recv_len;

    //BIND SOCKET TO PORT
    if (bind(s[0], (struct sockaddr* )&si_me, sizeof(si_me)) == -1){
        die("bind()");
    }


    PACKET rcv_pkt;

    fd_set readfds;
    int maxfd, activity;

    //START RECEIVING PACKETS FROM CLIENT OR SERVER
    // printf("HELLO");
    while (1){
        FD_ZERO(&readfds);
        FD_SET(s[0], &readfds);
        FD_SET(s[1], &readfds);


        // CHECK WHICH SOCKET IS READY TO READ FROM
        maxfd = (s[0] > s[1]) ? s[0] : s[1];

        //NO TIMER ON SELECT
        // printf("WAITING FOR A PACKET\n");
        activity = select(maxfd + 1, &readfds, NULL, NULL, NULL);

        // CHECK WHICH IS SET

        if (FD_ISSET(s[0], &readfds)){
            //RECEIVE FROM CLIENT
            if ((recv_len = recvfrom(s[0], &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_client, &slencl)) == -1)
            {
                die("recvfrom()");
            }

            // printf("RECIEVED FROM CLIENT\n");

            //SIMULATE PACKET DROP
            if (dropPacket() == 1){
                printf("PACKET WITH SEQ NO DROPPED%d\n", rcv_pkt.seq_no);
                continue;
            }

            //INTRODUCE DELAY BEFORE TRANSMITTING TO SERVER
            randomDelay();

            //SEND PACKET TO SERVER
            if (sendto(s[1], &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_server, sizeof(si_server)) == -1){
                die("sendto()");
            }

            printf("FORWARDED PACKET %d TO SERVER\n", rcv_pkt.seq_no);
        }

        if (FD_ISSET(s[1], &readfds)){
            // RECEIVE ACK FROM SERVER
            if ((recv_len = recvfrom(s[1], &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_server, &slense)) == -1)
            {
                die("recvfrom()");
            }

            // printf("RECIEVED ACK FROM SERVER\n");

            // SEND PACKET TO CLIENT
            if (sendto(s[0], &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&si_client, sizeof(si_client)) == -1)
            {
                die("sendto()");
            }
            printf("FORWARDED ACK %d TO CLIENT\n", rcv_pkt.seq_no);
        }
    }
}