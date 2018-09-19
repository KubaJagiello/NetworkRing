
#ifndef NODE_H
#define NODE_H

#include<stdlib.h>
#include "queue.h"

typedef struct node_info{
    char *address;
    int port;
} node_info;

typedef struct socket_and_queue{
    struct queue* queue;
    int socket_fd;
    struct node_info* server_info;
    struct node_info* client_info;
    bool is_tcp;
    struct timer_node* timer_info;
} socket_and_queue;

typedef struct timer_node{
    bool is_winner;
    int num_laps;
    float avg_time;
    time_t timer;
} timer_node;

static const int NUM_LAPS_FOR_TIMING = 50000;
bool sigint;

void node_mem_free_function(data);

node_info* create_node_info(char *address, int port);

void free_all(node_info *self, node_info *target);

node_info* init_target_node_info(char *address, char *port);

node_info* init_self_node_info(char* port);

void usage_error(const char *const *argv);

void parse_message(char message[100], queue *q, struct node_info *info, struct timer_node *timer_node);

void *socket_ring_reader(void *sq) ;

void *socket_ring_writer(void *sq);

bool first_arg_is_bigger(char *self_id, char *other_id);

void start_election(const node_info *info, int socket);

bool str_is_equal(const char *str1, const char *str2);

void message_election_over_logic(char message[100], queue *q, char *self_id, char *other_id, struct timer_node *timer_node);

void message_election_logic(queue *q, node_info *info, char *self_id, char *other_id);

void message_normal_logic(char *message, queue *q);

int socket_tcp_connect(int writer_socket, const node_info *writer_info);

socket_and_queue *socket_and_queue_create(node_info *server_info, node_info *client_info, int server_socket, queue *q);

void start_threads_for_node(socket_and_queue *server_sq, socket_and_queue *client_sq);

void set_socket_type(const char *type_of_node, int *client_socket, int *server_socket);

pthread_t start_sender_thread(socket_and_queue *server_sq);

pthread_t start_reader_thread(socket_and_queue *client_sq);

void init_ring(const char **argv);

clock_t clock_difference_in_ms(const struct timer_node *timer_node);

float clock_average_time(const struct timer_node *timer_node);

void timer_lap(struct timer_node *timer_node);

#endif //NODE_H