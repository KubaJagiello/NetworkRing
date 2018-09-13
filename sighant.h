#ifndef _SIGHANT_
#define _SIGHANT_
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include "node.h"


void check_sigint(int signal);

/* Initializes sigaction and sends check_sigint() to it.
 * Returns: 	pointer to sigaction struct.
 */
struct sigaction *initSigint(void);

#endif