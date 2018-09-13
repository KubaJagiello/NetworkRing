
#ifndef __DATA
#define __DATA
typedef void* data;
#endif

#ifndef __FREEFUNC
#define __FREEFUNC
typedef void memFreeFunc(data);
#endif

#ifndef __QUEUE_H
#define __QUEUE_H
#include <stdbool.h>
#include <lzma.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
typedef struct node{
    data value;
    struct node* next_in_line;
} node;

typedef node* queue_position;

typedef struct queue{
    node *front;
    node *back;
    memFreeFunc* freeFunc;
} queue;
#endif

queue* queue_create(void);

data queue_dequeue(queue*);

void queue_enqueue(queue*, data);

bool queue_is_empty(queue* queue);

void queue_free(queue*);

void queue_release_threads(void);

void queue_set_memory_handler(queue *queue, memFreeFunc *freeFunc);

void queue_free_node(queue_position position);