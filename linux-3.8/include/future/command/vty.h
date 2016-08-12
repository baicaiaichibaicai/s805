/*
 * Virtual terminal [aka TeletYpe] interface routine
 * Copyright (C) 1997 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  
 */

#ifndef _ZEBRA_VTY_H
#define _ZEBRA_VTY_H

#include <future/time.h>
#include <future/command/capture.h>
#include <future/omni/sync/omni_track_sync_print.h>

#define BUFSIZ	1024

#define VTY_BUFSIZ (41 * BUFSIZ)
#define VTY_MAXHIST 20

#define MAX_USER_NAME_LEN 16
#define MAX_HOST_NAME_LEN 48
#define MAX_TTY_NAME_LEN 16

/* Admin queue struct. */
struct admin_queue {
	char *data;
	int head;
	int tail;
};

/* Client info struct. */
struct client_info {
	struct socket *sock;
	struct file *file;
	char *user_name;
	int group_id;
	char *host_name;
	char *tty_name;
	time_t uptime;
	int users;
};

/* Telnet struct */
struct telnet_info {
	unsigned char state;
	unsigned char option;
	unsigned char *sb_buffer;
	int sb_buffer_idx;
	unsigned char tmp_ch;
};

/* VTY struct. */
struct vty 
{
	/* File descripter of this vty. */
	int fd;

	/* Is this vty connect to file or not */
	enum {VTY_TERM} type;

	/* Node status of this vty */
	int node;

	/* Privilege level of this vty. */
	int privilege;

	/* Failure count */
	int fail;

	/* Output buffer. */
	struct buffer *obuf;

	/* Command input buffer */
	char *buf;

	/* Command cursor point */
	int cp;

	/* Command length */
	int length;

	/* Command max length. */
	int max;

	/* Histry of command */
	char *hist[VTY_MAXHIST];

	/* History lookup current point */
	int hp;

	/* History insert end point */
	int hindex;

	/* For current referencing point of interface, route-map,
	   access-list etc... */
	//void *index;

	/* For multiple level index treatment such as key chain and key. */
	void *index_sub;

	/* For escape character. */
	unsigned char escape;

	/* Current vty status. */
	enum {VTY_NORMAL, VTY_CLOSE, VTY_MORE, VTY_START, VTY_CONTINUE} status;

	/* Window width/height. */
	int width;
	int height;

	int scroll_one;

	/* Configure lines. */
	int lines;

	/* Current executing function pointer. */
	int (*func) (struct vty *, void *arg);

	/* Terminal monitor. */
	int monitor;

	/* In configure mode. */
	int config;

	/* Read and write thread. */
	struct thread *t_read;
	struct thread *t_write;

	/* Timeout seconds and thread. */
	unsigned long v_timeout;
	struct thread *t_timeout;

	/* Thread output function. */
	struct thread *t_output;

	/* Output data pointer. */
	int (*output_func) (struct vty *, int);
	void (*output_clean) (struct vty *);
	struct route_node *output_rn;
	unsigned long output_count;
	int output_type;
	void *output_arg;

	/* Terminal Print. */
	void (*print_func) (void);

	/* Telnet Info */
	struct telnet_info *telnet;

	/* Debug Level */
	unsigned int debug_level;

	/* Client Info */
	struct client_info *client;

	/* Queue */
	struct admin_queue *queue;

	/* Capture */
	struct capture_info *capture;

	/* Sync Session */
	struct sync_track *sn_tr;

	/* Index */
	int index;

	/* SpinLock */
	spinlock_t lock;
};

/* Small macro to determine newline is newline only or linefeed needed. */
#define VTY_NEWLINE  ((vty->type == VTY_TERM) ? "\r\n" : "\n")

/* Default time out value */
#define VTY_TIMEOUT_DEFAULT 600

/* Vty read buffer size. */
#define VTY_READ_BUFSIZ 512

/* Directory separator. */
#ifndef DIRECTORY_SEP
#define DIRECTORY_SEP '/'
#endif /* DIRECTORY_SEP */

#ifndef IS_DIRECTORY_SEP
#define IS_DIRECTORY_SEP(c) ((c) == DIRECTORY_SEP)
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

/* Prototypes. */
struct vty *vty_new (void);
int vty_out (struct vty *, const char *, ...);
int vty_error (struct vty *, const char *, ...);
void vty_time_print (struct vty *, int);
void vty_close (struct vty *);
int vty_config_lock (struct vty *);
int vty_config_unlock (struct vty *);
void vty_hello (struct vty *);
void vty_show_system_info(struct vty *);
void vty_will_echo (struct vty *);
struct vty * vty_create (unsigned int, struct socket *);
int vty_flush (struct vty *vty);
int vty_read (struct vty *vty, char *, int);

void vty_init2(void);
void vty_cleanup(void);

#endif /* _ZEBRA_VTY_H */

