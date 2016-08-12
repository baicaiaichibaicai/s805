/*
 * 	Format of an ARP firewall descriptor
 *
 * 	src, tgt, src_mask, tgt_mask, arpop, arpop_mask are always stored in
 *	network byte order.
 * 	flags are stored in host byte order (of course).
 */
#ifndef _MACFILTER_H
#define _MACFILTER_H

#include <linux/if.h>
#include <linux/in.h>
#include <linux/if_arp.h>
#include <linux/skbuff.h>
#include <uapi/linux/netfilter_macf/omni_macf_tables.h>

/* Standard entry. */
struct macf_standard {
	struct macf_entry entry;
	struct omni_xt_standard_target target;
};

struct macf_error {
	struct macf_entry entry;
	struct omni_xt_error_target target;
};

#define MACF_ENTRY_INIT(__size)						       \
{									       \
	.target_offset	= sizeof(struct macf_entry),			       \
	.next_offset	= (__size),					       \
}

#define MACF_STANDARD_INIT(__verdict)					       \
{									       \
	.entry		= MACF_ENTRY_INIT(sizeof(struct macf_standard)),       \
	.target		= OMNI_XT_TARGET_INIT(OMNI_XT_STANDARD_TARGET,		       \
					 sizeof(struct omni_xt_standard_target)), \
	.target.verdict	= -(__verdict) - 1,				       \
}

#define MACF_ERROR_INIT							       \
{									       \
	.entry		= MACF_ENTRY_INIT(sizeof(struct macf_error)),	       \
	.target		= OMNI_XT_TARGET_INIT(OMNI_XT_ERROR_TARGET,		       \
					 sizeof(struct omni_xt_error_target)),      \
	.target.errorname = "ERROR",					       \
}

extern void *macf_alloc_initial_table(const struct omni_xt_table *);
extern struct omni_xt_table *macf_register_table(struct net *net,
					    const struct omni_xt_table *table,
					    const struct macf_replace *repl);
extern void macf_unregister_table(struct omni_xt_table *table);
extern unsigned int macf_do_table(struct sk_buff *skb,
				  unsigned int hook,
				  const struct net_device *in,
				  const struct net_device *out,
				  struct omni_xt_table *table);

#ifdef CONFIG_COMPAT
#include <net/compat.h>

struct compat_macf_entry {
	struct macf_arp arp;
	__u16 target_offset;
	__u16 next_offset;
	compat_uint_t comefrom;
	struct omni_compat_xt_counters counters;
	unsigned char elems[0];
};

static inline struct omni_xt_entry_target *
compat_macf_get_target(struct compat_macf_entry *e)
{
	return (void *)e + e->target_offset;
}

#endif /* CONFIG_COMPAT */
#endif /* _MACFILTER_H */
