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
        int len = recvfrom(socket_fd, buf, BUFSIZE, 0, NULL, NULL);
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
