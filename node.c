#include <stdio.h>
#include <errno.h>
#include <asm/errno.h>
#include "node.h"
#include "network_helper.h"
#include "socket_helper.h"
#include "queue.h"
#include "message_helper.h"

#define REQUIRED_ARGUMENT_NUMBER 5
#define TCP_NODE "tcpnode"
#define UDP_NODE "udpnode"

int main(int argc, char const *argv[]) {
    if (argc != REQUIRED_ARGUMENT_NUMBER) {
        usage_error(argv);
    }
    init_ring(argv);
    return 0;
}

/**
 * initiates the full node with port, transport protocol and target address
 * @param argv cmd arguments
 */
void init_ring(const char **argv) {
    char ip[MAX_SIZE];
    hostname_to_ip(argv[3], ip);
    const char *type_of_node = argv[1];

    node_info *server_info = init_self_node_info((char *) argv[2]);
    node_info *client_info = init_target_node_info(ip, (char *) argv[4]);

    int client_socket = -1;
    int server_socket = -1;

    //tcp or udp
    set_socket_type(type_of_node, &client_socket, &server_socket);

    //fill information needed for threads
    queue *q = queue_create();
    queue_set_memory_handler(q, &node_mem_free_function);
    socket_and_queue *server_sq = socket_and_queue_create(server_info, client_info, client_socket, q);
    socket_and_queue *client_sq = socket_and_queue_create(server_info, client_info, server_socket, q);

    server_sq->is_tcp = strcmp(type_of_node, TCP_NODE) == 0;
    client_sq->is_tcp = strcmp(type_of_node, TCP_NODE) == 0;

    struct timer_node *timer_info = calloc(1, sizeof(struct timer_node));
    client_sq->timer_info = timer_info;
    start_threads_for_node(server_sq, client_sq);

    queue_free(q);
    free(timer_info);
    free(client_sq);
    free(server_sq);
    free_all(server_info, client_info);
}

/**
 * Set tcp or udp for sockets
 * @param type_of_node
 * @param client_socket
 * @param server_socket
 */
