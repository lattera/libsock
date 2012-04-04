#if !defined(_SOCK_UTIL_H)
#define _SOCK_UTIL_H

void *xmalloc(size_t);
void xfree(void *, size_t);
SOCK *get_sock_client(const char *, const char *, int);
char *get_udp_packet_ip(SOCK *, void *);

#endif
