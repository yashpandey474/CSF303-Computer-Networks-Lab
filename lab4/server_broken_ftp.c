#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr, client_addr;
    char sendBuff[1025];
    int numrv;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket retrieve success\n");
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5001);
    bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    int slen = sizeof(client_addr);

    while (1)
    {
        unsigned char offset_buffer[10] = {'\0'};
        unsigned char command_buffer[2] = {'\0'};
        int offset;

        int command, temp;
        printf("Waiting for client to send the command (Full File (0) Partial File (1)\n");
        recvfrom(listenfd, command_buffer, sizeof(command_buffer), 0, (struct sockaddr *)&client_addr, &slen);

        sscanf(command_buffer, "%d", &command);

        printf("GOT COMMAND FROM CLIENT %d\n", command);

        if (command == 0)
            offset = 0;

        else
        {
            printf("Waiting for client to send the offset\n");
            // while (read(connfd, offset_buffer, 10) == 0)
            //     ;
            recvfrom(listenfd, offset_buffer, 2, 0, (struct sockaddr *)&client_addr, &slen);

            sscanf(offset_buffer, "%d", &offset);

            printf("GOT OFFSET FROM CLIENT\n");
        }
        /* Open the file that we wish to transfer */
        FILE *fp = fopen("source_file.txt", "rb");
        if (fp == NULL)
        {
            printf("File opern error");
            return 1;
        }
        /* Read data from file and send it */
        fseek(fp, offset, SEEK_SET);
        while (1)
        {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[256] = {0};
            int nread = fread(buff, 1, 256, fp);
            printf("Bytes read %d \n", nread);
            /* If read was success, send data. */
            if (nread > 0)
            {
                printf("Sending \n");
                // write(connfd, buff, nread);
                if (sendto(listenfd, &buff, nread, 0, (struct sockaddr *)&client_addr, slen) == -1)
                {
                    die("sendto()");
                }

                printf("SENT BYTES TO CLIENT!");
            }
            /*
             * There is something tricky going on with read ..
             * Either there was error, or we reached end of file.
             */

            if (nread < 256)
            {
                if (feof(fp))
                    printf("End of file\n");
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }
        }
        sleep(1);
    }
    return 0;
}
