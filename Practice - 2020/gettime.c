#include <stdio.h>
#include <sys/time.h>

int main()
{
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;

    char buffer[80];
    strftime(buffer, 80, "%H:%M:%S", localtime(&curTime.tv_sec)); // Only time format

    char currentTime[84] = "";
    sprintf(currentTime, "%s.%08d", buffer, milli);
    printf("current time: %s \n", currentTime);
}