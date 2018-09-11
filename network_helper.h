
#ifndef RING_NETWORK_HELPER_H
#define RING_NETWORK_HELPER_H

#include <unistd.h>
#include <netdb.h>
#include <string.h>
int getFQDN(char *fqdn, size_t n);
int hostname_to_ip(const char *hostname, char *ip);
#endif


