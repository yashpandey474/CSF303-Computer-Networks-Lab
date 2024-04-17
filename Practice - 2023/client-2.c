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

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT2);
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");

    // OPEN FILE
    FILE *fp;
    fp = fopen("id.txt", "r");

    if (fp == NULL)
    {
        printf("COULD NOT OPEN FILE");
        exit(1);
    }

    // INITIAL STATE
    int state = 0, retval = 0, ack_no = 0, next_state = 1, slen = sizeof(si_other);

    // TIMER
    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(s, &rfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    pkt.ACK_OR_DATA = 0; // Assuming this is for data packets
    pkt.seq_no = 0;
    pkt.len = 0;
    memset(pkt.data, 0, MAX_DATA_SIZE); // Initialize data buffer

    while (1)
    {

        printf("CURRENT STATE: %d\n", state);

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

            if (feof(fp)){
                pkt.final_packet = 1;
            }

            printf("DATA READ: %s of size %d Bytes\n", pkt.data, pkt.len);

            // SET SEQ. NO.
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

            tv.tv_sec = 5;
            tv.tv_usec = 0;

            ack_no = (state - 1) / 2;
            next_state = (state + 1) % 4;

            // printf("TIMEOUT VAL: %d\n", tv.tv_sec);
            printf("WAITING ON SELECT FOR ACK %d\n", ack_no);

            // CHECK SOCKET AND TIMEOUT
            retval = select(s + 1, &rfds, NULL, NULL, &tv);

            printf("RETVAL FROM SELECT FOR 0: %d\n", retval);

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