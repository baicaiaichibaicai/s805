/*
 * Zebra configuration command interface routine
 * Copyright (C) 1997, 98 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 * 
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _ZEBRA_COMMAND_H
#define _ZEBRA_COMMAND_H

#include <future/command/vector.h>
#include <future/command/vty.h>
#include <future/general.h>
#include <future/stdlib.h>

/* Host configuration variable */
struct host
{
	/* Host name of this router. */
	char *name;

	/* System wide terminal lines. */
	int lines;

	/* config file name of this host */
	char *config;

	/* Flags for services */
	int advanced;

	/* Banner configuration. */
	char *motd;
};

/* There are some command levels which called from command node. */
enum node_type 
{
	VIEW_NODE,		/* View node. Default mode of vty interface. */
	ENABLE_NODE,	/* Enable node. */
	MAX_NODE
};

/* Node which has some commands and prompt string and configuration
   function pointer . */
struct cmd_node 
{
	/* Node index. */
	enum node_type node;		

	/* Prompt character at vty interface. */
	char *prompt;			

	/* Is this node's configuration goes to vtysh ? */
	int vtysh;

	/* Node's configuration write function */
	int (*func) (struct vty *);

	/* Vector of this node's command list. */
	vector cmd_vector;	
};

/* Structure of command element. */
struct cmd_element 
{
	char *string;		/* Command specification by string. */
	int (*func) (struct cmd_element *, struct vty *, int, char **);
	char *doc;			/* Documentation of this command. */
	int daemon;         /* Daemon to which this command belong. */
	vector strvec;		/* Pointing out each description vector. */
	int cmdsize;		/* Command index count. */
	char *config;		/* Configuration string */
	vector subconfig;	/* Sub configuration string */
};

/* Command description structure. */
struct desc
{
	char *cmd;			/* Command string. */
	char *str;			/* Command's description. */
};

/* Return value of the commands. */
#define CMD_SUCCESS              0
#define CMD_WARNING              1
#define CMD_ERR_NO_MATCH         2
#define CMD_ERR_AMBIGUOUS        3
#define CMD_ERR_INCOMPLETE       4
#define CMD_ERR_EXEED_ARGC_MAX   5
#define CMD_ERR_NOTHING_TODO     6
#define CMD_COMPLETE_FULL_MATCH  7
#define CMD_COMPLETE_MATCH       8
#define CMD_COMPLETE_LIST_MATCH  9
#define CMD_SUCCESS_DAEMON      10
#define CMD_ERR_INVALID_PARAM   11

/* Argc max counts. */
#define CMD_ARGC_MAX   25

/* DEFUN for vty command interafce. Little bit hacky ;-). */
#define DEFUN(funcname, cmdname, cmdstr, helpstr) \
	int funcname (struct cmd_element *, struct vty *, int, char **); \
struct cmd_element cmdname = \
{ \
	cmdstr, \
	funcname, \
	helpstr \
}; \
int funcname \
(struct cmd_element *self, struct vty *vty, int argc, char **argv)

/* DEFUN_NOSH for commands that vtysh should ignore */
#define DEFUN_NOSH(funcname, cmdname, cmdstr, helpstr) \
	DEFUN(funcname, cmdname, cmdstr, helpstr)

/* DEFSH for vtysh. */
#define DEFSH(daemon, cmdname, cmdstr, helpstr) \
	struct cmd_element cmdname = \
{ \
	cmdstr, \
	NULL, \
	helpstr, \
	daemon \
}; \

/* DEFUN + DEFSH */
#define DEFUNSH(daemon, funcname, cmdname, cmdstr, helpstr) \
	int funcname (struct cmd_element *, struct vty *, int, char **); \
struct cmd_element cmdname = \
{ \
	cmdstr, \
	funcname, \
	helpstr, \
	daemon \
}; \
int funcname \
(struct cmd_element *self, struct vty *vty, int argc, char **argv)

