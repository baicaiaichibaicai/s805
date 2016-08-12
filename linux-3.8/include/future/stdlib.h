/* /include/future/stdlib.h
 *
 * (C) 2013 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _FUTURE_STDLIB_H
#define _FUTURE_STDLIB_H

#include <stddef.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/param.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/socket.h>
#include <linux/in.h>
#include <linux/fcntl.h>
#include <linux/rtc.h>
#include <future/time.h>
#include <linux/in6.h>
#include <linux/netdevice.h>

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define RAND_MAX 0x7fffffff

extern int errno;

#if 0
#define DEBUGP(fmt, ...)				\
	/* printk(KERN_DEBUG fmt, ##__VA_ARGS__) */ \
	printk(fmt, ##__VA_ARGS__)
#else
#define DEBUGP(fmt, ...)				\
do {									\
	if (0)								\
		printk(KERN_DEBUG fmt, ##__VA_ARGS__);	\
} while (0)
#endif

unsigned long strtoul (const char *nptr, char **endptr, int base);
long strtol(const char *nptr, char **endptr, int base);
int	atoi(const char *s);
int get_link_status(struct net_device *dev);
int get_link_speed(struct net_device *dev);
int get_link_duplex(struct net_device *dev);
long	atol(const char *s);
long long	atoll(const char *s);

char *	strerror(int errcode);

void	srand(unsigned int seed);
int	rand(void);

char *strdup(const char *str);
char *strlwr(char *str);
char *strupr(char *str);
char *strtok(char *s, const char *ct);

/*/future/libc/strftime.c*/
size_t strftime(char *s, size_t maxsize, const char *format, const struct tm *tim_p);

extern void *malloc(size_t size);
extern void free(void *p);
extern void *realloc(void *ptr, size_t size);
extern void *calloc(size_t nmemb, size_t size);
extern int inet_addr(const char *cp);
#endif /*_FUTURE_STDLIB_H*/
