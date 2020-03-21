#ifndef OS_SCHEDULER_PROCESSQUEUE_H
#define OS_SCHEDULER_PROCESSQUEUE_H

#include "generalQueue.c"
#include "processData.h"

struct processQueue
{
    generalHead *head;
};

typedef struct processQueue processQueue;

processQueue *new_processQueue();
bool enqueue(processQueue *queue, processData *newNode);
processData *dequeue(processQueue *queue);
bool isEmpty(processQueue *queue);
void clear(processQueue *queue);

#endif