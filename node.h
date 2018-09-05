
#ifndef NODE_H
#define NODE_H

#include<stdlib.h>

typedef struct node_info{
    char *address;
    int port;
} node_info;

node_info *create_node_info(char *address, int port);

#endif //NODE_H