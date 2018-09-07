//#include <stdlib.h>
//#include <stdio.h>
//
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include "network_helper.h"
//#include <arpa/inet.h>
//#include <pthread.h>
//#include "socket_helper.h"
//
//
//#define SEND_BUFSIZE 100
//
//void *read_from_socket(void *);
//
//
//
//int main(int argc, char *argv[]) {
//
//    /* Port for the server to listen on */
//    int output_port;
//    char *output_ip_address;
//    if (argc != 3 || sscanf(argv[1], "%d", &output_port) != 1) {
//        fprintf(stderr, "1 Usage: %s <port> <ip>\n", argv[0]);
//        return 1;
//    }
//    output_ip_address = argv[2];
//
//    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
//    if(server_socket < 0){
//        perror_exit("socket()");
//    }
//
//    int reuseaddr=1;
//    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))==-1) {
//        perror_exit("setsockopt(reuseaddr)");
//    }
//
//    fprintf(stderr, "%d %s\n", output_port, output_ip_address);
//
//    struct sockaddr_in serv_addr={0};
//    serv_addr.sin_family = AF_INET;
//    int error;
//
//    if((error = inet_pton(AF_INET, output_ip_address, &serv_addr.sin_addr))<=0)
//    {
//        fprintf(stderr, "errorcode: %d\n", error);
//        perror_exit("inet_pton()");
//    }
//    serv_addr.sin_port = htons(output_port);
//
//    if(connect(server_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
//        perror_exit("connect()");
//    }
//
//    pthread_t thread_reader;
//    pthread_create(&thread_reader, NULL, &socket_read_from, &server_socket);
//    pthread_t thread_writer;
//    pthread_create(&thread_writer, NULL, &socket_write_to, &server_socket);
//    pthread_join(thread_reader, 0);
//    pthread_join(thread_writer, 0);
//    return 0;
//}
