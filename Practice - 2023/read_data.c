#include <stdio.h>  //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>


int main(){
    FILE *fp;
    fp = fopen("name.txt", "r");

    if (fp == NULL)
    {
        printf("COULD NOT OPEN FILE");
        exit(1);
    }

    char line[100]; // Adjust the buffer size as needed

    // KEEP READING LINE BY LINE
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        // Remove newline character if present
        line[strcspn(line, "\n")] = 0;

        // Split line by commas
        char *token = strtok(line, ",");
        while (token != NULL)
        {

            // If token has a ., remove it
            if (token[strlen(token) - 1] == '.')
            {
                token[strlen(token) - 1] = 0;
            }

            // Process value
            printf("%s %d\n", token, strlen(token));

            // Get next token
            token = strtok(NULL, ",");

            
        }

        // Check if line ends with a period
        if (line[strlen(line) - 1] == '.')
        {
            break;
        }
    }

    fclose(fp);
    return 0;
}
