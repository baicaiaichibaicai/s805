#ifndef _OMNI_IPTABLES_MANGLE_H
#define _OMNI_IPTABLES_MANGLE_H

extern /*static*/ unsigned int
omni_iptable_mangle_hook(unsigned int hook,
		     struct sk_buff *skb,
		     const struct net_device *in,
		     const struct net_device *out,
		     int (*okfn)(struct sk_buff *));

#endif
