#include "processQueue.h"

processQueue *new_processQueue()
{
    Head *h = (Head *)malloc(sizeof(Head));
    h->first = NULL;
    h->last = NULL;
    processQueue *queue = (processQueue *)malloc(sizeof(processQueue));
    queue->head = h;
}

bool enqueue(processQueue *queue, processData *newNode)
{
    Node *p = (Node *)malloc(sizeof(Node));
    if (p == NULL)
        return false;

    p->data = newNode;
    p->next = NULL;

    if (isEmpty(queue))
        queue->head->first = p;
    else
        queue->head->last->next = p;
    queue->head->last = p;
    return true;
}

processData *dequeue(processQueue *queue)
{
    if (isEmpty(queue))
        return NULL;
    Node *temp = queue->head->first;
    queue->head->first = queue->head->first->next;
    return temp->data;
}

bool isEmpty(processQueue *queue)
{
    return (queue->head->first == NULL);
}

void clear(processQueue *queue)
{
    free(queue->head);
    free(queue);
}
