//#include <stdlib.h>
//#include <stdio.h>
//
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include "network_helper.h"
//#include <arpa/inet.h>
//#include <socket_helper.h>
//#include <pthread.h>
//
//
//int main(int argc, char *argv[]) {
//    /* Port for the server to listen on */
//    int output_port;
//    char *output_ip_address;
//    if (argc != 3 || sscanf(argv[1], "%d", &output_port) != 1) {
//        fprintf(stderr, "1 Usage: %s <port> <ip>\n", argv[0]);
//        return 1;
//    }
//
//    output_ip_address = argv[2];
//    int client_socket = socket_udp_create();
//    socket_connect(output_port, output_ip_address, client_socket);
//
//    pthread_t thread_reader;
//    pthread_create(&thread_reader, NULL, &socket_read_from, &client_socket);
//    pthread_t thread_writer;
//    pthread_create(&thread_writer, NULL, &socket_write_to, &client_socket);
//    pthread_join(thread_reader, 0);
//    pthread_join(thread_writer, 0);
//    pthread_exit(0);
//}