/* Header for use in defining a given protocol. */
#ifndef _OMNI_NAT_L4PROTO_H
#define _OMNI_NAT_L4PROTO_H
#include <future/omni/nat/omni_nat.h>

struct omni_nat_range;
struct omni_nat_l3proto;

struct omni_nat_l4proto {
	/* Protocol number. */
	u8 l4proto;

	/* Translate a packet to the target according to manip type.
	 * Return true if succeeded.
	 */
	bool (*manip_pkt)(struct sk_buff *skb,
			  const struct omni_nat_l3proto *l3proto,
			  unsigned int iphdroff, unsigned int hdroff,
			  const struct omni_track_tuple *tuple,
			  enum omni_nat_manip_type maniptype);

	/* Is the manipable part of the tuple between min and max incl? */
	bool (*in_range)(const struct omni_track_tuple *tuple,
			 enum omni_nat_manip_type maniptype,
			 const union omni_track_man_proto *min,
			 const union omni_track_man_proto *max);

	/* Alter the per-proto part of the tuple (depending on
	 * maniptype), to give a unique tuple in the given range if
	 * possible.  Per-protocol part of tuple is initialized to the
	 * incoming packet.
	 */
	void (*unique_tuple)(const struct omni_nat_l3proto *l3proto,
			     struct omni_track_tuple *tuple,
			     const struct omni_nat_range *range,
			     enum omni_nat_manip_type maniptype,
			     const struct omni_track *track);

	int (*nlattr_to_range)(struct nlattr *tb[],
			       struct omni_nat_range *range);
};

/* Protocol registration. */
extern int omni_nat_l4proto_register(u8 l3proto, const struct omni_nat_l4proto *l4proto);
extern void omni_nat_l4proto_unregister(u8 l3proto, const struct omni_nat_l4proto *l4proto);

extern const struct omni_nat_l4proto *__omni_nat_l4proto_find(u8 l3proto, u8 l4proto);

/* Built-in protocols. */
extern const struct omni_nat_l4proto omni_nat_l4proto_tcp;
extern const struct omni_nat_l4proto omni_nat_l4proto_udp;
extern const struct omni_nat_l4proto omni_nat_l4proto_icmp;
extern const struct omni_nat_l4proto omni_nat_l4proto_icmpv6;
extern const struct omni_nat_l4proto omni_nat_l4proto_unknown;

extern bool omni_nat_l4proto_in_range(const struct omni_track_tuple *tuple,
				    enum omni_nat_manip_type maniptype,
				    const union omni_track_man_proto *min,
				    const union omni_track_man_proto *max);

extern void omni_nat_l4proto_unique_tuple(const struct omni_nat_l3proto *l3proto,
					struct omni_track_tuple *tuple,
					const struct omni_nat_range *range,
					enum omni_nat_manip_type maniptype,
					const struct omni_track *track,
					u16 *rover);

extern int omni_nat_l4proto_nlattr_to_range(struct nlattr *tb[],
					  struct omni_nat_range *range);

#endif /*_OMNI_NAT_L4PROTO_H*/
