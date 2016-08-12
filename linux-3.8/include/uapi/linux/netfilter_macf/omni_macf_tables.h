/*
 * 	Format of an ARP firewall descriptor
 *
 * 	src, tgt, src_mask, tgt_mask, arpop, arpop_mask are always stored in
 *	network byte order.
 * 	flags are stored in host byte order (of course).
 */

#ifndef _UAPI_MACFILTER_H
#define _UAPI_MACFILTER_H

#include <linux/types.h>
#include <linux/compiler.h>
#include <linux/netfilter_macf.h>

#include <linux/netfilter/omni_x_tables.h>

#ifndef __KERNEL__
#define MACF_FUNCTION_MAXNAMELEN	XT_FUNCTION_MAXNAMELEN
#define MACF_TABLE_MAXNAMELEN		XT_TABLE_MAXNAMELEN
#define macf_entry_target			omni_xt_entry_target
#define macf_standard_target		omni_xt_standard_target
#define macf_error_target			omni_xt_error_target
#define MACF_CONTINUE				OMNI_XT_CONTINUE
#define MACF_RETURN					OMNI_XT_RETURN
#define macf_counters_info			omni_xt_counters_info
#define macf_counters				omni_xt_counters
#define MACF_STANDARD_TARGET		OMNI_XT_STANDARD_TARGET
#define MACF_ERROR_TARGET			OMNI_XT_ERROR_TARGET
#define MACF_ENTRY_ITERATE(entries, size, fn, args...) \
	OMNI_XT_ENTRY_ITERATE(struct macf_entry, entries, size, fn, ## args)
#else
#define MACF_FUNCTION_MAXNAMELEN	XT_FUNCTION_MAXNAMELEN
#define MACF_TABLE_MAXNAMELEN		XT_TABLE_MAXNAMELEN
#define macf_entry_target			omni_xt_entry_target
#define macf_standard_target		omni_xt_standard_target
#define macf_error_target			omni_xt_error_target
#define MACF_CONTINUE				OMNI_XT_CONTINUE
#define MACF_RETURN					OMNI_XT_RETURN
#define macf_counters_info			omni_xt_counters_info
#define macf_counters				omni_xt_counters
#define MACF_STANDARD_TARGET		OMNI_XT_STANDARD_TARGET
#define MACF_ERROR_TARGET			OMNI_XT_ERROR_TARGET
#define MACF_ENTRY_ITERATE(entries, size, fn, args...) \
	OMNI_XT_ENTRY_ITERATE(struct macf_entry, entries, size, fn, ## args)

#endif

#define MACF_DEV_ADDR_LEN_MAX 16

struct macf_devaddr_info {
	char addr[MACF_DEV_ADDR_LEN_MAX];
	char mask[MACF_DEV_ADDR_LEN_MAX];
};

/* Yes, Virginia, you have to zero the padding. */
struct macf_arp {
	/* Source and target IP addr */
	struct in_addr src, tgt;
	/* Mask for src and target IP addr */
	struct in_addr smsk, tmsk;

	/* Device hw address length, src+target device addresses */
	__u8 arhln, arhln_mask;
	struct macf_devaddr_info src_devaddr;
	struct macf_devaddr_info tgt_devaddr;

	/* ARP operation code. */
	__be16 arpop, arpop_mask;

	/* ARP hardware address and protocol address format. */
	__be16 arhrd, arhrd_mask;
	__be16 arpro, arpro_mask;

	/* The protocol address length is only accepted if it is 4
	 * so there is no use in offering a way to do filtering on it.
	 */

	char iniface[IFNAMSIZ], outiface[IFNAMSIZ];
	unsigned char iniface_mask[IFNAMSIZ], outiface_mask[IFNAMSIZ];

	/* Flags word */
	__u8 flags;
	/* Inverse flags */
	__u16 invflags;
};

/* Values for "flag" field in struct macf_ip (general arp structure).
 * No flags defined yet.
 */
#define MACF_F_MASK		0x00	/* All possible flag bits mask. */

/* Values for "inv" field in struct macf_arp. */
#define MACF_INV_VIA_IN		0x0001	/* Invert the sense of IN IFACE. */
#define MACF_INV_VIA_OUT	0x0002	/* Invert the sense of OUT IFACE */
#define MACF_INV_SRCIP		0x0004	/* Invert the sense of SRC IP. */
#define MACF_INV_TGTIP		0x0008	/* Invert the sense of TGT IP. */
#define MACF_INV_SRCDEVADDR	0x0010	/* Invert the sense of SRC DEV ADDR. */
#define MACF_INV_TGTDEVADDR	0x0020	/* Invert the sense of TGT DEV ADDR. */
#define MACF_INV_ARPOP		0x0040	/* Invert the sense of ARP OP. */
#define MACF_INV_ARPHRD		0x0080	/* Invert the sense of ARP HRD. */
#define MACF_INV_ARPPRO		0x0100	/* Invert the sense of ARP PRO. */
#define MACF_INV_ARPHLN		0x0200	/* Invert the sense of ARP HLN. */
#define MACF_INV_MASK		0x03FF	/* All possible flag bits mask. */

/* This structure defines each of the firewall rules.  Consists of 3
   parts which are 1) general ARP header stuff 2) match specific
   stuff 3) the target to perform if the rule matches */
struct macf_entry
{
	struct macf_arp arp;

	/* Size of macf_entry + matches */
	__u16 target_offset;
	/* Size of macf_entry + matches + target */
	__u16 next_offset;

	/* Back pointer */
	unsigned int comefrom;

	/* Packet and byte counters. */
	struct macf_counters counters;

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
#define MACF_BASE_CTL				100

#define MACF_SO_SET_REPLACE			(MACF_BASE_CTL)
#define MACF_SO_SET_ADD_COUNTERS	(MACF_BASE_CTL + 1)
#define MACF_SO_SET_MAX				MACF_SO_SET_ADD_COUNTERS

#define MACF_SO_GET_INFO			(MACF_BASE_CTL)
#define MACF_SO_GET_ENTRIES			(MACF_BASE_CTL + 1)
/* #define MACF_SO_GET_REVISION_MATCH	(APRT_BASE_CTL + 2) */
#define MACF_SO_GET_REVISION_TARGET	(MACF_BASE_CTL + 3)
#define MACF_SO_GET_MAX				(MACF_SO_GET_REVISION_TARGET)

/* The argument to MACF_SO_GET_INFO */
struct macf_getinfo {
	/* Which table: caller fills this in. */
	char name[MACF_TABLE_MAXNAMELEN];

	/* Kernel fills these in. */
	/* Which hook entry points are valid: bitmask */
	unsigned int valid_hooks;

	/* Hook entry points: one per netfilter hook. */
	unsigned int hook_entry[OMNI_MACF_NUMHOOKS];

	/* Underflow points. */
	unsigned int underflow[OMNI_MACF_NUMHOOKS];

	/* Number of entries */
	unsigned int num_entries;

	/* Size of entries. */
	unsigned int size;
};

/* The argument to MACF_SO_SET_REPLACE. */
struct macf_replace {
	/* Which table. */
	char name[MACF_TABLE_MAXNAMELEN];

	/* Which hook entry points are valid: bitmask.  You can't
           change this. */
	unsigned int valid_hooks;

	/* Number of entries */
	unsigned int num_entries;

	/* Total size of new entries */
	unsigned int size;

	/* Hook entry points. */
	unsigned int hook_entry[OMNI_MACF_NUMHOOKS];

	/* Underflow points. */
	unsigned int underflow[OMNI_MACF_NUMHOOKS];

	/* Information about old entries: */
	/* Number of counters (must be equal to current number of entries). */
	unsigned int num_counters;
	/* The old entries' counters. */
	struct macf_counters __user *counters;

	/* The entries (hang off end: not really an array). */
	struct macf_entry entries[0];
};

/* The argument to MACF_SO_GET_ENTRIES. */
struct macf_get_entries {
	/* Which table: user fills this in. */
	char name[MACF_TABLE_MAXNAMELEN];

	/* User fills this in: total entry size. */
	unsigned int size;

	/* The entries. */
	struct macf_entry entrytable[0];
};

/* Helper functions */
static __inline__ struct macf_entry_target *macf_get_target(struct macf_entry *e)
{
	return (void *)e + e->target_offset;
}

/*
 *	Main firewall chains definitions and global var's definitions.
 */
#endif /* _UAPI_MACFILTER_H */
