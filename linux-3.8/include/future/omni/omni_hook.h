#ifndef _OMNI_HOOK_H
#define _OMNI_HOOK_H

extern int omni4_hook(struct sk_buff *skb, struct net_device *in, struct net_device *out, 
		int (*hook)(struct sk_buff *, struct net_device *, struct net_device *, bool post));
extern int omni4_hook_post(struct sk_buff *skb, struct net_device *in, struct net_device *out, 
		int (*hook)(struct sk_buff *, struct net_device *, struct net_device *, bool post));

extern int omni4_l3_prerouting(struct sk_buff *skb, struct net_device *in, struct net_device *out, bool post);
extern int omni4_l3_local_in(struct sk_buff *skb, struct net_device *in, struct net_device *out, bool post);
extern int omni4_l3_forward(struct sk_buff *skb, struct net_device *in, struct net_device *out, bool post);
extern int omni4_l3_local_out(struct sk_buff *skb, struct net_device *in, struct net_device *out, bool post);
extern int omni4_l3_postrouting(struct sk_buff *skb, struct net_device *in, struct net_device *out, bool post);
extern int omni4_hook_forward(struct sk_buff *skb, struct net_device *in, struct net_device *out);

extern int omni6_hook(struct sk_buff *skb, struct net_device *in, struct net_device *out, 
		int (*hook)(struct sk_buff *, struct net_device *, struct net_device *));
extern int omni6_l3_prerouting(struct sk_buff *skb, struct net_device *in, struct net_device *out);
extern int omni6_l3_local_in(struct sk_buff *skb, struct net_device *in, struct net_device *out);
extern int omni6_l3_forward(struct sk_buff *skb, struct net_device *in, struct net_device *out);
extern int omni6_l3_local_out(struct sk_buff *skb, struct net_device *in, struct net_device *out);
extern int omni6_l3_postrouting(struct sk_buff *skb, struct net_device *in, struct net_device *out);
extern int omni6_hook_forward(struct sk_buff *skb, struct net_device *in, struct net_device *out);
#endif
