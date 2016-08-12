#ifndef _ARPA_INET_H
#define _ARPA_INET_H

#include <linux/in.h>

/* Convert Internet host address from numbers-and-dots notation in CP
   into binary data in network byte order.  */
extern int inet_addr (__const char *__cp);

/* Convert Internet host address from numbers-and-dots notation in CP
   into binary data and store the result in the structure INP.  */
extern int inet_aton (__const char *__cp, struct in_addr *__inp);

/* Convert Internet number in IN to ASCII representation.  The return value
   is a pointer to an internal array containing the string.  */
extern char *inet_ntoa (struct in_addr __in);

#endif
