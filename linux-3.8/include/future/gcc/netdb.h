#ifndef _NETDB_H
#define _NETDB_H
/***************************************************************************
* Copyright (c) 1990-1994, Future Systems, Inc. / Seoul, Republic of Korea *
* All Rights Reserved.							   *
*									   *
* This document contains proprietary and confidential information.  No	   *
* parts of this document or the computer program it embodies may be in	   *
* any way copied, duplicated, reproduced, translated into a different	   *
* programming language, or distributed to any person, company, or	   *
* corporation without the prior written consent of Future Systems, Inc.	   *
***************************************************************************/

/*---------------------- [Comment in details]----------------------------*\
Description:	netdb header file (netdb.h)
Version: 3.0
\*-----------------------------------------------------------------------*/

/*
 * Copyright (c) 1980, 1983, 1988 Regents of the University of California.
 * All rights reserved.
 */

/*
 * Structures returned by network data base library. All addresses are supplied
 * in host order,
 * and returned in network order (suitable for use in system calls).
 */
#include <linux/in.h>

/* Absolute file name for network data base files. */
#define _PATH_HEQUIV		"/etc/hosts.equiv"
#define _PATH_HOSTS		"/etc/hosts"
#define _PATH_NETWORKS		"/etc/networks"
#define _PATH_NSSWITCH_CONF	"/etc/nsswitch.conf"
#define _PATH_PROTOCOLS		"/etc/protocols"
#define _PATH_SERVICES		"/etc/services"

extern int h_errno;

/* Possible values left in `h_errno'. */
#define NETDB_INTERNAL	-1	/* See errno. */
#define NETDB_SUCCESS	0	/* No problem. */
#define HOST_NOT_FOUND	1	/* Authentitative Answer Host not found. */
#define TRY_AGAIN	2	/* Non-Authoritative Host not found,
				   or SERVERFAIL. */
#define NO_RECOVERY	3	/* Non recoverable errors, FOREVER, REFUSED,
				   NOTIMP. */
#define NO_DATA		4	/* Valid name, no data record of requested
				   type. */
#define NO_ADDRESS	NO_DATA	/* No address, look for MX record. */

struct	hostent
{
    char	*h_name;	/* official name of host */
    char	**h_aliases;	/* alias list */
    int		h_addrtype;	/* host address type */
    int		h_length;	/* length of address */
    char	**h_addr_list;	/* list of addresses from name server */
#define	h_addr	h_addr_list[0]	/* address, for backward compatiblity */
};

/*
 * Assumption here is that a network number
 * fits in 32 bits -- probably a poor one.
 */
struct	netent
{
    char	*n_name;	/* official name of net */
    char	**n_aliases;	/* alias list */
    int		n_addrtype;	/* net address type */
    unsigned long	n_net;		/* network # */
};

struct	servent
{
    char	*s_name;	/* official service name */
    char	**s_aliases;	/* alias list */
    int		s_port;		/* port # */
    char	*s_proto;	/* protocol to use */
};

struct	protoent
{
    char	*p_name;	/* official protocol name */
    char	**p_aliases;	/* alias list */
    int		p_proto;	/* protocol # */
};

#if 0
struct hostent	* gethostbyname();
struct hostent	* gethostbyaddr();
struct netent	* getnetbyname();
struct servent	* getservbyname();
struct servent	* getservbyport();
struct protoent	* getprotobyname();
struct protoent	* getprotobynumber();
#endif


extern struct hostent *gethostbyname(char *name);
extern struct hostent *gethostbyaddr(u_char *addr, int len, int type);
extern struct servent *getservbyport(int port, const char *proto);
extern struct servent *getservbyname(const char *name, const char *proto);
extern struct protoent *getprotobyname(const char *name);
extern struct protoent *getprotobynumber(int proto);


/*
 * Error return codes from gethostbyname() and gethostbyaddr()
 * (left in extern int h_errno).
 */

#define	HOST_NOT_FOUND	1 /* Authoritative Answer Host not found */
#define	TRY_AGAIN	2 /* Non-Authoritive Host not found, or SERVERFAIL */
#define	NO_RECOVERY	3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define	NO_DATA		4 /* Valid name, no data record of requested type */
#define	NO_ADDRESS	NO_DATA		/* no address, look for MX record */

#endif
