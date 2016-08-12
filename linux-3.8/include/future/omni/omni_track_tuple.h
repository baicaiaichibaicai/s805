#ifndef _OMNI_TRACK_TUPLE_H
#define _OMNI_TRACK_TUPLE_H

#include <future/omni/omni_track_tuple_common.h>
#include <linux/llist.h>
#include <linux/in.h>
#include <linux/in6.h>

#define OMNI_TRACK_TUPLE_L3SIZE		4

/* The l3 protocol-specific manipulable parts of the tuple: always in
   network order! */
union omni_track_address {
	u_int32_t all[OMNI_TRACK_TUPLE_L3SIZE];
	__be32 ip;
	__be32 ip6[4];

	struct in_addr	in;
	struct in6_addr	in6;
};

/* The protocol-specific manipulable parts of the tuple: always in
   network order! */
union omni_track_man_proto
{
	/* Add other protocols here. */
	u_int16_t all;

	struct {
		__be16 port;
	} tcp;
	struct {
		__be16 port;
	} udp;
	struct {
		__be16 id;
	} icmp;
	struct {
		__be16 port;
	} sctp;
	struct {
		__be16 key;		/* GRE key is 32bit, PPtP only uses 16bit */
	} gre;
};

/* The manipulable part of the tuple. */
struct omni_track_man
{
	union omni_track_address u3;
	union omni_track_man_proto u;
	/* layer 3 protocol */
	u_int16_t l3num;		/* {AF_INET | AF_INET6} */
};

struct omni_track_tuple
{
	struct omni_track_man src;

	/* These are the parts of the tuple wich are fixed. */
	struct {
		union omni_track_address u3;
		union {
			/* Add other protocols here. */
			u_int16_t all;

			struct {
				__be16 port;
			} tcp;
			struct {
				__be16 port;
			} udp;
			struct {
				u_int8_t type, code;
			} icmp;
			struct {
				__be16 port;
			} sctp;
			struct {
				__be16 key;
			} gre;
		} u;

		/* The protocol. */
		u_int8_t protonum;

		/* The direction (for tuplehash) */
		u_int8_t dir;
	} dst;
};

/* This is optimized opposed to a memset of the whole structure. Everything we
 * really care about is the source/destination unions */
#define OMNI_TRACK_TUPLE_U_BLANK(tuple)							\
	do {														\
		(tuple)->src.u.all = 0;									\
		(tuple)->dst.u.all = 0;									\
		memset(&(tuple)->src.u3, 0, sizeof((tuple)->src.u3));	\
		memset(&(tuple)->dst.u3, 0, sizeof((tuple)->dst.u3));	\
	} while (0)

/* If we're the first tuple, it's the original dir. */
#define OMNI_TRACK_DIRECTION(h)									\
	((enum omni_track_dir)(h)->tuple.dst.dir)

/* Connections have a entry in the hash table: forward way only */
struct omni_track_tuple_hash
{
	struct llist_node list;

	struct omni_track_tuple tuple;

	u_int8_t disable;
};

static inline int omni_track_tuple_src_equal(const struct omni_track_tuple *t1,
		const struct omni_track_tuple *t2)
{
	return (t1->src.u3.all[0] == t2->src.u3.all[0] &&
			t1->src.u3.all[1] == t2->src.u3.all[1] &&
			t1->src.u3.all[2] == t2->src.u3.all[2] &&
			t1->src.u3.all[3] == t2->src.u3.all[3] &&
			t1->src.u.all == t2->src.u.all &&
			t1->src.l3num == t2->src.l3num &&
			t1->dst.protonum == t2->dst.protonum);
}

static inline int omni_track_tuple_dst_equal(const struct omni_track_tuple *t1,
		const struct omni_track_tuple *t2)
{
	return (t1->dst.u3.all[0] == t2->dst.u3.all[0] &&
			t1->dst.u3.all[1] == t2->dst.u3.all[1] &&
			t1->dst.u3.all[2] == t2->dst.u3.all[2] &&
			t1->dst.u3.all[3] == t2->dst.u3.all[3] &&
			t1->dst.u.all == t2->dst.u.all &&
			t1->src.l3num == t2->src.l3num &&
			t1->dst.protonum == t2->dst.protonum);
}

static inline int omni_track_tuple_equal(const struct omni_track_tuple *t1,
		const struct omni_track_tuple *t2)
{
	return omni_track_tuple_src_equal(t1, t2) && 
		   omni_track_tuple_dst_equal(t1, t2);
}

static inline int omni_track_tuple_mask_cmp(const struct omni_track_tuple *tuple_1,
						      				const struct omni_track_tuple *tuple_2,
							  				const struct omni_track_tuple *mask )
{
	int count = 0;

	for (count = 0; count < OMNI_TRACK_TUPLE_L3SIZE; count++){
		if ((tuple_1->src.u3.all[count] ^ tuple_2->src.u3.all[count]) &
			 mask->src.u3.all[count])
			return 0;
	}
			    
	for (count = 0; count < OMNI_TRACK_TUPLE_L3SIZE; count++){
		if ((tuple_1->dst.u3.all[count] ^ tuple_2->dst.u3.all[count]) &
			 mask->dst.u3.all[count])
			return 0;
	}
				    
	if ((tuple_1->src.u.all ^ tuple_2->src.u.all) & mask->src.u.all ||
	    (tuple_1->dst.u.all ^ tuple_2->dst.u.all) & mask->dst.u.all ||
	    (tuple_1->src.l3num ^ tuple_2->src.l3num) & mask->src.l3num ||
	    (tuple_1->dst.protonum ^ tuple_2->dst.protonum) & mask->dst.protonum)
		return 0;
					    
	return 1;
}

void omni_track_dump_tuple_ip(const struct omni_track_tuple *);
void omni_track_dump_tuple_ipv6(const struct omni_track_tuple *);

static inline void omni_track_dump_tuple(const struct omni_track_tuple *t)
{
	switch (t->src.l3num) {
	case AF_INET:
		omni_track_dump_tuple_ip(t);
		break;
	case AF_INET6:
		omni_track_dump_tuple_ipv6(t);
		break;
	}
}

#endif
