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

#ifndef BUFSIZE
#define BUFSIZE 100
/* Convenience function for error handling */
void perror_exit(char msg[]);
void *socket_read_from(void *socket);
void *socket_write_to(void *socket);

#endif

#endif //RING_SOCKET_HELPER_H
