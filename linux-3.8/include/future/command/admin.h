/* /include/future/command/admin.h
 *
 * (C) 2013 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _COMMAND_ADMIN_H
#define _COMMAND_ADMIN_H

#include <future/command/vty.h>

struct admin {
	struct vty *vty;
	struct task_struct *sub_task;
	struct task_struct *queue_task;
};

int admin_get_users(void);

#endif/*_COMMAND_ADMIN_H*/

