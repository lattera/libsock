#if !defined(_SOCK_H)
#define _SOCK_H

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

#define EXPORTED_SYM __attribute__((visibility("default")))

#define SOCK_FLAG_NONE          0x00000000
#define SOCK_FLAG_ACTIVE        0x00000001
#define SOCK_FLAG_KEVENT_ACTIVE 0x00000002

#define SOCK_TCP    SOCK_STREAM
#define SOCK_UDP    SOCK_DGRAM

typedef struct _sock {
    int sockfd;
    unsigned int flags;
    int protocol;

    /* For UDP */
    struct addrinfo *servinfo;
    struct addrinfo *addrinfo;
    char *ipaddr;
} SOCK;

SOCK *SocketClient(const char *, const char *, int);
ssize_t SocketWrite(SOCK *, const void *, size_t);
ssize_t SocketRead(SOCK *, void *, size_t);
void SocketClose(SOCK *);
ssize_t udp_read(SOCK *, void *, size_t);

#endif
