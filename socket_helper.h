//
// Created by c16fld on 2018-09-07.
//

#ifndef RING_SOCKET_HELPER_H
#define RING_SOCKET_HELPER_H
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "network_helper.h"
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>

#ifndef BUFSIZE
#define BUFSIZE 100
/* Convenience function for error handling */
void perror_exit(char msg[]);
void *socket_read_from(void *socket);
void *socket_write_to(void *socket);
int socket_single_write_to(int socket, char* message);
int socket_udp_create();
int socket_tcp_create();
void socket_make_reusable(int socket);
void socket_make_timeout(int sockfd);
int socket_connect(int port, char *ip_address, int socket);
void socket_bind(int port, int socket);
void socket_tcp_listen(int socket);
int socket_tcp_get_connecting_socket(int socket);
ssize_t socket_recvfrom(int port, char *ip_address, int socket, char* message);



#endif

#endif //RING_SOCKET_HELPER_H
