#include <stdio.h>
#include "node.h"
#include "network_helper.h"

#define REQUIRED_ARGUMENT_NUMBER 5

void free_all(node_info *self, node_info *target);
void free_node(node_info *node);

node_info * init_target_node_info(char *address, char *port);
node_info * init_self_node_info(char* port);

//{tcpnode,udpnode} local-port next-host next-port
int main(int argc, char const *argv[]) {

    // lyssna på en port
    // connecta till en port

    if(argc != REQUIRED_ARGUMENT_NUMBER){
        perror("Invalid arguments");
        exit(EXIT_FAILURE);
    }


    node_info *self = init_self_node_info((char*)argv[2]);
    node_info *target = init_target_node_info((char*)argv[3], (char*)argv[4]);
    printf("%s, %d", self->address, self->port);

    //start(argv[1], self, target);


    free_all(self, target);

    return 0;
}

void free_all(node_info *self, node_info *target) {
    free_node(self);
    free_node(target);
}

node_info * init_target_node_info(char *address, char *port) {
    int targetPort;

    if((targetPort = atoi(port)) == -1){
        perror("Invalid port argument");
        exit(EXIT_FAILURE);
    }
    return create_node_info(address, targetPort);
}

node_info * init_self_node_info(char* port) {
    int selfPort;
    char *selfAddress = calloc(256, sizeof(char));
    getFQDN(selfAddress, 256);
    if((selfPort = atoi(port)) == -1){
        perror("Invalid port argument");
        exit(EXIT_FAILURE);
    }
    return create_node_info(selfAddress, selfPort);
}

void free_node( node_info *node) {
    free(node);
}

node_info *create_node_info(char *address, int port){
    node_info* node = calloc(1, sizeof(node_info));
    node->port = port;
    node->address = address;
    return node;
}
