#include "sighant.h"

extern bool sigint;

/* If SIGINT occours then sigint variable in mish.c will be changed to true.
 */
void check_sigint(int signal){
    if(signal == SIGINT){
        sigint = true;
        fprintf(stderr, "\nEYYYYYY\n");
    }
}

/* Initializes sigaction and sends check_sigint() to it.
 * Returns: 	pointer to sigaction struct.
 */
struct sigaction *initSigint(void){
    struct sigaction *sig = calloc(1, sizeof(struct sigaction));
    sig->sa_handler = check_sigint;
    sig->sa_flags = SA_RESTART;
    if(sigaction(SIGINT, sig, NULL) != 0)
        exit(EXIT_FAILURE);
    return sig;
}