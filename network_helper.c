#include <arpa/inet.h>
#include "network_helper.h"
#include "socket_helper.h"


int getFQDN(char *fqdn, size_t n) {
    char hostname[256];
    int r = gethostname(hostname, 256);
    if (r != 0) {
        return 1;
    }
    struct addrinfo h = {0};
    h.ai_family = AF_INET;
    h.ai_socktype = SOCK_STREAM;
    h.ai_flags = AI_CANONNAME;
    struct addrinfo *info;
    if (getaddrinfo(hostname, NULL, &h, &info) != 0) {
        return 2;
    }
    strncpy(fqdn, info->ai_canonname, n);
    freeaddrinfo(info);
    return 0;
}

int hostname_to_ip(const char *hostname, char *ip) {
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(hostname)) == NULL) {
        // get the host server_info
        perror_exit("gethostbyname");
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    //Return the first one;
    strcpy(ip, inet_ntoa(*addr_list[0]));
    return 0;
}