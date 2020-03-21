#ifndef OS_SCHEDULER_GENERALQUEUE_H
#define OS_SCHEDULER_GENERALQUEUE_H

#include "def.h"

typedef struct generalNode generalNode;
struct generalNode
{
    void *data;
    generalNode *next;
};

typedef struct generalHead generalHead;
struct generalHead
{
    generalNode *first;
    generalNode *last;
};

generalHead *new_generalQueue();
bool general_enqueue(generalHead *head, void *newNode);
void *general_dequeue(generalHead *head);
bool general_isEmpty(generalHead *head);
void general_clear(generalHead *head);
#endif