#include <stdio.h>
#include <errno.h>
#include <asm/errno.h>
#include "node.h"
#include "network_helper.h"
#include "socket_helper.h"
#include "queue.h"
#include "message_helper.h"
//#include "sighant.h"

#define REQUIRED_ARGUMENT_NUMBER 5

clock_t clock_difference_in_ms(const struct timer_node *timer_node);

float clock_average_time(const struct timer_node *timer_node);

void timer_lap(struct timer_node *timer_node);

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

    //struct sigaction* sigaction = initSigint();
    node_info *server_info = init_self_node_info((char*)argv[2]);
    node_info *client_info = init_target_node_info(ip, (char*)argv[4]);

    int client_socket = -1;
    int server_socket = -1;

    socket_setup(type_of_node, &client_socket, &server_socket);

    queue* q = queue_create();
    queue_set_memory_handler(q, &node_mem_free_function);
    socket_and_queue *server_sq = socket_and_queue_create(server_info, client_info, client_socket, q, server_socket);
    socket_and_queue *client_sq = socket_and_queue_create(server_info, client_info, server_socket, q, client_socket);


    if(strcmp(type_of_node, "tcpnode") == 0){
        server_sq->is_tcp = true;
        client_sq->is_tcp = true;
    } else{
        server_sq->is_tcp = false;
        client_sq->is_tcp = false;
    }
    struct timer_node* timer_info = calloc(1, sizeof(struct timer_node));
    client_sq->timer_info = timer_info;
    start_threads_for_node(server_sq, client_sq);

    queue_free(q);
    //free(sigaction);
    free(timer_info);
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

socket_and_queue *socket_and_queue_create(node_info *server_info, node_info *client_info, int server_socket, queue *q, int client_socket) {
    socket_and_queue *client_sq = calloc(1, sizeof(socket_and_queue));
    client_sq->queue = q;
    client_sq->socket_fd = server_socket;
    client_sq->client_info = client_info;
    client_sq->server_info = server_info;
    client_sq->other_socket = client_socket;
    return client_sq;
}

void start_election(const node_info *info, int socket) {
    fprintf(stderr, "\nStarting election:\n%s\n\n", message_election_start(info->port));
    socket_single_write_to(socket, message_election_start(info->port));
}

void *socket_ring_writer(void *sq) {
    socket_and_queue* writer_sq = (socket_and_queue*) sq;
    int client_socket = writer_sq->socket_fd;
    queue* q = writer_sq->queue;
    node_info* client_info = writer_sq->client_info;
    node_info* server_info = writer_sq->server_info;
    bool is_tcp = writer_sq->is_tcp;

    char* message_to_send;
    if(socket_tcp_connect(client_socket, client_info) == -1){
        //sigint
        queue_release_threads();
        close(client_socket);
        return 0;
    }
    if(is_tcp){
        start_election(server_info, client_socket);
    } else{
        message_to_send = message_election_start(server_info->port);
    }

    while(1){
        if(!queue_is_empty(q) || is_tcp || message_is_normal(message_to_send) || message_is_election_over(message_to_send)){
            message_to_send = (char*)queue_dequeue(q);
        }
        if(sigint){
            queue_release_threads();
            shutdown(writer_sq->other_socket, SHUT_RDWR);
            close(client_socket);
            return 0;
        }
        if(socket_single_write_to(client_socket, message_to_send) == -1 && errno != ECONNREFUSED){
            fprintf(stderr, "socket_single_Write() error\n");
            return 0;
        }
    }
}

void *socket_ring_reader(void *sq) {
    socket_and_queue* reader_sq = (socket_and_queue*) sq;
    int server_socket = reader_sq->socket_fd;
    queue* q = reader_sq->queue;
    node_info* server_info = reader_sq->server_info;
    socket_bind(server_info->port, server_socket);
    bool is_tcp = reader_sq->is_tcp;

    int client_socket = server_socket;
    if(is_tcp) {
        socket_tcp_listen(server_socket);
        client_socket = socket_tcp_get_connecting_socket(server_socket);
    }
    while(1){
        char message[BUFSIZE];
        ssize_t len = recv(client_socket, message, BUFSIZE, 0);
        if(sigint){
            queue_release_threads();
            if(is_tcp){
                close(client_socket);
            }
            close(server_socket);
            return 0;
        }
        if(len==0){
            return 0;
        } else if(len<0){
            perror("recvfrom()");
        } else {
            parse_message(message, q, reader_sq->server_info, reader_sq->timer_info);
        }
    }
}



int socket_tcp_connect(int writer_socket, const node_info *writer_info) {
    while(socket_connect(writer_info->port, writer_info->address, writer_socket) == -1){
        if(sigint){
            return -1;
        }
        fprintf(stderr, "\nCould not connect to ip: %s\nport: %d\n retrying...\n", writer_info->address, writer_info->port);
        sleep(1);
    }
    return 0;
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

void parse_message(char message[100], queue *q, struct node_info *info, struct timer_node *timer_node) {
    char* self_id = get_my_id(info->port);
    char* other_id = message_get_id_value(message);
    //fprintf(stderr, "\nRECIVING:\n%s\n\n", message);

    if(message_is_normal(message)){
        timer_lap(timer_node);
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
        message_election_over_logic(message, q, self_id, other_id, timer_node);
        free(self_id);
        return;
    }
    fprintf(stderr, "Unknown message type.\n");
    fprintf(stderr, "\n\n");
}

void timer_lap(struct timer_node *timer_node) {
    if(timer_node->is_winner){
            timer_node->num_laps++;
            if(timer_node->num_laps % 50000 == 0){
                timer_node->avg_time = clock_difference_in_ms(timer_node);
                fprintf(stdout, "Avg clock time is %fs for 50,000 laps\n", clock_average_time(timer_node));
                timer_node->timer = clock();
                timer_node->num_laps = 0;
            }
        }
}

float clock_average_time(const struct timer_node *timer_node) { return timer_node->avg_time / timer_node->num_laps; }

clock_t clock_difference_in_ms(const struct timer_node *timer_node) {
    return ((clock() - timer_node->timer) * 1000) / CLOCKS_PER_SEC;
}

void message_normal_logic(char *message, queue *q) {
    queue_enqueue(q, message);
}

void message_election_logic(queue *q, node_info *info, char *self_id, char *other_id) {
    if(str_is_equal(self_id, other_id)){
        fprintf(stderr, "I won the election.\n");
        queue_enqueue(q, message_election_over(info->port));
    } else if(first_arg_is_bigger(self_id, other_id)){
        queue_enqueue(q, create_message(ELECTION, self_id));
    } else{
        queue_enqueue(q, create_message(ELECTION, other_id));
    }
}

void message_election_over_logic(char message[100], queue *q, char *self_id, char *other_id, struct timer_node *timer_node) {
    if(str_is_equal(self_id, other_id)){
        timer_node->num_laps = 1;
        timer_node->is_winner = true;
        timer_node->timer = clock();
        queue_enqueue(q, message_normal(self_id));
    } else{
        queue_enqueue(q, message);
    }
}

bool str_is_equal(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}

bool first_arg_is_bigger(char *self_id, char *other_id) {
    return  strcmp(self_id, other_id) > 0;
}

node_info *create_node_info(char *address, int port){
    node_info* node = calloc(1, sizeof(node_info));
    node->port = port;
    node->address = address;
    return node;
}

void node_mem_free_function(data message) {
    char* string = (char*) message;
    free(string);
}


