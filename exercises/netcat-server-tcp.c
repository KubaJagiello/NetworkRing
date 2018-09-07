#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <socket_helper.h>

#define BUFSIZE 100

void *write_to_socket(void *);

int main(int argc, char* argv[]){

    /* Port for the server to listen on */
    int listen_port;
    if(argc!=2 || sscanf(argv[1], "%d", &listen_port)!=1){
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    /* Create a socket for IPv4 TCP*/
    int server_socket = socket_tcp_create();
    socket_make_reusable(server_socket);
    socket_tcp_bind(listen_port, server_socket);
    socket_tcp_listen(server_socket);
    int client_socket = socket_tcp_get_connecting_socket(server_socket);
    
    pthread_t tid_reader;
    pthread_t tid_writer;

    fprintf(stderr, "socket: %d\n", client_socket);
    pthread_create(&tid_reader, NULL, &socket_write_to, &client_socket);
    pthread_create(&tid_writer, NULL, &socket_read_from, &client_socket);
    pthread_join(tid_reader, 0);
    pthread_join(tid_writer, 0);
    pthread_exit(0);
}