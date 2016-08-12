/*
 * Memory management routine
 * Copyright (C) 1998 Kunihiro Ishiguro
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

#ifndef _ZEBRA_MEMORY_H
#define _ZEBRA_MEMORY_H

/* #define MEMORY_LOG */

/* For tagging memory, below is the type of the memory. */
enum
{
	MTYPE_TMP = 1,
	MTYPE_COMMAND,
	MTYPE_COMMAND_CONST,
	MTYPE_STRVEC,
	MTYPE_VECTOR,
	MTYPE_VECTOR_INDEX,
	MTYPE_LINK_LIST,
	MTYPE_LINK_NODE,
	MTYPE_VTY,
	MTYPE_VTY_HIST,
	MTYPE_BUFFER,
	MTYPE_BUFFER_DATA,
	MTYPE_ELEMENT,
	MTYPE_DESC,
	MTYPE_ADMIN,
	MTYPE_MAX
};

#define XMALLOC(mtype, size)       zmalloc ((mtype), (size))
#define XCALLOC(mtype, num, size)  zcalloc ((mtype), (num), (size))
#define XREALLOC(mtype, ptr, size) zrealloc ((mtype), (ptr), (size))
#define XFREE(mtype, ptr)          zfree ((mtype), (ptr))
#define XSTRDUP(mtype, str)        zstrdup ((mtype), (str))

/* Prototypes of memory function. */
void *zmalloc (int type, size_t size);
void *zcalloc (int type, size_t num, size_t size);
void *zrealloc (int type, void *ptr, size_t size);
void  zfree (int type, void *ptr);
char *zstrdup (int type, char *str);

#endif /* _ZEBRA_MEMORY_H */
