#if !defined(_SOCK_UTIL_H)
#define _SOCK_UTIL_H

SOCK *get_sock_client(const char *, const char *, int);
char *get_udp_packet_ip(SOCK *, void *);

#endif