/* ALIAS macro which define existing command's alias. */
#define ALIAS(funcname, cmdname, cmdstr, helpstr) \
	struct cmd_element cmdname = \
{ \
	cmdstr, \
	funcname, \
	helpstr \
};


/* Some macroes */
#define CMD_OPTION(S)		((S[0]) == '[')
#define CMD_VARIABLE(S)		(((S[0]) >= 'A' && (S[0]) <= 'Z') || ((S[0]) == '<'))
#define CMD_VARARG(S)		((S[0]) == '.')
#define CMD_RANGE(S)		((S[0] == '<'))
#define CMD_IPV4(S)			((strcmp ((S), "A.B.C.D") == 0))
#define CMD_IPV4_PREFIX(S)	((strcmp ((S), "A.B.C.D/M") == 0))
#define CMD_IPV6(S)			((strcmp ((S), "X:X::X:X") == 0))
#define CMD_IPV6_PREFIX(S)	((strcmp ((S), "X:X::X:X/MASK") == 0))

#define TEN_KILO		10240
#define TEN_MEGA		10485760
#define TEN_GIGA		10737418240
#define KILO			1024
#define MEGA			1048576
#define GIGA			1073741824
#define PRECISION		1000

#define COUNT_TEN_K		10000
#define COUNT_TEN_M		10000000
#define COUNT_TEN_G		10000000000

#define COUNT_K			1000
#define COUNT_M			1000000
#define COUNT_G			1000000000

#define COUNT_K_DECIMAL	10
#define COUNT_M_DECIMAL 10000
#define COUNT_G_DECIMAL 10000000

/*traffic bps calc*/
#define IFG				12
#define PREAMBLE		7
#define SFD				1
#define FSC				4
#define MARGIN			IFG + PREAMBLE + SFD + FSC

/* Common descriptions. */
#define SHOW_STR		"Show running system information\n"
#define IP_STR			"IP information\n"
#define IPV6_STR		"IPv6 information\n"
#define NO_STR			"Negate a command or set its defaults\n"
#define CLEAR_STR		"Reset functions\n"
#define DEBUG_STR		"Debugging functions (see also 'undebug')\n"
#define UNDEBUG_STR		"Disable debugging functions (see also 'debug')\n"
#define INTERFACE_STR	"Interface infomation\n"
#define IFNAME_STR		"Interface name(e.g. ep0)\n"
#define IP6_STR			"IPv6 Information\n"
#define SECONDS_STR		"<1-65535> Seconds\n"

/* IPv4 only machine should not accept IPv6 address for peer's IP
   address.  So we replace VTY command string like below. */
#ifdef HAVE_IPV6
#define NEIGHBOR_CMD		"neighbor (A.B.C.D|X:X::X:X) "
#define NO_NEIGHBOR_CMD		"no neighbor (A.B.C.D|X:X::X:X) "
#define NEIGHBOR_ADDR_STR	"IP address\nIPv6 address\n"
#else
#define NEIGHBOR_CMD		"neighbor A.B.C.D "
#define NO_NEIGHBOR_CMD		"no neighbor A.B.C.D "
#define NEIGHBOR_ADDR_STR	"IP address\n"
#endif /* HAVE_IPV6 */

/* Prototypes. */
void install_element(enum node_type, struct cmd_element *);
void install_element_all(struct cmd_element *);
void uninstall_element(enum node_type, struct cmd_element *);
void uninstall_element_all(struct cmd_element *);
void sort_node(void);
void align_node(void);

vector cmd_make_strvec(char *);
void cmd_free_strvec(vector);
char ** cmd_complete_command(vector vline, struct vty *vty, int *status);
char *cmd_prompt(enum node_type);
vector cmd_describe_command(vector vline, struct vty *vty, int *status);
int cmd_execute_command(vector, struct vty *, struct cmd_element **);
void cmd_init(int);
void cmd_terminate(void);


int config_exit(struct cmd_element *, struct vty *, int, char **);

#endif /* _ZEBRA_COMMAND_H */