void set_socket_type(const char *type_of_node, int *client_socket, int *server_socket) {
    if (strcmp(type_of_node, TCP_NODE) == 0) {
        (*client_socket) = socket_tcp_create();
        (*server_socket) = socket_tcp_create();
    } else if (strcmp(type_of_node, UDP_NODE) == 0) {
        (*client_socket) = socket_udp_create();
        (*server_socket) = socket_udp_create();
    } else {
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

/**
 * Creates information needed for node functions sent to threads
 * @param server_info
 * @param client_info
 * @param server_socket
 * @param q
 * @return
 */
socket_and_queue *socket_and_queue_create(node_info *server_info, node_info *client_info, int server_socket, queue *q) {
    socket_and_queue *client_sq = calloc(1, sizeof(socket_and_queue));
    client_sq->queue = q;
    client_sq->socket_fd = server_socket;
    client_sq->client_info = client_info;
    client_sq->server_info = server_info;
    return client_sq;
}

void start_election(const node_info *info, int socket) {
    socket_single_write_to(socket, message_election_start(info->port));
}

/**
 * The function that will write to the next node in the ring.
 * @param sq
 * @return
 */
void *socket_ring_writer(void *sq) {
    //load variables from socket_and_queue struct
    socket_and_queue *writer_sq = (socket_and_queue *) sq;
    int client_socket = writer_sq->socket_fd;
    queue *q = writer_sq->queue;
    node_info *client_info = writer_sq->client_info;
    node_info *server_info = writer_sq->server_info;
    bool is_tcp = writer_sq->is_tcp;


    //setup socket connection
    char *message_to_send = NULL;
    if (socket_tcp_connect(client_socket, client_info) == -1) {
        queue_release_threads();
        close(client_socket);
        return 0;
    }

    if (is_tcp) {
        start_election(server_info, client_socket);
    } else {
        message_to_send = message_election_start(server_info->port);
    }

    //write logic
    while (1) {
        if (!queue_is_empty(q) || is_tcp || message_is_normal(message_to_send) ||
            message_is_election_over(message_to_send)) {
            message_to_send = (char *) queue_dequeue(q);
        }
        //fprintf(stderr, "Sending: %s\n", message_to_send);
        if (socket_single_write_to(client_socket, message_to_send) == -1 && errno != ECONNREFUSED) {
            fprintf(stderr, "socket_single_Write() error\n");
            return 0;
        }
    }
}

/**
 * the function that will read from the previous node.
 * @param sq
 * @return
 */
void *socket_ring_reader(void *sq) {
    //load variables from socket_and_queue struct
    socket_and_queue *reader_sq = (socket_and_queue *) sq;
    int server_socket = reader_sq->socket_fd;
    queue *q = reader_sq->queue;
    node_info *server_info = reader_sq->server_info;
    bool is_tcp = reader_sq->is_tcp;

    //setup socket
    socket_bind(server_info->port, server_socket);
    int client_socket = server_socket;
    if (is_tcp) {
        socket_tcp_listen(server_socket);
        client_socket = socket_tcp_get_connecting_socket(server_socket);
    }

    //read logic
    while (1) {
        char message[BUFSIZE];
        ssize_t len = recv(client_socket, message, BUFSIZE, 0);

        if (len == 0) {
            return 0;
        } else if (len < 0) {
            perror("recvfrom()");
        } else {
            parse_message(message, q, reader_sq->server_info, reader_sq->timer_info);
        }
    }
}

/**
 * keep trying to connect, only used for tcp
 * @param writer_socket
 * @param writer_info
 * @return
 */
int socket_tcp_connect(int writer_socket, const node_info *writer_info) {
    while (socket_connect(writer_info->port, writer_info->address, writer_socket) == -1) {
        fprintf(stderr, "\nCould not connect to ip: %s\nport: %d\n retrying...\n", writer_info->address,
                writer_info->port);
        sleep(1);
    }
    return 0;
}

/**
 * prints usage error
 * @param argv
 */
void usage_error(const char *const *argv) {
    fprintf(stderr, "Usage: %s {tcpnode, udpnode} <local-port> <next-host-ip> <next-port>\n", argv[0]);
    exit(EXIT_FAILURE);
}

void free_all(node_info *self, node_info *target) {
    free(self);
    free(target);
}

//create port and ip address struct
node_info *init_target_node_info(char *address, char *port) {
    int targetPort;
    if ((targetPort = atoi(port)) == -1) {
        perror("Invalid port argument");
        exit(EXIT_FAILURE);
    }
    return create_node_info(address, targetPort);
}

/**
 * load port and address into a struct
 * @param port
 * @return
 */
node_info *init_self_node_info(char *port) {
    int selfPort;
    char *selfAddress = calloc(MAX_SIZE, sizeof(char));
    getFQDN(selfAddress, MAX_SIZE);
    hostname_to_ip(selfAddress, selfAddress);
    if ((selfPort = atoi(port)) == -1) {
        perror("Invalid port argument");
        exit(EXIT_FAILURE);
    }
    return create_node_info(selfAddress, selfPort);
}

/**
 * parses the given message and adds it to the queue for sending.
 * The parsing algorithm is the Chang and Roberts ring-based coordinator election algorithm
 * @param message
 * @param q
 * @param info
 * @param timer_node
 */
void parse_message(char message[100], queue *q, struct node_info *info, struct timer_node *timer_node) {
    char *self_id = get_my_id(info->port);
    char *other_id = message_get_id_value(message);
    if (message_is_normal(message)) {
        timer_lap(timer_node);
        message_normal_logic(message, q);
        free(self_id);
        return;
    }
    if (message_is_election(message)) {
        message_election_logic(q, info, self_id, other_id);
        free(self_id);
        return;
    }
    if (message_is_election_over(message)) {
        message_election_over_logic(message, q, self_id, other_id, timer_node);
        free(self_id);
        return;
    }
    fprintf(stderr, "Unknown message type.\n");
    fprintf(stderr, "\n\n");
}

/**
 * times the average lap time when the num laps for timing count has laps has passed
 * @param timer_node 
 */
void timer_lap(struct timer_node *timer_node) {
    if (timer_node->is_winner) {
        timer_node->num_laps++;
        if (timer_node->num_laps % NUM_LAPS_FOR_TIMING == 0) {
            timer_node->avg_time = clock_difference_in_ms(timer_node);
            fprintf(stdout, "Avg clock time is %fs for %d laps\n", clock_average_time(timer_node), NUM_LAPS_FOR_TIMING);
            timer_node->timer = clock();
            timer_node->num_laps = 0;
        }
    }
}

float clock_average_time(const struct timer_node *timer_node) {
    return timer_node->avg_time / timer_node->num_laps;
}

clock_t clock_difference_in_ms(const struct timer_node *timer_node) {
    return ((clock() - timer_node->timer)) / CLOCKS_PER_SEC;
}

void message_normal_logic(char *message, queue *q) {
    queue_enqueue(q, message);
}

void message_election_logic(queue *q, node_info *info, char *self_id, char *other_id) {
    queue_empty(q);
    if (str_is_equal(self_id, other_id)) {
        queue_enqueue(q, message_election_over(info->port));
    } else if (first_arg_is_bigger(self_id, other_id)) {
        queue_enqueue(q, create_message(ELECTION, self_id));
    } else {
        queue_enqueue(q, create_message(ELECTION, other_id));
    }
}

void message_election_over_logic(char message[100], queue *q, char *self_id, char *other_id, struct timer_node *timer_node) {
    if (str_is_equal(self_id, other_id)) {
        timer_node->num_laps = 1;
        timer_node->is_winner = true;
        timer_node->timer = clock();
        queue_enqueue(q, message_normal(self_id));
    } else {
        queue_enqueue(q, message);
    }
}

bool str_is_equal(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}

bool first_arg_is_bigger(char *self_id, char *other_id) {
    return strcmp(self_id, other_id) > 0;
}

node_info *create_node_info(char *address, int port) {
    node_info *node = calloc(1, sizeof(node_info));
    node->port = port;
    node->address = address;
    return node;
}

void node_mem_free_function(data message) {
    char *string = (char *) message;
    free(string);
}