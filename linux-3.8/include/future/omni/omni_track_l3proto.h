#ifndef _OMNI_TRACK_L3PROTO_H
#define _OMNI_TRACK_L3PROTO_H

#include <future/omni_track.h>
#include <uapi/linux/netlink.h>

struct omni_track_l3proto
{
	/* L3 Protocol Family number. ex) PF_INET */
	u_int16_t l3proto;

	/* Protocol name */
	const char *name;

	/*
	 * Try to fill in the third arg: nhoff is offset of l3 proto
	 * hdr.  Return true if possible.
	 */
	int (*pkt_to_tuple)(const struct sk_buff *skb, unsigned int nhoff,
			struct omni_track_tuple *tuple);

	/*
	 * Invert the per-proto part of the tuple: ie. turn xmit into reply.
	 * Some packets can't be inverted: return 0 in that case.
	 */
	int (*invert_tuple)(struct omni_track_tuple *inverse,
			const struct omni_track_tuple *orig);

	/* Print out the per-protocol part of the tuple. */
	int (*print_tuple)(struct seq_file *s,
			const struct omni_track_tuple *);

	/* Print out the private part of the omnitrack. */
	int (*print_omni_track)(struct seq_file *s, const struct omni_track *);

	/* Returns verdict for packet, or -1 for invalid. */
	int (*packet)(struct omni_track *track,
			const struct sk_buff *skb,
			enum omni_track_info trinfo);

	/*
	 * Called when a new connection for this protocol found;
	 * returns TRUE if it's OK.  If so, packet() called next.
	 */
	int (*new)(struct omni_track *track, const struct sk_buff *skb);

	/* Called when a omnitrack entry is destroyed */
	void (*destroy)(struct omni_track *track);

	int (*prepare)(struct sk_buff *skb, unsigned int *dataoff, 
			u_int8_t *protonum);

	int (*tuple_to_nlattr)(struct sk_buff *skb,
			       const struct omni_track_tuple *t);
	int (*nlattr_to_tuple)(struct nlattr *tb[],
			       struct omni_track_tuple *t);
	const struct nla_policy *nla_policy;
};

extern struct omni_track_l3proto *omni_track_l3protos[AF_MAX];

/* Protocol registration. */
extern int omni_track_l3proto_register(struct omni_track_l3proto *proto);

extern struct omni_track_l3proto *
omni_track_l3proto_find_get(u_int16_t l3proto);

/* Existing built-in protocols */
extern struct omni_track_l3proto omni_track_l3proto_ipv4;
extern struct omni_track_l3proto omni_track_l3proto_ipv6;
extern struct omni_track_l3proto omni_track_l3proto_generic;

static inline struct omni_track_l3proto *
__omni_track_l3proto_find(u_int16_t l3proto)
{
	if (unlikely(l3proto >= AF_MAX))
		return &omni_track_l3proto_generic;
	return omni_track_l3protos[l3proto];
}

/* for tproxy */
#define SO_OMNI_ORIGINAL_DST 85

#endif
