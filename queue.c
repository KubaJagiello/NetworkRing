
#include "queue.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

queue *queue_create(void) {
    pthread_mutex_lock(&mutex);
    queue* queue = calloc(1, sizeof(queue));
    queue->head = calloc(1, sizeof(node));
    queue->head->next = queue->head;
    queue->head->prev = queue->head;
    queue->head->value = NULL;
    queue->freeFunc = NULL;
    pthread_mutex_unlock(&mutex);
    return queue;
}

queue_position queue_dequeue(queue* queue) {
    if(queue_is_empty(queue)){
        return NULL;
    }
    pthread_mutex_lock(&mutex);
    queue_position node_to_return = queue->head->prev;
    queue->head->prev = node_to_return->prev;
    node_to_return->prev->next = queue->head;
    node_to_return->next = NULL;
    node_to_return->prev = NULL;
    pthread_mutex_unlock(&mutex);
    return node_to_return;
}

int queue_enqueue(queue* queue, data value) {
    pthread_mutex_lock(&mutex);
    queue_position new_node = calloc(1, sizeof(node));
    new_node->value = value;
    queue_position last_node = queue->head->next;
    new_node->next = last_node;
    new_node->prev = queue->head;
    last_node->prev = new_node;
    queue->head->next = new_node;
    pthread_mutex_unlock(&mutex);
    return 0;
}

bool queue_is_empty(queue* queue){
    pthread_mutex_lock(&mutex);
    bool is_empty = queue->head->next == queue->head;
    pthread_mutex_unlock(&mutex);
    return is_empty;
}

int queue_free(queue* queue) {
    pthread_mutex_lock(&mutex);
    while(!queue_is_empty(queue)){
        queue_position node_to_free = queue->head->next;
        if(queue->freeFunc != NULL){
            queue->freeFunc(node_to_free->value);
        }
        queue->head->next = node_to_free->next;
        free(node_to_free);
    }
    free(queue->head);
    free(queue);
    pthread_mutex_unlock(&mutex);
    return 0;
}

void queue_setMemHandler(queue *queue, memFreeFunc *freeFunc){
    pthread_mutex_lock(&mutex);
    queue->freeFunc = freeFunc;
    pthread_mutex_unlock(&mutex);
}