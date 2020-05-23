#ifndef OS_SCHEDULER_PROCESSDATA_H
#define OS_SCHEDULER_PROCESSDATA_H

#include <stdlib.h>

typedef struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memory;
}processData;

#endif 