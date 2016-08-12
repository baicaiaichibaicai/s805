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
#ifndef _OMNI_IPTABLES_H
#define _OMNI_IPTABLES_H

#include <linux/if.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/skbuff.h>

#include <linux/init.h>
#include <future/omni/filter/omni_ip_tables.h>
#include <future/omni/netfilter/omni_x_tables.h>

extern void omni_ipt_init(void) __init;

extern struct omni_xt_table *omni_ipt_register_table(const struct omni_xt_table *table,
					   const struct omni_ipt_replace *repl);
extern void omni_ipt_unregister_table(struct omni_xt_table *table);

/* Standard entry. */
struct omni_ipt_standard {
	struct omni_ipt_entry entry;
	struct omni_xt_standard_target target;
};

struct omni_ipt_error {
	struct omni_ipt_entry entry;
	struct omni_xt_error_target target;
};

#define OMNI_IPT_ENTRY_INIT(__size)						       \
{									       \
	.target_offset	= sizeof(struct omni_ipt_entry),			       \
	.next_offset	= (__size),					       \
}

#define OMNI_IPT_STANDARD_INIT(__verdict)					       \
{									       \
	.entry		= OMNI_IPT_ENTRY_INIT(sizeof(struct omni_ipt_standard)),	       \
	.target		= OMNI_XT_TARGET_INIT(OMNI_XT_STANDARD_TARGET,		       \
					 sizeof(struct omni_xt_standard_target)),   \
	.target.verdict	= -(__verdict) - 1,				       \
}

#define OMNI_IPT_ERROR_INIT							       \
{									       \
	.entry		= OMNI_IPT_ENTRY_INIT(sizeof(struct omni_ipt_error)),	       \
	.target		= OMNI_XT_TARGET_INIT(OMNI_XT_ERROR_TARGET,		       \
					 sizeof(struct omni_xt_error_target)),      \
	.target.errorname = "ERROR",					       \
}

extern void omni_xt_table_base_save(const struct omni_xt_table *info);
extern void *omni_ipt_alloc_initial_table(const struct omni_xt_table *);

extern unsigned int omni_ipt_do_table(struct sk_buff *skb,
				 unsigned int hook,
				 const struct net_device *in,
				 const struct net_device *out,
				 struct omni_xt_table *table);

extern unsigned int omni_filter_do_table(struct sk_buff *skb,
		 unsigned int hook,
	     const struct net_device *in,
	     const struct net_device *out);

extern unsigned int omni_mangle_do_table(struct sk_buff *skb,
		 unsigned int hook,
	     const struct net_device *in,
	     const struct net_device *out);

extern unsigned int omni_nat_do_table(struct sk_buff *skb,
		 unsigned int hook,
	     const struct net_device *in,
	     const struct net_device *out);

unsigned int omni4_filter_tuple_get(struct sk_buff *skb,
        struct omni_track_tuple *t);

extern void
check_not_use_modules(void);

#ifdef CONFIG_COMPAT
#include <net/compat.h>

struct omni_compat_ipt_entry {
	struct omni_ipt_ip ip;
	compat_uint_t nfcache;
	__u16 target_offset;
	__u16 next_offset;
	compat_uint_t comefrom;
	struct omni_compat_xt_counters counters;
	unsigned char elems[0];
};

/* Helper functions */
static inline struct omni_xt_entry_target *
omni_compat_ipt_get_target(struct omni_compat_ipt_entry *e)
{
	return (void *)e + e->target_offset;
}

#endif /*CONFIG_COMPAT*/
#endif /*_OMNI_IPTABLES_H*/
