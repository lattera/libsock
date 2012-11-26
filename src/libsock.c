#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/event.h>
#include <sys/time.h>

#include <errno.h>

#include "sock.h"
#include "util.h"

EXPORTED_SYM SOCK *SocketClient(const char *hostname, const char *port, int protocol)
{
    SOCK *s;

    switch (protocol) {
        case SOCK_TCP:
        case SOCK_UDP:
            break;
        default:
            return NULL;
    }

    s = get_sock_client(hostname, port, protocol);

    return s;
}

EXPORTED_SYM ssize_t SocketWrite(SOCK *sock, const void *data, size_t sz)
{
    ssize_t n=0, i;

    if ((sock->flags & SOCK_FLAG_ACTIVE) != SOCK_FLAG_ACTIVE)
        return 0;

    switch (sock->protocol) {
        case SOCK_TCP:
            do {
                i = send(sock->sockfd, data+n, sz-n, 0);
                if (i < 0 && (errno == EAGAIN))
                    continue;
                if (i < 0)
                    return -1;
                if (i == 0)
                    return n;

                n += i;
            } while (n != sz);
            break;
        case SOCK_UDP:
            do {
                i = sendto(sock->sockfd, data, sz, 0, sock->addrinfo->ai_addr, sock->addrinfo->ai_addrlen);
                if (i < 0 && (errno == EAGAIN))
                    continue;
                if (i < 0)
                    return -1;
                if (i == 0)
                    return n;

                n += i;
            } while (n != sz);
            break;
    }

    return n;
}

EXPORTED_SYM ssize_t SocketRead(SOCK *sock, void *data, size_t sz)
{
    ssize_t n=0, i;

    if ((sock->flags & SOCK_FLAG_ACTIVE) != SOCK_FLAG_ACTIVE)
        return 0;

    switch (sock->protocol) {
        case SOCK_TCP:
            do {
                i = recv(sock->sockfd, data, sz, 0);
                if (i <= 0 && (errno == EAGAIN))
                    continue;
                n = i;
            } while (n == 0);
            break;
        case SOCK_UDP:
            do {
                n = udp_read(sock, data, sz);
            } while (n == 0);
            break;
    }

    return n;
}

ssize_t udp_read(SOCK *sock, void *data, size_t sz)
{
    ssize_t n=0, i;
    socklen_t len=sock->addrinfo->ai_addrlen;
    struct sockaddr *reply_addr=NULL;
    char *name=NULL;

    reply_addr = xmalloc(sock->addrinfo->ai_addrlen);
    do {
        i = recvfrom(sock->sockfd, data, sz, 0, reply_addr, &len);
        if (i <= 0) {
            if (errno == EAGAIN) {
                continue;
            } else {
                free(reply_addr);
                return -1;
            }
        }

        n = i;
    } while (n == 0);

    if (len != sock->addrinfo->ai_addrlen) {
        free(reply_addr);
        return 0;
    }

    name = get_udp_packet_ip(sock, reply_addr);
    if (!(name)) {
        fprintf(stderr, "Could not get name!\n");
        perror("get_udp_packet");
        free(reply_addr);
        return 0;
    }

    if (strcmp(name, sock->ipaddr))
        n = 0;

    free(reply_addr);
    free(name);
    return n;
}

EXPORTED_SYM void SocketClose(SOCK *sock)
{
    if ((sock->flags & SOCK_FLAG_ACTIVE) == SOCK_FLAG_ACTIVE)
        close(sock->sockfd);

    sock->flags &= ~SOCK_FLAG_ACTIVE;

    freeaddrinfo(sock->servinfo);
    free(sock->ipaddr);
    free(sock);
}
