#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "network_helper.h"

#define BUFSIZE BUFSIZE

/* Convenience function for error handling */
void perror_exit(char msg[]) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    /* Port for the server to listen on */
    int output_port;
    char *output_ip_address;
    if (argc != 2 || sscanf(argv[1], "%d", &output_port) != 1 || sscanf(argv[2], "%s", output_ip_address) != -1) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket < 0){
        perror_exit("socket()");
    }

    int reuseaddr=1;
    if (setsockopt(client_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr))==-1) {
        perror_exit("setsockopt(reuseaddr)");
    }

    struct sockaddr_in serv_addr={0};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = output_ip_address;
    serv_addr.sin_port = output_port;

    if(connect(client_socket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
        perror_exit("bind()");
    }


}