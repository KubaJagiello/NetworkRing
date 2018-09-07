//#include <stdlib.h>
//#include <stdio.h>
//
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <pthread.h>
//#include <socket_helper.h>
//
//#define BUFSIZE 100
//
//void *write_to_socket(void *);
//
//int main(int argc, char* argv[]){
//
//    /* Port for the server to listen on */
//    int listen_port;
//    if(argc!=2 || sscanf(argv[1], "%d", &listen_port)!=1){
//        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
//        return 1;
//    }
//
//    /* Create a socket for IPv4 TCP*/
//    int server_socket = socket(AF_INET,	SOCK_STREAM, 0);
//    if(server_socket < 0){
//        perror_exit("socket()");
//    }
//
//    /* Optional step, allow address reuse
//     * (needed if you want to use the same port after an unclean shutdown of your program).
//     */
//    int reuseaddr=1;
//    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))==-1) {
//        perror_exit("setsockopt(reuseaddr)");
//    }
//
//    /* Bind socket to port 'listen_port' */
//    struct sockaddr_in addr={0}; //init fields to zero
//    addr.sin_family = AF_INET; //the address is ipv4
//    addr.sin_addr.s_addr = INADDR_ANY; //use any address, eg. 0.0.0.0 for ipv4
//    addr.sin_port = htons(listen_port); //convert the port number to network-byte-order
//
//    if(bind(server_socket, (struct sockaddr*) &addr, sizeof(addr)) < 0){
//        perror_exit("bind()");
//    }
//
//    /* Start listening on the socket, note that the second argument 'backlog' specifies how
//     * many clients can be queued while waiting for the server to accept() */
//    int backlog = 1;//we're only handling a single client in this example
//    listen(server_socket, backlog);
//
//    /* Now we wait for a connection, accept() will block until a client connects,
//     * we're not interested in the remote host address in this example so we're ignoring them by passing NULL.
//     */
//    int client_socket = accept(server_socket, NULL, NULL);
//
//    if(client_socket<0){
//        perror_exit("accept()");
//    }
//
//    pthread_t tid;
//
//    fprintf(stderr, "socket: %d\n", client_socket);
//    int error = pthread_create(&tid, NULL, &write_to_socket, &client_socket);
//    if(error == -1){
//        perror_exit("pthread_create()");
//    }
//
//    while(1){
//        /* Read up to BUFSIZE from the socket and print to stdout. */
//        char buf[BUFSIZE];
//        int len = read(client_socket, buf, BUFSIZE);
//        if(len==0){//EOF - socket is closed
//            return 0;
//        } else if(len<0){//error code
//            perror_exit("read()");
//        } else {
//            //Print the data that was read from the socket to stdout.
//            fwrite(buf, sizeof(char), len, stdout);
//        }
//    }
//}
//
//void *write_to_socket(void *s) {
//    int socket = *((int*) s);
//    fprintf(stderr, "socket: %d\n", socket);
//    while(1){
//        char *buf = calloc(BUFSIZE, sizeof(char));
//        int len = read(STDIN_FILENO, buf, BUFSIZE);
//
//        if(len==0){
//            return 0;
//        } else if(len < 0){
//            perror_exit("read()");
//        } else{
//            fprintf(stderr, "writing %s", buf);
//            if(write(socket, buf, BUFSIZE) == -1){
//                perror_exit("write()");
//            }
//            write(STDOUT_FILENO, buf, BUFSIZE);
//        }
//    }
//}
