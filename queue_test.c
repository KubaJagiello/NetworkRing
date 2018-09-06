


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

void free_test();

typedef struct test_data{
    int id_of_thread;
    queue *q;
} test_data;


void my_int_mem_handler(data d){
    free(d);
}

void my_test_data_mem_handler(data d){
    free((test_data*) d);
}


void *init_threads(void *q){
    test_data *d = (test_data*)q;

    if(d->id_of_thread < NUM_THREADS / 2){
        sleep(1);
        queue_enqueue(d->q, d);
        fprintf(stderr,"enqueue with id %d\n", d->id_of_thread);
    } else {
        test_data *new_data = (test_data*)queue_dequeue(d->q);
        if(new_data != NULL){
            fprintf(stderr,"dequeue %d\n", new_data->id_of_thread);
        }
        my_test_data_mem_handler(new_data);
    }
}

int main(){
    free_test();
    //thread_test();
    //single_thread_test();
}

void free_test() {
    queue* q = queue_create();
    queue_set_memory_handler(q, &my_int_mem_handler);
    for (int i = 0; i < 100; ++i) {
        int* test = calloc(1, sizeof(int*));
        *test = 1;
        queue_enqueue(q, test);
    }
    for (int j = 0; j < 50; ++j) {
        my_int_mem_handler(queue_dequeue(q));
    }
    queue_free(q);
}

void single_thread_test() {
    queue* queue = queue_create();
    queue_set_memory_handler(queue,&my_int_mem_handler);
    for (int i = 0; i < 5; i++) {
        int* val = calloc(1, sizeof(val));
        *val = i;
        queue_enqueue(queue, val);
    }

    while(!queue_is_empty(queue)){
        int* value = queue_dequeue(queue);
        printf("value = %d\n", *value);
        my_int_mem_handler(value);
    }

    queue_free(queue);

}

void thread_test() {
    pthread_t tid[NUM_THREADS];
    queue *queue = queue_create();
    queue_set_memory_handler(queue, &my_test_data_mem_handler);

    for(int i=0; i < NUM_THREADS; i++){
        test_data *d = calloc(1, sizeof(test_data));
        d->q = queue;
        d->id_of_thread = i;
        int err = pthread_create(&(tid[i]), NULL, &init_threads, d);
        if(err != 0){
            printf("\n can't create thread: [%s]", strerror(err));
        }
    }
    for(int i=0; i < NUM_THREADS; i++){
        pthread_join(tid[i], NULL);
    }

    while(!queue_is_empty(queue)){
        queue_position position = queue_dequeue(queue);
        printf("%d", ((test_data*)position->value)->id_of_thread);
    }
    queue_free(queue);
}


