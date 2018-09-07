//#include <stdlib.h>
//#include <stdio.h>
//
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <socket_helper.h>
//#include <pthread.h>
//
//int main(int argc, char *argv[]) {
//
//    /* Port for the server to listen on */
//    int listen_port;
//    if (argc != 2 || sscanf(argv[1], "%d", &listen_port) != 1) {
//        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
//        return 1;
//    }
//
//    /* Create a socket for IPv4 UDP*/
//    int server_socket = socket_udp_create();
//    socket_make_reusable(server_socket);
//    socket_bind(listen_port, server_socket);
//
//    /* Since UDP is stateless we're immediately able to receive packets here, no 'listen' required*/
//    pthread_t thread_reader;
//    pthread_create(&thread_reader, NULL, &socket_read_from, &server_socket);
//    pthread_t thread_writer;
//    pthread_create(&thread_writer, NULL, &socket_write_to, &server_socket);
//    pthread_join(thread_reader, 0);
//    pthread_join(thread_writer, 0);
//}
