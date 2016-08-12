
#ifndef _OMNI_IPTABLES_NAT_H
#define _OMNI_IPTABLES_NAT_H

#include <linux/skbuff.h>
#include <future/omni/omni_track_compat.h>

#if IS_ENABLED(CONFIG_OMNI)
unsigned int omni4_nat_fn(struct sk_buff *, struct net_device *, struct net_device *);
unsigned int omni4_nat_in(struct sk_buff *, struct net_device *, struct net_device *);
unsigned int omni4_nat_out(struct sk_buff *, struct net_device *, struct net_device *);
unsigned int omni4_nat_local_fn(struct sk_buff *, struct net_device *, struct net_device *);
#else
static inline unsigned int
omni4_nat_fn(struct sk_buff *, struct net_device *, struct net_device *)
{
	return OMNI_ACCEPT;
}

static inline unsigned int
omni4_nat_in(struct sk_buff *, struct net_device *, struct net_device *)
{
	return OMNI_ACCEPT;
}

static inline unsigned int
omni4_nat_out(struct sk_buff *, struct net_device *, struct net_device *)
{
	return OMNI_ACCEPT;
}

static inline unsigned int
omni4_nat_local_fn(struct sk_buff *, struct net_device *, struct net_device *)
{
	return OMNI_ACCEPT;
}
#endif/*CONFIG_OMNI*/

#endif /*_OMNI_IPTABLES_NAT_H*/

