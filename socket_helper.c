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
    return socket_fd;
}

int socket_tcp_create(){
    int socket_fd = socket(AF_INET,	SOCK_STREAM, 0);
    if(socket_fd < 0){
        perror_exit("socket()");
    }
    return socket_fd;
}

void socket_make_reusable(int socket) {
    int reuseaddr=1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))==-1) {
        perror_exit("setsockopt(reuseaddr)");
    }
}

void socket_connect(int port, char *ip_address, int socket) {
    struct sockaddr_in serv_addr = {0};
    serv_addr.sin_family = AF_INET;
    //make string ip address into network byte order.
    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        perror_exit("inet_pton()");
    }
    //convert port integer to network byte order
    serv_addr.sin_port = htons(port);
    if (connect(socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror_exit("connect()");
    }
}

void socket_bind(int port, int socket) {
    /* Bind socket to port 'listen_port' */
    struct sockaddr_in addr={0}; //init fields to zero
    addr.sin_family = AF_INET; //the address is ipv4
    addr.sin_addr.s_addr = INADDR_ANY; //use any address, eg. 0.0.0.0 for ipv4
    addr.sin_port = htons(port); //convert the port number to network-byte-order

    if(bind(socket, (struct sockaddr*) &addr, sizeof(addr)) < 0){
        perror_exit("bind()");
    }
}

void socket_tcp_listen(int socket){
    /* Start listening on the socket, note that the second argument 'backlog' specifies how
     * many clients can be queued while waiting for the server to accept() */
    int backlog = 1;//we're only handling a single client in this example
    listen(socket, backlog);
}

int socket_tcp_get_connecting_socket(int socket){
    int client_socket = accept(socket, NULL, NULL);

    if(client_socket<0){
        perror_exit("accept()");
    }
    return client_socket;
}
