#ifndef _OMNI_DEFRAG_IPV6_H
#define _OMNI_DEFRAG_IPV6_H


extern void omni_defrag_ipv6_enable(void);

extern int omni_track_frag6_init(void);
extern void omni_track_frag6_cleanup(void);
extern struct sk_buff *omni_track_frag6_gather(struct sk_buff *skb, u32 user);
extern void omni_track_frag6_output(unsigned int hooknum, struct sk_buff *skb,
			       struct net_device *in,
			       struct net_device *out,
			       int (*okfn)(struct sk_buff *));

extern unsigned int omni_ipv6_defrag(unsigned int hooknum,
				struct sk_buff *skb,
				const struct net_device *in,
				const struct net_device *out,
				int (*okfn)(struct sk_buff *));

struct inet_frags_ctl;

#endif /* _OMNI_DEFRAG_IPV6_H */
