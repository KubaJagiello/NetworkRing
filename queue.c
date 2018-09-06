
#include "queue.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

queue *queue_create(void) {
    pthread_mutex_lock(&mutex);
    queue* queue = calloc(1, sizeof(queue));
    queue->head = NULL;
    queue->tail = calloc(1, sizeof(node));
    pthread_mutex_unlock(&mutex);
    return queue;
}

data queue_dequeue(queue* queue) {
    if(queue->head != NULL){
        node* node_to_return = queue->head->next;

        

        return node_to_return->value;
    }


    while(queue_is_empty(queue)){
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_lock(&mutex);

    pthread_mutex_unlock(&mutex);
    return value_to_return;
}

int queue_enqueue(queue* queue, data value) {
    pthread_mutex_lock(&mutex);

    node* new_node = calloc(1, sizeof(node));
    new_node->value = value;

    if(queue->head == NULL){
        queue->head = new_node;
        queue->tail->next = new_node;
        pthread_mutex_unlock(&mutex);
        return 0;
    }

    new_node->next = queue->tail->next;
    queue->tail->next = new_node;

    pthread_mutex_unlock(&mutex);
    return 0;
}

bool queue_is_empty(queue* queue){
    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);
    return 1;
}

int queue_free(queue* queue) {

    while(!queue_is_empty(queue)){
        pthread_mutex_lock(&mutex);

        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_lock(&mutex);

    pthread_mutex_unlock(&mutex);
    return 0;
}

void queue_set_memory_handler(queue* queue, memFreeFunc* freeFunc){
    pthread_mutex_lock(&mutex);

    pthread_mutex_unlock(&mutex);
}