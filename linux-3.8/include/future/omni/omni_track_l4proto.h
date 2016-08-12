#ifndef _OMNI_TRACK_L4PROTO_H
#define _OMNI_TRACK_L4PROTO_H

#include <future/omni_track.h>
#include <net/netlink.h>
#include <uapi/linux/netlink.h>

struct omni_track_l4proto
{
	/* L3 Protocol number. */
	u_int16_t l3proto;

	/* L4 Protocol number. */
	u_int8_t l4proto;

	/* Protocol name */
	const char *name;

	/* Try to fill in the third arg: dataoff is offset past network protocol
	   hdr.  Return true if possible. */
	int (*pkt_to_tuple)(const struct sk_buff *skb,
			unsigned int dataoff,
			struct omni_track_tuple *tuple);

	/* Invert the per-proto part of the tuple: ie. turn xmit into reply.
	 * Some packets can't be inverted: return 0 in that case.
	 */
	int (*invert_tuple)(struct omni_track_tuple *inverse,
			const struct omni_track_tuple *orig);

	/* Print out the per-protocol part of the tuple. Return like seq_* */
	int (*print_tuple)(struct seq_file *s,
			const struct omni_track_tuple *);

	/* Print out the private part of the omnitrack. */
	int (*print_track)(struct seq_file *s, const struct omni_track *);

	/* Returns verdict for packet, or -1 for invalid. */
	int (*packet)(struct omni_track *track,
			const struct sk_buff *skb,
			unsigned int dataoff,
			enum omni_track_info trinfo,
			int pf);

	/* Called when a new connection for this protocol found;
	 * returns TRUE if it's OK.  If so, packet() called next. */
	int (*new)(struct omni_track *track, const struct sk_buff *skb,
			unsigned int dataoff);

	/* Called when a conntrack entry is destroyed */
	void (*destroy)(struct omni_track *track);

	int (*error)(struct sk_buff *skb, unsigned int dataoff,
			enum omni_track_info *ctinfo,
			int pf);
    /* convert protoinfo to netink attributes */
	int (*to_nlattr)(struct sk_buff *skb, struct nlattr *nla,
			struct omni_track *track);

	int (*tuple_to_nlattr)(struct sk_buff *skb,
			       const struct omni_track_tuple *t);
	int (*nlattr_to_tuple)(struct nlattr *tb[],
			       struct omni_track_tuple *t);
	const struct nla_policy *nla_policy;
};

extern const struct nla_policy omni_track_port_nla_policy[];

/* Existing built-in protocols */
extern struct omni_track_l4proto omni_track_l4proto_tcp4;
extern struct omni_track_l4proto omni_track_l4proto_tcp6;
extern struct omni_track_l4proto omni_track_l4proto_udp4;
extern struct omni_track_l4proto omni_track_l4proto_udp6;
extern struct omni_track_l4proto omni_track_l4proto_icmp;
extern struct omni_track_l4proto omni_track_l4proto_icmpv6;
extern struct omni_track_l4proto omni_track_l4proto_generic;

#define MAX_OMNI_TRACK_PROTO 256
extern struct omni_track_l4proto **omni_track_protos[PF_MAX];

extern struct omni_track_l4proto *
__omni_track_l4proto_find(u_int16_t l3proto, u_int8_t l4proto);
extern struct omni_track_l4proto *
omni_track_l4proto_find_get(u_int16_t l3proto, u_int8_t l4proto);

/* Protocol registration. */
extern int omni_track_l4proto_register(struct omni_track_l4proto *proto);

extern int omni_track_port_tuple_to_nlattr(struct sk_buff *skb,
		const struct omni_track_tuple *tuple);
extern int omni_track_port_nlattr_to_tuple(struct nlattr *tb[],
		struct omni_track_tuple *t);


#endif
