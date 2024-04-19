/*
1.  The sender transmits packets through two different channels (TCP connections).
2.    The  server  acknowledges  the  receipt  of  a  packet  via  the  same  channel  through  which  the
corresponding packet has been received.
3.   The sender transmits a new packet using the same channel on which it has received an ACK for its
one of the previously transmitted packet.  Note that, at a time, there can be at most two outstanding
unacknowledged packets at the sender side.
4. On the server-side, the packets transmitted through different channels may arrive out of order.  In
that case, the server has to buffer the packets temporarily to finally construct in-order data stream.
*/


#include "packet.h"
// GLOBAL VARIABLES
int currseq = 0, lastseq1, lastseq2, lastack1, lastack2;
PACKET lastunack1, lastunack2;
char inputBuffer[BUFFER_SIZE + 1];
int buffercount = -1;
FILE *fp;


//CREATE A PACKET
PACKET* createPacket(int channel_id, int seq_no){
    PACKET *pkt = (PACKET *)malloc(sizeof(PACKET));
    

    
    pkt->ACK_OR_DATA = 1;
    pkt->channel_id = channel_id;
    pkt->seq_no = seq_no + (BUFFER_SIZE)*buffercount;
    pkt->size = PACKET_SIZE;

    int i = currseq;

    for (i = currseq; i < currseq + PACKET_SIZE; i ++){
        pkt->data[i - currseq] = inputBuffer[i];
    }

    return pkt;
}


FILE* getfilestream(FILE* fp, char* inputBuffer, int buffersize){
    int lenofinputread;

    if (fp == NULL){
        exit(1);
    }

    if (feof(fp)){
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
void inputintobuffer(){
    for (int i = 0; i < BUFFER_SIZE; i ++){
        inputBuffer[i] = '\0';
    }

    fp = getfilestream(fp, inputBuffer, BUFFER_SIZE);
    buffercount += 1;
}

int compute(){
    if (currseq == BUFFER_SIZE){
        currseq = 0;
    }

    if (inputBuffer[currseq] == '\0'){
        return 1;
    }
    return 0;
}
int main(){
    

    // CREATE SOCKETS

    //STORE IN ARRAY FOR EASE WHEN CHECKING TIMEOUT
    int *s = (int *)malloc(sizeof(int) * 2);

    s[0] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s[0] < 0)
    {
        printf("ERROR IN OPENING A SOCKET\n");
        exit(1);
    }

    s[1] = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s[1] < 0)
    {
        printf("ERROR IN OPENING A SOCKET\n");
        exit(1);
    }

    //DEFINE COMMON SERVER ADDRESS
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("ADDRESS ASSIGNED\n");

    // ESTABLISH CONNECTION
    int c = connect(s[0],
                    (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (c < 0)
    {
        printf("ERROR WHILE ESTABLISHING CONNECTION FROM 1ST SOCKET\n");
        exit(1);
    }
    printf("CONNECTION ESTABLISHED FROM 1ST SOCKET\n");

    int c = connect(s[1],
                    (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (c < 0)
    {
        printf("ERROR WHILE ESTABLISHING CONNECTION FROM 2nd SOCKET\n");
        exit(1);
    }
    printf("CONNECTION ESTABLISHED FROM 2nd SOCKET\n");

    int check = 0,state = 0;

    //OPEN THE FILE
    fp = fopen("input.txt", "rb");

    //TAKE INITIAL INPUT INTO BUFFER
    inputintobuffer();

    //LOOP UNTIL ALL TRANSMITTED AND ACKED
    while (1){

        switch(state){
            case(0):{
                //CREATE A PACKET FOR CHANNEL 1
                pkt = createPacket(0, currseq);

                lastseq1 = currseq;
                currseq += PACKET_SIZE;
            }
        }
    }

}