/*
 * Connection state tracking for netfilter.  This is separated from,
 * but required by, the (future) NAT layer; it can also be used by an iptables
 * extension.
 *
 * 16 Dec 2003: Yasuyuki Kozakai @USAGI <yasuyuki.kozakai@toshiba.co.jp>
 *	- generalize L3 protocol dependent part.
 *
 * Derived from include/linux/netfiter_ipv4/ip_conntrack.h
 */

#ifndef _OMNI_TRACK_COMAPT_H
#define _OMNI_TRACK_COMAPT_H

#include <linux/bitops.h>
#include <linux/compiler.h>
#include <linux/atomic.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/netdevice.h>

#include <future/omni_track_common.h>
#include <future/omni/omni_track_tuple_common.h>

#include <linux/netfilter.h>


#ifdef CONFIG_OMNI_DEBUG
#define OMNI_TRACK_ASSERT(x)		WARN_ON(!(x))
#else
#define OMNI_TRACK_ASSERT(x)
#endif/*CONFIG_OMNI_DEBUG*/


/*/include/net/netfilter/nf_conntrack.h*/
extern s16 (*omni_ct_nat_offset)(const struct omni_track *track,
		enum omni_track_dir dir, u32 seq);

/*/include/linux/netfilter.h*/
extern struct nfq_ct_nat_hook __rcu *omniq_ct_nat_hook;

/*/include/linux/netfilter.h*/
extern void (*omni_nat_decode_session_hook)(struct sk_buff *, struct flowi *);

static inline struct omni_track *
omni_track_get(const struct sk_buff *skb, enum omni_track_info *trinfo)
{
	*trinfo = skb->trackinfo;
	return (struct omni_track *)skb->track;
}

static inline int omni_track_is_untracked(const struct omni_track *track)
{
	return test_bit(OTS_UNTRACKED_BIT, &track->status);
}

bool omni_track_invert_tuplepr(struct omni_track_tuple *,
		const struct omni_track_tuple *);

void *omni_track_alloc_hashtable(unsigned int *, int);
void omni_track_free_hashtable(void *, unsigned int);

/* Packet is received from loopback */
static inline bool omni_is_loopback_packet(const struct sk_buff *skb)
{
	return skb->dev && skb->skb_iif && skb->dev->flags & IFF_LOOPBACK;
}

void omni_track_alter_reply(struct omni_track *track,
		const struct omni_track_tuple *newreply);

static inline int omni_inet_addr_cmp(const union omni_track_address *a1,
				   const union omni_track_address *a2)
{
	return a1->all[0] == a2->all[0] &&
	       a1->all[1] == a2->all[1] &&
	       a1->all[2] == a2->all[2] &&
	       a1->all[3] == a2->all[3];
}

extern void omni_track_iterate_cleanup(int (*iter)(struct omni_track *i, void *data),
		void *data);

extern int omni_track_tuple_taken(const struct omni_track_tuple *tuple,
		const struct omni_track *ignored_track);

#endif /*_OMNI_TRACK_COMAPT_H*/
