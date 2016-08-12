/* /include/future/command/queue.h
 *
 * (C) 2013 by suyoon <songung@future.co.kr>
 *
 * Based on FOS 4.0 - (C) Future Systems
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */
#ifndef _COMMAND_QUEUE_H
#define _COMMAND_QUEUE_H

#include <future/command/vty.h>


#define MAX_QUEUE_BUF	1024
#define LIMIT_QUEUE_BUF	1022 //MAX_QUEUE_BUF - 2

#define MAX_QUEUE_SIZE	65536 //(64 * MAX_QUEUE_BUF)


int admin_queue_put(struct vty *, char *, int);
void admin_queue_get_and_sendmsg(struct vty *);

#endif /*_COMMAND_QUEUE_H*/
