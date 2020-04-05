#ifndef OS_SCHEDULER_PCB_H
#define OS_SCHEDULER_PCB_H
#include "processData.h"
#include <stdlib.h>


typedef struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id; 
} processData;

typedef struct PCB
{
    processData processStruct;
    int remainingTime;
    int startTime;
    enum STATUS {STOPPED, STARTED, RESUMED, FINISHED};
    int TA;
    int pid;
}PCB;

#endif

