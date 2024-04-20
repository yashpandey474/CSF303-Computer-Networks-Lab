#include <stdio.h>
#include <sys/socket.h> //SOCKET, CONNECT, SEND, RECV
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //CLOSE

#define RETRANSMISSION_TIME 2
// SAME PORT AT SERVER AND DIFFERENT SOCKETS AT CLIENT => DIFFERENT PORTS AT CLIENT
#define SERVER_PORT 5001
#define PACKET_SIZE 100
#define BUFFER_SIZE 1000
// STRUCTURE FOR PACKET

/*
a. The size (number of bytes) of the payload
b. The Seq. No. (in terms of byte) specifying the offset of the first byte of the packet with
respect to the input file.
c. Whether the packet is last packet or not?
d. The packet is DATA or ACK. In this way, you can utilize the same packet structure for
both DATA send by the  client and ACK  send by the server. The Seq. No. field in the
ACK packet would correspond to it DATA packet with the same Seq. No. value received
from the client.
e. The channel information specifying the Id (either 0 or 1) of the channel through which
the packet has been transmitted.


*/
typedef struct
{
    int ACK_OR_DATA;
    char data[PACKET_SIZE + 1];
    int seq_no; // FIRST BYTE OF PACKET WITH RESPECT TO INPUT FILE
    int size;
    int last_packet;
    int channel_id;
} PACKET;