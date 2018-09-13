#ifndef _SIGHANT_
#define _SIGHANT_
#define _GNU_SOURCE

#include "node.h"
#include <signal.h>


void check_sigint(int signal);

/* Initializes sigaction and sends check_sigint() to it.
 * Returns: 	pointer to sigaction struct.
 */
struct sigaction *initSigint(void);

#endif