#ifndef OS_SCHEDULER_PROCESSQUEUE_H
#define OS_SCHEDULER_PROCESSQUEUE_H

#include "processData.h"
#include "def.h"

typedef struct Node Node;
struct Node
{
    processData *data;
    Node *next;
};

typedef struct Head Head;
struct Head
{
    Node *first;
    Node *last;
};

typedef struct processQueue processQueue;
struct processQueue
{
    Head *head;
};

processQueue *new_processQueue();
bool enqueue(processQueue *queue, processData *newNode);
processData *dequeue(processQueue *queue);
bool isEmpty(processQueue *queue);
void clear(processQueue *queue);

#endif