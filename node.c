#include <stdio.h>
#include "node.h"
#include "network_helper.h"
#include "socket_helper.h"
#include "queue.h"

#define REQUIRED_ARGUMENT_NUMBER 5

void free_all(node_info *self, node_info *target);
node_info * init_target_node_info(char *address, char *port);
node_info * init_self_node_info(char* port);

void usage_error(const char *const *argv);

char *parse_message(char *string);

void *socket_ring_receiver(void *sq) ;
void *socket_ring_sender(void *sq) ;

//{tcpnode,udpnode} local-port next-host next-port
int main(int argc, char const *argv[]) {

    // lyssna på en port
    // connecta till en port

    if(argc != REQUIRED_ARGUMENT_NUMBER ){
        usage_error(argv);
    }

    const char* tcp = argv[1];
    node_info *client_info = init_self_node_info((char*)argv[2]);
    node_info *server_info = init_target_node_info((char*)argv[3], (char*)argv[4]);
    int server_socket = -1;
    int client_socket = -1;
    if(strcmp(tcp, "tcpnode") == 0){
        server_socket = socket_tcp_create();
        client_socket = socket_tcp_create();
    } else if(strcmp(tcp, "udpnode") == 0){
        server_socket = socket_udp_create();
        client_socket = socket_udp_create();
    } else{
        usage_error(argv);
    }

    socket_connect(server_info->port, server_info->address, server_socket);
    socket_bind(client_info->port, client_socket);
    queue* q = queue_create();
    socket_and_queue *server_sq = calloc(1, sizeof(socket_and_queue));
    server_sq->queue = q;
    server_sq->socket_fd = server_socket;

    socket_and_queue *client_sq = calloc(1, sizeof(socket_and_queue));
    client_sq->queue = q;
    client_sq->socket_fd = client_socket;

    pthread_t thread_receiver;
    pthread_create(&thread_receiver, NULL, &socket_ring_receiver, client_sq);
    pthread_t thread_sender;
    pthread_create(&thread_sender, NULL, &socket_ring_sender, server_sq);
    pthread_join(thread_receiver, 0);
    pthread_join(thread_sender, 0);

    queue_free(q);
    free(client_sq);
    free(server_sq);
    free_all(client_info, server_info);
    return 0;
}

void *socket_ring_receiver(void *sq) {
    socket_and_queue* client_sq = (socket_and_queue*) sq;
    int socket_fd = client_sq->socket_fd;
    queue* q = client_sq->queue;

    while(1){
        char message[BUFSIZE];
        ssize_t len = recvfrom(socket_fd, message, BUFSIZE, 0, NULL, NULL);
        if(len==0){//EOF - socket is closed
            return 0;
        } else if(len<0){//error code
            perror_exit("read()");
        } else {
            queue_enqueue(q, message);
        }
    }
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void *socket_ring_sender(void *sq) {
    socket_and_queue* server_sq = (socket_and_queue*) sq;
    int socket_fd = server_sq->socket_fd;
    queue* q = server_sq->queue;

    while(1){
        //queue is blocking so no worry of endless loop.
        char* message = parse_message((char*)queue_dequeue(q));
        if(send(socket_fd, message, BUFSIZE, 0) == -1){
            perror_exit("write()");
        }
    }
}
#pragma clang diagnostic pop

char *parse_message(char *string) {


    return NULL;
}

void usage_error(const char *const *argv) {
    fprintf(stderr, "Usage: %s {tcpnode, udpnode} <local-port> <next-host-ip> <next-port>\n", argv[0]);
    exit(EXIT_FAILURE);
}

void free_all(node_info *self, node_info *target) {
    free(self);
    free(target);
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


node_info *create_node_info(char *address, int port){
    node_info* node = calloc(1, sizeof(node_info));
    node->port = port;
    node->address = address;
    return node;
}
