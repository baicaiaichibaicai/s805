#ifndef _OMNI_NAT_CORE_H
#define _OMNI_NAT_CORE_H
#include <linux/list.h>
#include <future/omni_track.h>
#include <future/omni/nat/omni_nat.h>

/* This header used to share core functionality between the standalone
   NAT module, and the compatibility layer's use of NAT for masquerading. */

extern unsigned int omni_nat_packet(struct omni_track *track,
				  enum omni_track_info trinfo,
				  unsigned int hooknum,
				  struct sk_buff *skb);

extern int omni_xfrm_me_harder(struct sk_buff *skb, unsigned int family);

static inline int omni_nat_initialized(struct omni_track *track,
				     enum omni_nat_manip_type manip)
{
	if (manip == OMNI_NAT_MANIP_SRC)
		return track->status & OTS_SRC_NAT_DONE;
	else
		return track->status & OTS_DST_NAT_DONE;
}

extern int omni_nat_clear_info(struct omni_track *track);

struct nlattr;

extern int
(*omni_netlink_parse_nat_setup_hook)(struct omni_track *track,
				  enum omni_nat_manip_type manip,
				  const struct nlattr *attr);

#endif /*_OMNI_NAT_CORE_H*/
