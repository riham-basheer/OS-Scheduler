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
} PCB;

#endif
