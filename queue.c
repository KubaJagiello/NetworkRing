
#include "queue.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

queue *queue_create(void) {
    pthread_mutex_lock(&mutex);
    queue* q = calloc(1, sizeof(queue));
    q->front = NULL;
    q->back = NULL;
    pthread_mutex_unlock(&mutex);
    return q;
}

data queue_dequeue(queue* q) {
    pthread_mutex_lock(&mutex);

    while(q->front == NULL)
        pthread_cond_wait(&cond, &mutex);

    node* front_node = q->front;
    data data_to_return = front_node->value;
    q->front = front_node->next_in_line;
    free(front_node);
    pthread_mutex_unlock(&mutex);
    return data_to_return;
}

void queue_enqueue(queue* q, data value) {
    pthread_mutex_lock(&mutex);
    node* new_node = calloc(1, sizeof(node));
    new_node->value = value;
    new_node->next_in_line = NULL;

    if(q->front == NULL)
        q->front = q->back = new_node;
    else {
        q->back->next_in_line = new_node;
        q->back = new_node;
    }
    
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

bool queue_is_empty(queue* q){
    pthread_mutex_lock(&mutex);
    bool is_empty = q->front == NULL;
    pthread_mutex_unlock(&mutex);
    return is_empty;
}

void queue_free(queue* q) {
    pthread_mutex_lock(&mutex);
    while(q->front != NULL){
        node *next_node = q->front->next_in_line;
        if(q->freeFunc != NULL)
            q->freeFunc(q->front->value);
        free(q->front);
        q->front = next_node;
    }
    free(q);
    pthread_mutex_unlock(&mutex);
}

void queue_set_memory_handler(queue* q, memFreeFunc* freeFunc){
    pthread_mutex_lock(&mutex);
    q->freeFunc = freeFunc;
    pthread_mutex_unlock(&mutex);
}