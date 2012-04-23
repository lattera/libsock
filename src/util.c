#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "sock.h"
#include "util.h"

void *xmalloc(size_t sz)
{
    void *p;

    p = malloc(sz);
    if ((p))
        memset(p, 0x00, sz);

    return p;
}

void xfree(void *p, size_t sz)
{
    if (!(p))
        return;

    memset(p, 0x00, sz);
    free(p);
}

SOCK *get_sock_client(const char *host, const char *port, int protocol)
{
    SOCK *sock;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    switch (protocol) {
        case SOCK_TCP:
        case SOCK_UDP:
            break;
        default:
            return NULL;
    }

    memset(&hints, 0x00, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = protocol;

    rv = getaddrinfo(host, port, &hints, &servinfo);
    if (rv != 0)
        return NULL;

    sock = xmalloc(sizeof(SOCK));
    sock->protocol = protocol;

    for (p = servinfo; p != NULL; p = p->ai_next) {
        sock->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock->sockfd == -1)
            continue;

        if (protocol == SOCK_TCP) {
            if (connect(sock->sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(sock->sockfd);
                continue;
            }
        }

        sock->flags |= SOCK_FLAG_ACTIVE;

        break;
    }

    if (!(p)) {
        free(sock);
        return NULL;
    }

    sock->servinfo = servinfo;
    sock->addrinfo = p;
    sock->ipaddr = get_udp_packet_ip(sock, p->ai_addr);

    return sock;
}

char *get_udp_packet_ip(SOCK *s, void *addrinfo)
{
    char *name;

    name = xmalloc(INET6_ADDRSTRLEN+1);
    if (!(name))
        return NULL;

    switch (s->addrinfo->ai_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)addrinfo)->sin_addr), name, INET_ADDRSTRLEN);
            break;
        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)addrinfo)->sin6_addr), name, INET6_ADDRSTRLEN);
            break;
        default:
            free(name);
            return NULL;
    }

    return name;
}
