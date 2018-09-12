#include <stdio.h>
#include "node.h"
#include "network_helper.h"
#include "socket_helper.h"
#include "queue.h"
#include "message_helper.h"

#define REQUIRED_ARGUMENT_NUMBER 5

void socket_tcp_connect(int writer_socket, const node_info *writer_info);

socket_and_queue *socket_and_queue_create(node_info *server_info, node_info *client_info, int server_socket, queue *q);

void start_threads_for_node(socket_and_queue *server_sq, socket_and_queue *client_sq);

void socket_setup(const char *type_of_node, int *client_socket, int *server_socket);

pthread_t start_sender_thread(socket_and_queue *server_sq);

pthread_t start_reader_thread(socket_and_queue *client_sq);

void start_node(const char **argv);

//{tcpnode,udpnode} local-port next-host next-port
int main(int argc, char const *argv[]) {
    if(argc != REQUIRED_ARGUMENT_NUMBER){
        usage_error(argv);
    }
    start_node(argv);
    return 0;
}

void start_node(const char **argv) {
    char ip[MAX_SIZE];
    hostname_to_ip(argv[3], ip);
    const char* type_of_node = argv[1];

    node_info *server_info = init_self_node_info((char*)argv[2]);
    node_info *client_info = init_target_node_info(ip, (char*)argv[4]);

    int client_socket = -1;
    int server_socket = -1;

    socket_setup(type_of_node, &client_socket, &server_socket);

    queue* q = queue_create();
    socket_and_queue *server_sq = socket_and_queue_create(server_info, client_info, client_socket, q);
    socket_and_queue *client_sq = socket_and_queue_create(server_info, client_info, server_socket, q);

    start_threads_for_node(server_sq, client_sq);

    queue_free(q);
    free(client_sq);
    free(server_sq);
    free_all(server_info, client_info);
}

void socket_setup(const char *type_of_node, int *client_socket, int *server_socket) {
    if(strcmp(type_of_node, "tcpnode") == 0){
        (*client_socket) = socket_tcp_create();
        (*server_socket) = socket_tcp_create();
    } else if(strcmp(type_of_node, "udpnode") == 0){
        (*client_socket) = socket_udp_create();
        (*server_socket) = socket_udp_create();
    } else{
        perror_exit("socket_setup()");
    }
}

void start_threads_for_node(socket_and_queue *server_sq, socket_and_queue *client_sq) {
    pthread_t thread_reader = start_reader_thread(client_sq);
    pthread_t thread_sender = start_sender_thread(server_sq);
    pthread_join(thread_reader, 0);
    pthread_join(thread_sender, 0);
}

pthread_t start_reader_thread(socket_and_queue *client_sq) {
    pthread_t thread_reader;
    pthread_create(&thread_reader, NULL, &socket_ring_reader, client_sq);
    return thread_reader;
}

pthread_t start_sender_thread(socket_and_queue *server_sq) {
    pthread_t thread_sender;
    pthread_create(&thread_sender, NULL, &socket_ring_writer, server_sq);
    return thread_sender;
}

socket_and_queue *socket_and_queue_create(node_info *server_info, node_info *client_info, int server_socket, queue *q) {
    socket_and_queue *client_sq = calloc(1, sizeof(socket_and_queue));
    client_sq->queue = q;
    client_sq->socket_fd = server_socket;
    client_sq->client_info = client_info;
    client_sq->server_info = server_info;
    return client_sq;
}

void start_election(const node_info *server_info, int server_socket) {
    fprintf(stderr, "\nStarting election:\n%s\n\n", message_election_start(server_info->address, server_info->port));
    socket_single_write_to(server_socket, message_election_start(server_info->address, server_info->port));
}

void *socket_ring_reader(void *sq) {
    socket_and_queue* reader_sq = (socket_and_queue*) sq;
    int server_socket = reader_sq->socket_fd;
    queue* q = reader_sq->queue;
    node_info* server_info = reader_sq->server_info;

    socket_bind(server_info->port, server_socket);
    socket_tcp_listen(server_socket);
    int reader_socket = socket_tcp_get_connecting_socket(server_socket);
    fprintf(stderr, "\nNode connected to reader\n\n");

    while(1){
        char message[BUFSIZE];
        ssize_t len = recvfrom(reader_socket, message, BUFSIZE, 0, NULL, NULL);
        if(len==0){//EOF - socket is closed
            return 0;
        } else if(len<0){//error code
            perror("recvfrom()");
            sleep(1);
        } else {
            parse_message(message, q, reader_sq->server_info);
        }
    }
}

