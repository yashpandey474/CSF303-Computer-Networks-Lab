#include "packet.h"

void die(char *s)
{
    perror(s);
    exit(1);
}

int currbyte = 0;
int currseq = 0;
int lastseq = 0;
char buf[BUFFER_SIZE];
FILE *fp;

PACKET *createPacket()
{
    PACKET *pkt = (PACKET *)malloc(sizeof(PACKET));

    pkt->ack_or_data = 1;
    pkt->seq_no = currseq;
    pkt->size = PACKET_SIZE;

    int i = currbyte;

    for (i = currbyte; i < currbyte + PACKET_SIZE; i++)
    {
        pkt->payload[i - currbyte] = buf[i];
    }

    return pkt;
}

FILE *getfilestream(FILE *fp, char *inputBuffer, int buffersize)
{
    int lenofinputread;

    if (fp == NULL)
    {
        exit(1);
    }

    if (feof(fp))
    {
        fclose(fp);
        return NULL;
    }
    if ((lenofinputread = (fread(inputBuffer, sizeof(char), buffersize, fp))) > 0)
    {
        inputBuffer[lenofinputread] = '\0';
        return fp;
    }
    else
    {
        fclose(fp);
        return NULL;
    }
}
void inputintobuffer()
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buf[i] = '\0';
    }

    fp = getfilestream(fp, buf, BUFFER_SIZE);
    
}

int compute()
{
    if (currbyte == BUFFER_SIZE)
    {
        currbyte = 0;
    }

    if (buf[currbyte] == '\0')
    {
        return 1;
    }
    return 0;
}

int main(){
    //CREATE TWO UDP SOCKETS: ONE FOR R1 AND ONE FOR R2
    int *s = (int *)malloc(sizeof(int)*2);
    if ((s[0] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
    if ((s[1] = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    printf("SOCKETS CREATED AT CLIENT\n");

    //DEFINE ADDRESSES OF R1 & R2
    struct sockaddr_in si_r1, si_r2;
    int slen1 = sizeof(si_r1), slen2 = sizeof(si_r2);
    
    
    PACKET lastunackpkt, rcv_ack;
  
    
    memset((char *)&si_r1, 0, sizeof(si_r1));
    si_r1.sin_family = AF_INET;
    si_r1.sin_port = htons(R1_PORT);
    si_r1.sin_addr.s_addr = inet_addr("127.0.0.1");

    memset((char *)&si_r2, 0, sizeof(si_r2));
    si_r2.sin_family = AF_INET;
    si_r2.sin_port = htons(R2_PORT);
    si_r2.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("ADDRESSES FOR R1 AND R2 ARE SET AT CLIENT\n");

    int state = 0;  
    struct timeval timer;
    int retval;
    fd_set rfds;

    timer.tv_sec = 5;
    timer.tv_usec = 0;


    //OPEN THE FILE TO TRANSFER
    fp = fopen("input.txt", "rb");
    //TAKE INPUT TO BUFFER
    inputintobuffer();

    int base = 0;

    PACKET sent_packets[WINDOW_SIZE];
    int acks[WINDOW_SIZE];

    while (!feof(fp) || base < currseq)
    {
        // Send new packets if window is not full
        while (currseq < base + WINDOW_SIZE && !feof(fp))
        {
            PACKET packet;
            packet.seq_no = currseq;
            packet.ack_or_data = 1;
            packet.size = fread(packet.payload, 1, PACKET_SIZE, fp);



            sent_packets[currseq] = packet;
            currseq = (currseq + 1) % WINDOW_SIZE;
            currbyte += PACKET_SIZE;

            // CHECK IF FINAL PACKET
            if (feof(fp))
            {
                packet->final_packet = 1;
            }
            packet.final_packet = 0;

            // TRANSMIT PACKET TO R1 IF ODD SEQ NO
            if (currseq % 2)
            {
                if (sendto(s[0], &packet, sizeof(packet), 0, (struct sockaddr *)&si_r1, slen1) == -1)
                {
                    die("sendto()");
                }
            }
            else
            {
                // TRANSMIT TO R2 FOR EVEN SEQ NO
                if (sendto(s[1], &packet, sizeof(packet), 0, (struct sockaddr *)&si_r1, slen1) == -1)
                {
                    die("sendto()");
                }
            }
            // STORE PACKET FOR TIMEOUT
            sent_packets[currseq] = packet;
        


        }

        // Receive acknowledgments
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);


        struct timeval timeout;
        timeout.tv_sec = RETRANSMISSION_TIME;
        timeout.tv_usec = 0;


        int ready = select(sockfd + 1, &read_fds, NULL, NULL, &timeout);


        if (ready == 0)
        {
                // Timeout occurred
                // Resend unacknowledged packets
                for (int i = base; i < currseq; i++)
                {
                    if (!acks[i % WINDOW_SIZE])
                    {
                        sendto(sockfd, &acks[i % WINDOW_SIZE], sizeof(PACKET), 0, (struct sockaddr *)&server_addr, server_len);
                    }
                }
            
            
        }
        else if (ready > 0)
        {
            PACKET ack_packet;
            recvfrom(sockfd, &ack_packet, sizeof(ack_packet), 0, (struct sockaddr *)&server_addr, &server_len);
            // Mark packet as acknowledged
            acks[ack_packet.seq_no % WINDOW_SIZE] = 1;
            // Slide window if necessary
            while (acks[base % WINDOW_SIZE])
            {
                base++;
            }
        }
        else
        {
            die("select error");
            exit(EXIT_FAILURE);
        }
    }
}
                