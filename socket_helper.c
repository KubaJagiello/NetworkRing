#include "socket_helper.h"

void perror_exit(char msg[]) {
    perror(msg);
    exit(1);
}

void *socket_read_from(void *socket) {
    int socket_fd = *((int*) socket);

    while(1){
        /* Read up to BUFSIZE from the socket and print to stdout. */
        char buf[BUFSIZE];
        ssize_t len = recvfrom(socket_fd, buf, BUFSIZE, 0, NULL, NULL);
        if(len==0){//EOF - socket is closed
            return 0;
        } else if(len<0){//error code
            perror_exit("read()");
        } else {
            //Print the data that was read from the socket to stdout.
            fwrite(buf, sizeof(char), len, stdout);
        }
    }
}


void *socket_write_to(void *socket) {
    int socket_fd = *((int*) socket);
    while(1){
        char *buf = calloc(BUFSIZE, sizeof(char));
        ssize_t len = read(STDIN_FILENO, buf, BUFSIZE);
        //eof
        if(len==0){
            return 0;
        } else if(len < 0){
            perror_exit("read()");
        }

        if(send(socket_fd, buf, BUFSIZE, 0) == -1){
            perror_exit("write()");
        }
    }
}

int socket_udp_create() {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        perror_exit("socket()");
    }
    socket_make_reusable(socket_fd);
    return socket_fd;
}

int socket_tcp_create(){
    int socket_fd = socket(AF_INET,	SOCK_STREAM, 0);
    if(socket_fd < 0){
        perror_exit("socket()");
    }
    socket_make_reusable(socket_fd);
    return socket_fd;
}

void socket_make_reusable(int socket) {
    int reuseaddr=1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))==-1) {
        perror_exit("setsockopt(reuseaddr)");
    }
}

int socket_connect(int port, char *ip_address, int socket) {
    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    //make string ip address into network byte order.
    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        fprintf(stderr, "ip: %s \n", ip_address);
        perror_exit("inet_pton()");
    }
    //convert port integer to network byte order
    serv_addr.sin_port = htons(port);
    return connect(socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
}

void socket_bind(int port, int socket) {
    struct sockaddr_in addr={0}; //init fields to zero
    addr.sin_family = AF_INET; //the address is ipv4
    addr.sin_addr.s_addr = INADDR_ANY; //use any address, eg. 0.0.0.0 for ipv4
    addr.sin_port = htons(port); //convert the port number to network-byte-order

    if(bind(socket, (struct sockaddr*) &addr, sizeof(addr)) < 0){
        perror_exit("bind()");
    }
}

void socket_tcp_listen(int socket){
    int backlog = 1;
    listen(socket, backlog);
}

int socket_tcp_get_connecting_socket(int socket){
    int client_socket = accept(socket, NULL, NULL);
    if(client_socket<0){
        perror_exit("accept()");
    }
    return client_socket;
}

int socket_single_write_to(int socket, char* message) {
    return (int) send(socket, message, BUFSIZE, 0);
}


ssize_t socket_recvfrom(int port, char *ip_address, int socket, char* message) {
    struct sockaddr_in serv_addr;
    socklen_t fromlen = sizeof(serv_addr);
    return recvfrom(socket, message, BUFSIZE, 0, (struct sockaddr *) &serv_addr,
                    &fromlen);
}