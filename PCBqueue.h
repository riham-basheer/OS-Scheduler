#ifndef PCBQUEUE
#define PCBQUEUE

#include "PCB.h"
#include <stdio.h>
#include<stdbool.h>

/*typedef short bool;
#define true 1
#define false 0
*/
// ------------------ //
typedef struct PCBNode PCBNode;
struct PCBNode
{
    PCB data;
    
    PCBNode *next;
};

typedef struct PCBqueue PCBqueue;
struct PCBqueue
{
    PCBNode *head;
};

// ------------------ //

PCBqueue *new_PCBqueue()
{
    PCBqueue *queue = (PCBqueue *)malloc(sizeof(PCBqueue));
    queue->head = NULL;
    int x;
};

bool PCB_isEmpty(PCBNode *head)
{
    return (head == NULL);
};

bool PCB_enqueue(PCBqueue *queue, PCB *newNode)
{
    PCBNode *pNew = (PCBNode *)malloc(sizeof(PCBNode));
    if (pNew == NULL)
        return false;
   
    pNew->data = *newNode;
    pNew->next = NULL;

    if (PCB_isEmpty(queue->head))
    {
        pNew->next = NULL;
        queue->head = pNew;
        return true;
    }
   
        pNew->next = queue->head;
        queue->head = pNew;
   
    return true;
};

bool PCB_dequeue(PCBqueue *queue, PCB *pcb)
{
    PCBNode *pTemp = (PCBNode *)malloc(sizeof(PCBNode));

    if (PCB_isEmpty(queue->head))
    {
        return false;
    }

    else if(queue->head->next == NULL){
        *pcb = queue->head->data;
        queue->head= NULL;
        return true;
    }
    else
    {

        pTemp = queue->head;
        while (pTemp->next->next != NULL)
        {
            pTemp = pTemp->next;
        }
        *pcb = pTemp->next->data;
        pTemp->next = NULL;
        return true;
    }

    
};

void PCB_clear(PCBNode *head)
{
    free(head);
};

#endif
