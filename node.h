
#ifndef NODE_H
#define NODE_H

#include<stdlib.h>
#include "queue.h"

typedef struct node_info{
    char *address;
    int port;
} node_info;

typedef struct socket_and_queue{
    queue* queue;
    int socket_fd;
} socket_and_queue;

node_info *create_node_info(char *address, int port);

#endif //NODE_H