#ifndef __STRING_H__
#define __STRING_H__

#include <linux/string.h>

// FUTURE_IPS
#define bzero(s, n) memset(s, 0, n)
#define bcopy(s, d, n) memcpy(s, d, n)
#endif

extern char *strtok(char *s, const char *ct);
extern char *strdup(const char *str);
