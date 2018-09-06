


#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <ctype.h>
#include "queue.h"

pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;


#define NUM_THREADS 100

void thread_test();

void single_thread_test();

typedef struct test_data{
    int id_of_thread;
    queue *q;
} test_data;

void *init_threads(void *q){
    test_data *d = (test_data*)q;

    printf("%d\n", d->id_of_thread);

    if(d->id_of_thread < NUM_THREADS / 2){
        queue_enqueue(d->q, d);
        printf("enqueue with id %d\n", d->id_of_thread);
    } else {
        queue_position  p = queue_dequeue(d->q);
        if(p != NULL){
            test_data* new_data = (test_data*) p->value;
            printf("dequeue %d\n", new_data->id_of_thread);
        }
    }
}

int main(){
    thread_test();
    single_thread_test();
}

void single_thread_test() {
    queue* queue = queue_create();
    for (int i = 0; i < 5; i++) {
        int* val = calloc(1, sizeof(val));
        *val = i;
        queue_enqueue(queue, val);
    }

    while(!queue_is_empty(queue)){
        queue_position position = queue_dequeue(queue);
        printf("value = %d\n", *((int*)position->value));
    }

}

void thread_test() {
    pthread_t tid[NUM_THREADS];
    queue *test_queue = queue_create();


    for(int i=0; i < NUM_THREADS; i++){
        test_data *d = calloc(1, sizeof(test_data));
        d->q = test_queue;
        d->id_of_thread = i;
        int err = pthread_create(&(tid[i]), NULL, &init_threads, d);
        if(err != 0){
            printf("\n can't create thread: [%s]", strerror(err));
        }
    }
//    test_data *d = calloc(1, sizeof(test_data));
//    d->q = test_queue;
//    d->id_of_thread = 1337;
//    init_threads(d);
//

    for(int i=0; i < NUM_THREADS; i++){
        pthread_join(tid[i], NULL);
    }

    while(!queue_is_empty(test_queue)){
        queue_position position = queue_dequeue(test_queue);
        printf("%d", ((test_data*)position->value)->id_of_thread);
    }
}


