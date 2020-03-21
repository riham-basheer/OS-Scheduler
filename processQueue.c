#include "processQueue.h"

processQueue *new_processQueue()
{
    processQueue *queue = (processQueue *)malloc(sizeof(processQueue));
    queue->head = new_generalQueue();
}

bool enqueue(processQueue *queue, processData* newNode)
{
    return general_enqueue(queue->head, (void *)newNode);
}

processData *dequeue(processQueue *queue)
{
    return (processData *)general_dequeue(queue->head);
}

bool isEmpty(processQueue *queue)
{
    return general_isEmpty(queue->head);
}

void clear(processQueue *queue)
{
    general_clear(queue->head);
    free(queue);
}
