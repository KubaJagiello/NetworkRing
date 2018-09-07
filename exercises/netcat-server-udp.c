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
//    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
//    if (server_socket < 0) {
//        perror_exit("socket()");
//    }
//
//    /* Optional step, allow address reuse
//       (needed if you want to use the same port after an unclean shutdown of your program).
//     */
//    int reuseaddr = 1;
//    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) == -1) {
//        perror_exit("setsockopt(reuseaddr)");
//    }
//
//    /* Bind socket to port 'listen_port' */
//    struct sockaddr_in addr = {0}; //init fields to zero
//    addr.sin_family = AF_INET; //the address is ipv4
//    addr.sin_addr.s_addr = INADDR_ANY; //use any address, eg. 0.0.0.0 for ipv4
//    addr.sin_port = htons(listen_port); //convert the port number to network-byte-order
//
//    if (bind(server_socket, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
//        perror_exit("bind()");
//    }
//
//    /* Since UDP is stateless we're immediately able to receive packets here, no 'listen' required*/
//
//
//    pthread_t thread_reader;
//    pthread_create(&thread_reader, NULL, &socket_read_from, &server_socket);
//    pthread_t thread_writer;
//    pthread_create(&thread_writer, NULL, &socket_write_to, &server_socket);
//    pthread_join(thread_reader, 0);
//    pthread_join(thread_writer, 0);
//}
