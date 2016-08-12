/* /include/future/command/file.h
 *
 * (C) 2014 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _COMMAND_FILE_H
#define _COMMAND_FILE_H

struct file *admin_file_open(char *);
int admin_file_close(struct file *);
int admin_file_write(struct file *, char *, int);

#endif/*_COMMAND_FILE_H*/

