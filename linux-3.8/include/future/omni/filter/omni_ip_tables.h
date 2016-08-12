/*
 * 25-Jul-1998 Major changes to allow for ip chain table
 *
 * 3-Jan-2000 Named tables to allow packet selection for different uses.
 */

/*
 * 	Format of an IP firewall descriptor
 *
 * 	src, dst, src_mask, dst_mask are always stored in network byte order.
 * 	flags are stored in host byte order (of course).
 * 	Port numbers are stored in HOST byte order.
 */

#ifndef _UAPI_OMNI_IPTABLES_H
#define _UAPI_OMNI_IPTABLES_H

#include <linux/types.h>
#include <linux/compiler.h>

#include <linux/netfilter_ipv4.h>
#include <future/omni/filter/omni_x_tables.h>
#include <future/omni/filter/omni_filter.h>

#ifndef __KERNEL__
#define IPT_FUNCTION_MAXNAMELEN XT_FUNCTION_MAXNAMELEN
#define IPT_TABLE_MAXNAMELEN XT_TABLE_MAXNAMELEN
#define ipt_match omni_xt_match
#define ipt_target omni_xt_target
#define ipt_table omni_xt_table
#define ipt_get_revision omni_xt_get_revision
#define ipt_entry_match omni_xt_entry_match
#define ipt_entry_target omni_xt_entry_target
#define ipt_standard_target omni_xt_standard_target
#define ipt_error_target omni_xt_error_target
#define ipt_counters omni_xt_counters
#define IPT_CONTINUE OMNI_XT_CONTINUE
#define IPT_RETURN OMNI_XT_RETURN


/* This group is older than old (iptables < v1.4.0-rc1~89) */
#include <future/omni/netfilter/omni_xt_tcpudp.h>
#define ipt_udp xt_udp
#define ipt_tcp xt_tcp
#define IPT_TCP_INV_SRCPT	OMNI_XT_TCP_INV_SRCPT
#define IPT_TCP_INV_DSTPT	OMNI_XT_TCP_INV_DSTPT
#define IPT_TCP_INV_FLAGS	OMNI_XT_TCP_INV_FLAGS
#define IPT_TCP_INV_OPTION	OMNI_XT_TCP_INV_OPTION
#define IPT_TCP_INV_MASK	OMNI_XT_TCP_INV_MASK
#define IPT_UDP_INV_SRCPT	OMNI_XT_UDP_INV_SRCPT
#define IPT_UDP_INV_DSTPT	OMNI_XT_UDP_INV_DSTPT
#define IPT_UDP_INV_MASK	OMNI_XT_UDP_INV_MASK

/* The argument to IPT_SO_ADD_COUNTERS. */
#define ipt_counters_info omni_xt_counters_info
/* Standard return verdict, or do jump. */
#define IPT_STANDARD_TARGET OMNI_XT_STANDARD_TARGET
/* Error verdict. */
#define IPT_ERROR_TARGET OMNI_XT_ERROR_TARGET

