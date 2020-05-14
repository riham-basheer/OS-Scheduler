#include <time.h>
#include "headers.h"
#include <signal.h>

/* Modify this file as needed*/
void stop_message(int signum);
void cont_message(int signum);

clock_t remainingTime;

int main(int agrc, char *argv[])
{
    signal(SIGSTOP, stop_message);
    signal(SIGCONT, cont_message);
    initClk();

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    // So, you'll be sending the remaining time when you initiate the process process
    // through main.

    clock_t startTime, currentTime;
    currentTime = 0;
    remainingTime = (clock_t)atoi(argv[1]);
    while (remainingTime > currentTime)
    {
        startTime = clock();
        currentTime = (double)(startTime / CLOCKS_PER_SEC);
    }

    destroyClk(false);
    // zero means termintated successfully.
    return 0;
}

void stop_message(int sig)
{
    printf("\nI am stopped. My remaining time is %d.\n", (int)remainingTime);
}
void cont_message(int sig)
{
    printf("\nI am continuing. My remaining time is %d.\n", (int)remainingTime);
}
