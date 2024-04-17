#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    FILE *fp;
    fp = fopen("list.txt", "wb");

    fputs(",", fp);
    fputs("helloyash", fp);

    fclose(fp);
    return 0;
}
