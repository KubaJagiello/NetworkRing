


#ifndef __DATA
#define __DATA
typedef void* data;
#endif


typedef struct node{
    data value;
    struct node* next;
    struct node* prev;
} node;

typedef node* queue_position;

typedef struct queue{
    queue_position front;
} queue;

queue* queue_create(void);

queue_position queue_dequeue(queue*);

int queue_enqueue(queue*, data);

int queue_free(queue*);

