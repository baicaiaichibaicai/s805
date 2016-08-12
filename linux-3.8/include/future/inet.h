#ifndef _INET_H
#define _INET_H

#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/net.h>
#include <asm/string.h>
#include <linux/in.h>
#include <linux/socket.h>

#define MAX_IN6_ADDR_SIZE               sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 16
#endif /* INET_ADDRSTRLEN */

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif /* INET6_ADDRSTRLEN */

extern char *inet_ntoa(struct in_addr in);
extern int inet_addr(const char *cp);
extern int inet_aton(char *cp, struct in_addr *inp);
//extern char *inet_ntop6(u_char *src, char *dst, size_t size);
extern char *inet_ntop(int af, void *src, char *dst, size_t size);
extern int inet_pton(int af, char *src, void *dst);
extern void v6_mask_to_addr(unsigned char *addr, int mask);
extern int v6_addr_to_mask(unsigned char *addr);

#endif
