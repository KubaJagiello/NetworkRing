
#ifndef NODE_H
#define NODE_H

#include<stdlib.h>
#include "queue.h"

typedef struct node_info{
    char *address;
    int port;
    bool is_participant;
} node_info;

typedef struct socket_and_queue{
    struct queue* queue;
    int socket_fd;
    struct node_info* self_info;
    struct node_info* target_info;
} socket_and_queue;

node_info *create_node_info(char *address, int port);

void free_all(node_info *self, node_info *target);

node_info * init_target_node_info(char *address, char *port);

node_info * init_self_node_info(char* port);

void usage_error(const char *const *argv);

void parse_message(char *message, queue* q, node_info* info);

void *socket_ring_reader(void *sq) ;

void *socket_ring_writer(void *sq);

bool first_arg_is_bigger(char *self_id, char *other_id);


#endif //NODE_H