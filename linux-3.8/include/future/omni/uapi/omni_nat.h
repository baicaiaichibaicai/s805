#ifndef _UAPI_OMNI_NAT_H
#define _UAPI_OMNI_NAT_H

//#include <linux/netfilter.h>
#include <future/omni/omni_track_tuple_common.h>

#define OMNI_NAT_RANGE_MAP_IPS			1
#define OMNI_NAT_RANGE_PROTO_SPECIFIED	2
#define OMNI_NAT_RANGE_PROTO_RANDOM		4
#define OMNI_NAT_RANGE_PERSISTENT		8

struct omni_nat_ipv4_range {
	unsigned int				flags;
	__be32						min_ip;
	__be32						max_ip;
	union omni_track_man_proto	min;
	union omni_track_man_proto	max;
};

struct omni_nat_ipv4_multi_range_compat {
	unsigned int				rangesize;
	__be32 dummy;
	__be32 base_ip;
	struct omni_nat_ipv4_range	range[1];
};

struct omni_nat_range {
	unsigned int				flags;
	union omni_track_address	min_addr;
	union omni_track_address	max_addr;
	union omni_track_address	base_addr;
	union omni_track_man_proto	min_proto;
	union omni_track_man_proto	max_proto;
};

#endif /* _UAPI_OMNI_NAT_H */

