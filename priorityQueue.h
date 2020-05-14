#ifndef OS_SCHEDULER_PRIORITYQUEUE_H
# define OS_SCHEDULER_PRIORITYQUEUE_H

#include "PCB.h"

typedef short bool;
#define true 1
#define false 0

// -------- For HPF ----------- //
typedef struct priorityNode priorityNode;
struct priorityNode
{
    PCB data;
    int priority;
    priorityNode *next;
};

typedef struct priorityQueue priorityQueue;
struct priorityQueue
{
    priorityNode *head;
};

// ---------------------------- //

priorityQueue *new_PrioriyQueue()
{
    priorityQueue *queue = (priorityQueue *)malloc(sizeof(priorityQueue));
    queue->head = NULL;
};

bool priority_isEmpty(priorityNode *head)
{
    return (head == NULL);
};

bool priority_enqueue(priorityQueue *queue, PCB *newNode)
{
    priorityNode *pNew = (priorityNode *)malloc(sizeof(priorityNode));
    if (pNew == NULL)
        return false;
    priorityNode *pTemp = (priorityNode *)malloc(sizeof(priorityNode));
    pNew->data = *newNode;
    pNew->priority = (*newNode).processStruct.priority;
    pNew->next = NULL;
    if (priority_isEmpty(queue->head))
    {
        pNew->next = NULL;
        queue->head = pNew;
        return true;
    }
    if (pNew->priority < queue->head->priority)
    {
        pNew->next = queue->head;
        queue->head = pNew;
    }
    else
    {
        pTemp = queue->head;
        while (pTemp->next != NULL)
        {
            if (pTemp->next->priority > pNew->priority)
            {
                pNew->next = pTemp->next;
                pTemp->next = pNew;
                return true;
            }
            pTemp = pTemp->next;
        }
        if (pTemp->priority <= pNew->priority)
        {
            pNew->next = NULL;
            pTemp->next = pNew;
        }
    }
    return true;
};

bool priority_dequeue(priorityQueue *queue, PCB *pcb)
{
    if (priority_isEmpty(queue->head))
    {
        return false;
    }
    else
    {
        *pcb = queue->head->data;
        queue->head = queue->head->next;
        return true;
    }
};

void priority_clear(priorityNode *head)
{
    free(head);
};

#endif