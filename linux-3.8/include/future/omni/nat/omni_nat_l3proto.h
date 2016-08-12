#ifndef _OMNI_NAT_L3PROTO_H
#define _OMNI_NAT_L3PROTO_H

struct omni_nat_l4proto;
struct omni_nat_l3proto {
	u8	l3proto;

	bool	(*in_range)(const struct omni_track_tuple *t,
			    const struct omni_nat_range *range);

	u32 	(*secure_port)(const struct omni_track_tuple *t, __be16);

	bool	(*manip_pkt)(struct sk_buff *skb,
			     unsigned int iphdroff,
			     const struct omni_nat_l4proto *l4proto,
			     const struct omni_track_tuple *target,
			     enum omni_nat_manip_type maniptype);

	void	(*csum_update)(struct sk_buff *skb, unsigned int iphdroff,
			       __sum16 *check,
			       const struct omni_track_tuple *t,
			       enum omni_nat_manip_type maniptype);

	void	(*csum_recalc)(struct sk_buff *skb, u8 proto,
			       void *data, __sum16 *check,
			       int datalen, int oldlen);

	void	(*decode_session)(struct sk_buff *skb,
				  const struct omni_track *track,
				  enum omni_track_dir dir,
				  unsigned long statusbit,
				  struct flowi *fl);

	int	(*nlattr_to_range)(struct nlattr *tb[],
				   struct omni_nat_range *range);
};

extern int omni_nat_l3proto_register(const struct omni_nat_l3proto *);
extern void omni_nat_l3proto_unregister(const struct omni_nat_l3proto *);
extern const struct omni_nat_l3proto *__omni_nat_l3proto_find(u8 l3proto);

extern int omni_nat_icmp_reply_translation(struct sk_buff *skb,
					 struct omni_track *track,
					 enum omni_track_info trinfo,
					 unsigned int hooknum);
extern int omni_nat_icmpv6_reply_translation(struct sk_buff *skb,
					   struct omni_track *track,
					   enum omni_track_info trinfo,
					   unsigned int hooknum,
					   unsigned int hdrlen);

#endif /*_OMNI_NAT_L3PROTO_H*/
