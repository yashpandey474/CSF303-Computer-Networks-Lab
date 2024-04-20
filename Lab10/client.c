
<<<<<<< HEAD
#include "packet.h"
//FUNCTION FOR CHECKING SOCKET WITH TIMEOUT 
int is_ready(int sockfd)
=======
#define PORT 5002
#define MAX_DATA_SIZE 256
#define TIMEOUT_SEC 5

typedef struct
>>>>>>> f132a61 (Tryr)
{
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(sockfd, &rfds);

    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;

    return select(sockfd + 1, &rfds, NULL, NULL, &tv);
}

<<<<<<< HEAD
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
=======

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
    if (fgets(pkt->data, MAX_DATA_SIZE, fp) != NULL)
    {
        int length = strlen(pkt->data);
        if (pkt->data[length - 1] == '\n')
        {
            pkt->data[length - 1] = '\0';
            length--; // Adjust length if newline was removed
        }
        pkt->len = length;
        pkt->seq_no = 0;
>>>>>>> f132a61 (Tryr)
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
<<<<<<< HEAD
    // printf("TIMEOUT AT SENDER FOR SEQ NO. %d\n", ack_no);
=======
    printf("TIMEOUT AT SENDER FOR SEQ NO. %d\n", ack_no);
>>>>>>> f132a61 (Tryr)
    if (sendto(sockfd, pkt, sizeof(PACKET), 0, (const struct sockaddr *)dest_addr, slen) == -1)
    {
        die("sendto()");
    }
    printf("RESENT DATA: Seq No %d OF SIZE %d Bytes\n", pkt->seq_no, pkt->len);
}

void handle_ack(int sockfd, struct sockaddr_in *si_other, socklen_t slen, PACKET *rcv_ack, int *state, int ack_no, int next_state)
{
<<<<<<< HEAD
    // printf("READING FROM SOCKET\n");
=======
    printf("READING FROM SOCKET\n");
>>>>>>> f132a61 (Tryr)
    if (recvfrom(sockfd, rcv_ack, sizeof(PACKET), 0, (struct sockaddr *)si_other, &slen) == -1)
    {
        die("recvfrom()");
    }
<<<<<<< HEAD
    // printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack->seq_no, rcv_ack->data);
=======
    printf("Received packet: Seq. No = %d, Data = %s\n", rcv_ack->seq_no, rcv_ack->data);
>>>>>>> f132a61 (Tryr)

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
<<<<<<< HEAD
    // printf("Transmitted Packet with Sequence No. %d\n", pkt->seq_no);
=======
    printf("Transmitted Packet with Sequence No. %d\n", pkt->seq_no);
>>>>>>> f132a61 (Tryr)
}


int main()
{
    printf("HELLO");
    // DEFINE PACKET
    PACKET pkt, rcv_ack;

    struct sockaddr_in si_other;
    int s;

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");

    // OPEN FILE
    FILE *fp;
    fp = fopen("name.txt", "r");

    if (fp == NULL)
    {
        printf("COULD NOT OPEN FILE");
        exit(1);
    }


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

<<<<<<< HEAD
        // printf("CURRENT STATE: %d\n", state);
=======
        printf("CURRENT STATE: %d\n", state);
>>>>>>> f132a61 (Tryr)

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
<<<<<<< HEAD

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

            // CHANGE STATE
            state += 1;

            break;
        

        case 3:
        case 1:
            ack_no = (state - 1) / 2;
            next_state = (state + 1) % 4;
            printf("WAITING ON SELECT FOR ACK %d\n", ack_no);

            //CHECK SOCKET AND TIMEOUT
            retval = select(s + 1, &rfds, NULL, NULL, &tv);

            printf("RETVAL FROM SELECT FOR 0: %d\n", retval);


>>>>>>> f132a61 (Tryr)
            // ERROR ON SELECT
            if (retval == -1)
            {

                die("select()");
            }

            // NON-ZERO: READY TO BE READ
            else if (retval)
            {
                handle_ack(s, &si_other, slen, &rcv_ack, &state, ack_no, next_state);
            }

            // TIMEOUT OCCURED! [RETVAL = 0]
            else
            {
                handle_timeout(s, &si_other, slen, &pkt, &state, ack_no);
            }
            break;
        }
    }
    return 0;
}