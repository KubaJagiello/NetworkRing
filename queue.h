
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
#include <pthread.h>
typedef struct node{
    data value;
    struct node* next;
    struct node* prev;
} node;

typedef node* queue_position;

typedef struct queue{
    queue_position head;
    memFreeFunc* freeFunc;
} queue;
#endif

queue* queue_create(void);

queue_position queue_dequeue(queue*);

int queue_enqueue(queue*, data);

bool queue_is_empty(queue* queue);

int queue_free(queue*);

void queue_setMemHandler(queue *queue, memFreeFunc *freeFunc);