#include "generalQueue.h"

generalHead *new_generalQueue()
{
    generalHead *head = (generalHead *)malloc(sizeof(generalHead));
    head->first = NULL;
    head->last = NULL;
}

bool general_enqueue(generalHead *head, void *newNodeData)
{
    generalNode *p = (generalNode *)malloc(sizeof(generalNode));
    if (p == NULL)
        return false;

    p->data = newNodeData;
    p->next = NULL;

    if (general_isEmpty(head))
        head->first = p;
    else
        head->last->next = p;
    head->last = p;
    return true;
}

void *general_dequeue(generalHead *head)
{
    if (general_isEmpty(head))
        return NULL;
    generalNode *temp = head->first;
    head->first = head->first->next;
    return temp->data;
}

bool general_isEmpty(generalHead *head)
{
    return (head->first == NULL);
}

void general_clear(generalHead *head)
{
    free(head);
}