/* fn returns 0 to continue iteration */
#define OMNI_IPT_MATCH_ITERATE(e, fn, args...) \
	OMNI_XT_MATCH_ITERATE(struct omni_ipt_entry, e, fn, ## args)

/* fn returns 0 to continue iteration */
#define OMNI_IPT_ENTRY_ITERATE(entries, size, fn, args...) \
	OMNI_XT_ENTRY_ITERATE(struct omni_ipt_entry, entries, size, fn, ## args)
#endif

/* Do table, local in packet for accept with no rule, no match */
#define RULE_MATCH          0
#define NO_RULE_MATCH       1

/* Yes, Virginia, you have to zero the padding. */
struct omni_ipt_ip {
	/* Source and destination IP addr */
	struct in_addr src, dst;
	/* Mask for src and dest IP addr */
	struct in_addr smsk, dmsk;
	char iniface[IFNAMSIZ], outiface[IFNAMSIZ];
	unsigned char iniface_mask[IFNAMSIZ], outiface_mask[IFNAMSIZ];

	/* Protocol, 0 = ANY */
	__u16 proto;

	/* Flags word */
	__u8 flags;
	/* Inverse flags */
	__u8 invflags;
};

/* Values for "flag" field in struct ipt_ip (general ip structure). */
#define IPT_F_FRAG		0x01	/* Set if rule is a fragment rule */
#define IPT_F_GOTO		0x02	/* Set if jump is a goto */
#define IPT_F_MASK		0x03	/* All possible flag bits mask. */

/* Values for "inv" field in struct ipt_ip. */
#define IPT_INV_VIA_IN		0x01	/* Invert the sense of IN IFACE. */
#define IPT_INV_VIA_OUT		0x02	/* Invert the sense of OUT IFACE */
#define IPT_INV_TOS		0x04	/* Invert the sense of TOS. */
#define IPT_INV_SRCIP		0x08	/* Invert the sense of SRC IP. */
#define IPT_INV_DSTIP		0x10	/* Invert the sense of DST OP. */
#define IPT_INV_FRAG		0x20	/* Invert the sense of FRAG. */
#define IPT_INV_PROTO		OMNI_XT_INV_PROTO
#define IPT_INV_MASK		0x7F	/* All possible flag bits mask. */

/* This structure defines each of the firewall rules.  Consists of 3
   parts which are 1) general IP header stuff 2) match specific
   stuff 3) the target to perform if the rule matches */

struct omni_ipt_entry {
	struct omni_ipt_ip ip;

	/* omni-filter declare */
	struct omni_filter filter;

	/* Mark with fields that we care about. */
	unsigned int nfcache;

	/* Size of ipt_entry + matches */
	__u16 target_offset;
	/* Size of ipt_entry + matches + target */
	__u16 next_offset;

	/* Back pointer */
	unsigned int comefrom;

	/* Packet and byte counters. */
	struct omni_xt_counters counters;

	/* The matches (if any), then the target. */
	unsigned char elems[0];
};

/*
 * New IP firewall options for [gs]etsockopt at the RAW IP level.
 * Unlike BSD Linux inherits IP options so you don't have to use a raw
 * socket for this. Instead we check rights in the calls.
 *
 * ATTENTION: check linux/in.h before adding new number here.
 */

#define OMNI_IPT_BASE_CTL				64 + 6

#define OMNI_IPT_SO_SET_REPLACE			(OMNI_IPT_BASE_CTL)
#define OMNI_IPT_SO_SET_ADD_COUNTERS	(OMNI_IPT_BASE_CTL + 1)
#define OMNI_IPT_SO_SET_SWITCH			(OMNI_IPT_BASE_CTL + 2)
#define OMNI_IPT_SO_SET_MAX				OMNI_IPT_SO_SET_SWITCH

#define OMNI_IPT_SO_GET_INFO			(OMNI_IPT_BASE_CTL)
#define OMNI_IPT_SO_GET_ENTRIES			(OMNI_IPT_BASE_CTL + 1)
#define OMNI_IPT_SO_GET_REVISION_MATCH	(OMNI_IPT_BASE_CTL + 2)
#define OMNI_IPT_SO_GET_REVISION_TARGET	(OMNI_IPT_BASE_CTL + 3)
#define OMNI_IPT_SO_GET_MAX				OMNI_IPT_SO_GET_REVISION_TARGET

/* ICMP matching stuff */
struct omni_ipt_icmp {
	__u8 type;				/* type to match */
	__u8 code[2];				/* range of code */
	__u8 invflags;				/* Inverse flags */
};

/* Values for "inv" field for struct ipt_icmp. */
#define IPT_ICMP_INV	0x01	/* Invert the sense of type/code test */

/* The argument to IPT_SO_GET_INFO */
struct omni_ipt_getinfo {
	/* Which table: caller fills this in. */
	char name[XT_TABLE_MAXNAMELEN];

	/* Kernel fills these in. */
	/* Which hook entry points are valid: bitmask */
	unsigned int valid_hooks;

	/* Hook entry points: one per netfilter hook. */
	unsigned int hook_entry[OMNI_INET_NUMHOOKS];

	/* Underflow points. */
	unsigned int underflow[OMNI_INET_NUMHOOKS];

	/* Number of entries */
	unsigned int num_entries;

	/* Size of entries. */
	unsigned int size;
};

/* The argument to IPT_SO_SET_REPLACE. */
struct omni_ipt_replace {
	/* Which table. */
	char name[XT_TABLE_MAXNAMELEN];

	/* Which hook entry points are valid: bitmask.  You can't
           change this. */
	unsigned int valid_hooks;

	/* Number of entries */
	unsigned int num_entries;

	/* Total size of new entries */
	unsigned int size;

	/* Hook entry points. */
	unsigned int hook_entry[OMNI_INET_NUMHOOKS];

	/* Underflow points. */
	unsigned int underflow[OMNI_INET_NUMHOOKS];

	/* Information about old entries: */
	/* Number of counters (must be equal to current number of entries). */
	unsigned int num_counters;
	/* The old entries' counters. */
	struct omni_xt_counters __user *counters;

	/* The entries (hang off end: not really an array). */
	struct omni_ipt_entry entries[0];
};

/* The argument to IPT_SO_GET_ENTRIES. */
struct omni_ipt_get_entries {
	/* Which table: user fills this in. */
	char name[XT_TABLE_MAXNAMELEN];

	/* User fills this in: total entry size. */
	unsigned int size;

	/* The entries. */
	struct omni_ipt_entry entrytable[0];
};

/* Helper functions */
static __inline__ struct omni_xt_entry_target *
omni_ipt_get_target(struct omni_ipt_entry *e)
{
	return (void *)e + e->target_offset;
}

/*
 *	Main firewall chains definitions and global var's definitions.
 */
#endif /* _UAPI_OMNI_IPTABLES_H */
