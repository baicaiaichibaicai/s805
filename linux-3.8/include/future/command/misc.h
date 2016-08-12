/* /include/future/command/misc.h
 *
 * (C) 2013 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */
		  
#ifndef _COMMAND_MISC_H
#define _COMMAND_MISC_H

#include <future/command/vty.h>

#define TS_DATA		0
#define TS_IAC		1
#define TS_WILL		2
#define TS_WONT		3
#define TS_DO		4
#define TS_DONT		5
#define TS_CR		6
#define TS_SB		7
#define TS_SE		8


char telrcv(struct vty *, unsigned char);
int putchars(struct vty *, const char *);
int putnchars(struct vty *, const char *, int);
int kbhit(struct vty *);
int getch(struct vty *);

#endif /*_COMMAND_MISC_H*/

