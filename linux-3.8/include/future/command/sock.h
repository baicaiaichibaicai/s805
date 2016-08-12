/* /include/future/command/sock.h
 *
 * (C) 2013 by suyoon <songung@future.co.kr>
 *
 * Based on FOS 4.0 - (C) Future Systems
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _COMMAND_SOCK_H
#define _COMMAND_SOCK_H

#include <linux/net.h>


int admin_sock_sendmsg(struct socket *sock, char *buf, int buf_len);
int admin_sock_recvmsg(struct socket *sock, char *buf, int buf_len);

int admin_sock_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len);
int admin_sock_listen(struct socket *sock);
int admin_sock_accept(struct socket *sock, struct socket **newsk);
int admin_sock_connect(struct socket *sock, struct sockaddr *uaddr, int addr_len);
int admin_sock_setsockopt(struct socket *sock, int level, int optname, char *optval, int optlen);

int admin_sock_create(int family, int type, int protocol, struct socket **res);
void admin_sock_release(struct socket *sock);

int admin_sock_getsockname(struct socket *sock, struct sockaddr *uaddr, int *addr_len);
int admin_sock_getpeername(struct socket *sock, struct sockaddr *uaddr, int *addr_len);

#endif /*_COMMAND_SOCK_H*/
