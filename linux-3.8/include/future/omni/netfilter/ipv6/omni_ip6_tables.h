/*
 * 25-Jul-1998 Major changes to allow for ip chain table
 *
 * 3-Jan-2000 Named tables to allow packet selection for different uses.
 */

/*
 * 	Format of an IP6 firewall descriptor
 *
 * 	src, dst, src_mask, dst_mask are always stored in network byte order.
 * 	flags are stored in host byte order (of course).
 * 	Port numbers are stored in HOST byte order.
 */
#ifndef _OMNI_IP6_TABLES_H
#define _OMNI_IP6_TABLES_H

#include <linux/if.h>
#include <linux/in6.h>
#include <linux/ipv6.h>
#include <linux/skbuff.h>

#include <linux/init.h>
#include <future/omni/filter/omni_ip6_tables.h>
#include <future/omni/netfilter/omni_x_tables.h>

extern void omni_ip6t_init(void) __init;

extern void *omni_ip6t_alloc_initial_table(const struct omni_xt_table *);
extern struct omni_xt_table *omni_ip6t_register_table(struct net *net,
					    const struct omni_xt_table *table,
					    const struct omni_ip6t_replace *repl);
extern void omni_ip6t_unregister_table(struct net *net, struct omni_xt_table *table);
extern unsigned int omni6_filter_do_table(struct sk_buff *skb,
				  unsigned int hook,
				  const struct net_device *in,
				  const struct net_device *out);

extern unsigned int omni6_nat_do_table(struct sk_buff *skb,
				  unsigned int hook,
				  const struct net_device *in,
				  const struct net_device *out);

#ifdef CONFIG_OMNI_NAT64
extern unsigned int omni6_nat64_do_table(struct sk_buff *skb,
				  unsigned int hook,
				  const struct net_device *in,
				  const struct net_device *out);
#endif

extern unsigned int omni6_filter_tuple_get(struct sk_buff *skb, struct omni_track_tuple *t);
extern void omni6_xt_table_base_save(const struct omni_xt_table *info);

/* Check for an extension */
static inline int
omni_ip6t_ext_hdr(u8 nexthdr)
{	return (nexthdr == IPPROTO_HOPOPTS) ||
	       (nexthdr == IPPROTO_ROUTING) ||
	       (nexthdr == IPPROTO_FRAGMENT) ||
	       (nexthdr == IPPROTO_ESP) ||
	       (nexthdr == IPPROTO_AH) ||
	       (nexthdr == IPPROTO_NONE) ||
	       (nexthdr == IPPROTO_DSTOPTS);
}

#ifdef CONFIG_COMPAT
#include <net/compat.h>

struct omni_compat_ip6t_entry {
	struct omni_ip6t_ip6 ipv6;
	compat_uint_t nfcache;
	__u16 target_offset;
	__u16 next_offset;
	compat_uint_t comefrom;
	struct omni_compat_xt_counters counters;
	unsigned char elems[0];
};

static inline struct omni_xt_entry_target *
omni_compat_ip6t_get_target(struct omni_compat_ip6t_entry *e)
{
	return (void *)e + e->target_offset;
}
#endif /*CONFIG_COMPAT*/
#endif /*_OMNI_IP6_TABLES_H*/
