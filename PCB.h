#ifndef OS_SCHEDULER_PCB_H
#define OS_SCHEDULER_PCB_H
#include "processData.h"
typedef enum
{
    STOPPED,
    STARTED,
    RESUMED,
    FINISHED
} STATUS;

typedef struct PCB
{
    processData processStruct;
    int remainingTime;
    int startTime;
    STATUS status;
    int TA;
    int pid;
    int nquanta_taken;
<<<<<<< HEAD
    int totalwaitTime;
    int wait_at_start;
    int finishTime;
=======
>>>>>>> ba7c01aca13ca5118f115c619242da6efed58c5b
} PCB;

#endif