void *socket_ring_writer(void *sq) {
    socket_and_queue* writer_sq = (socket_and_queue*) sq;
    int client_socket = writer_sq->socket_fd;
    queue* q = writer_sq->queue;
    node_info* client_info = writer_sq->client_info;

    socket_tcp_connect(client_socket, client_info);
    fprintf(stderr, "Writer connected to next node. Starting election..\n\n");
    start_election(writer_sq->server_info, client_socket);

    while(1){
        //queue is blocking so no worry of endless loop.
        char* message = (char*)queue_dequeue(q);
        fprintf(stderr, "Sending:\n%s\n\n", message);

        if(send(client_socket, message, BUFSIZE, 0) == -1){
            perror_exit("write()");
            break;
        }
    }
    return 0;
}

void socket_tcp_connect(int writer_socket, const node_info *writer_info) {
    while(socket_connect(writer_info->port, writer_info->address, writer_socket) == -1){
        fprintf(stderr, "\nCould not connect to ip: %s\nport: %d\n retrying...\n", writer_info->address, writer_info->port);
        sleep(1);
    }
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
    char *selfAddress = calloc(MAX_SIZE, sizeof(char));
    getFQDN(selfAddress, MAX_SIZE);
    hostname_to_ip(selfAddress, selfAddress);
    if((selfPort = atoi(port)) == -1){
        perror("Invalid port argument");
        exit(EXIT_FAILURE);
    }
    return create_node_info(selfAddress, selfPort);
}



/* 1. Skicka election_start.
 *      -address + port pa egen enhet
 *      - connectad enhet
 *
 * 2. Vid election_meddelande:
 *      Jämnför ID'n:
 *      om ID är samma som sig själv:
 *          - skicka election_over
 *      annars:
 *          - skicka vidare högsta ID
 *
 * 3. Vid election_over meddelande:
 *      Om samma id som sig själv:
 *          - skicka ett vanligt meddelande
 *      annars:
 *          - vidarebefodra
 *
 * 4. Vid vanligt meddelande
 *     - vidarebefodra
 *
 */

void parse_message(char *message, queue *q, node_info* info) {
    char* self_id = message_create_id_from(info->address, info->port);
    char* other_id = message_get_id_value(message);
    sleep(3);
    fprintf(stderr, "Receiving:\n%s\n\n", message);

    if(message_is_normal(message)){
        message_normal_logic(message, q);
        free(self_id);
        return;
    }
    if(message_is_election(message)){
        message_election_logic(q, info, self_id, other_id);
        free(self_id);
        return;
    }
    if(message_is_election_over(message)){
        message_election_over_logic(message, q, self_id, other_id);
        free(self_id);
        return;
    }
    fprintf(stderr, "Unknown message type.\n");

    fprintf(stderr, "\n\n");

}

void message_normal_logic(char *message, queue *q) { queue_enqueue(q, message); }

void message_election_logic(queue *q, node_info *info, char *self_id, char *other_id) {
    if(str_is_equal(self_id, other_id)){
        fprintf(stderr, "I won the election!\n");
        queue_enqueue(q, message_election_over(info->address, info->port));
    } else if(first_arg_is_bigger(self_id, other_id)){
        queue_enqueue(q, create_message(ELECTION, self_id));
    } else{
        queue_enqueue(q, create_message(ELECTION, other_id));
    }
}

void message_election_over_logic(char *message, queue *q, char *self_id, char *other_id) {

    if(strcmp(self_id, other_id) == 0){
        queue_enqueue(q, message_normal("hej..............1"));
    } else{
        queue_enqueue(q, message);
    }
}

bool str_is_equal(const char *self_id, const char *other_id) { return strcmp(self_id, other_id) == 0; }

bool first_arg_is_bigger(char *self_id, char *other_id) {
    return  strcmp(self_id, other_id) < 0;
}

node_info *create_node_info(char *address, int port){
    node_info* node = calloc(1, sizeof(node_info));
    node->port = port;
    node->address = address;
    return node;
}

