
#include <lzma.h>
#include <stdlib.h>
#include "queue.h"

queue *queue_create(void) {
    queue* queue = calloc(1, sizeof(queue));
    queue->front = calloc(1, sizeof(node));
    queue->front->next = queue->front;
    queue->front->value = NULL;
    return queue;
}

queue_position queue_dequeue(queue* queue) {

    return NULL;
}

int queue_enqueue(queue* queue, data value) {
    queue_position new_node = calloc(1, sizeof(node));
    new_node->next = queue->end->next;
    queue->end->next = new_node;
    return 0;
}

int queue_free(queue* queue) {
    return 0;
}
