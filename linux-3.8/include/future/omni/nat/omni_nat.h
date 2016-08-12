#ifndef _OMNI_NAT_H
#define _OMNI_NAT_H
#include <linux/list.h>
#include <linux/netdevice.h>

#include <future/omni_track_common.h>
#include <future/omni/omni_track_extend.h>
#include <future/omni/omni_track_compat.h>
#include <future/omni/uapi/omni_nat.h>

#include <uapi/linux/netfilter/nfnetlink_conntrack.h>
#include <uapi/linux/netfilter/nf_conntrack_tuple_common.h>
#include <linux/netfilter/nf_conntrack_pptp.h>

enum omni_nat_manip_type {
	OMNI_NAT_MANIP_SRC,
	OMNI_NAT_MANIP_DST
};

/* SRC manip occurs POST_ROUTING or LOCAL_IN */
#define OMNI_HOOK2MANIP(hooknum) ((hooknum) != OMNI_INET_POST_ROUTING && \
			     (hooknum) != OMNI_INET_LOCAL_IN)

/* NAT sequence number modifications */
struct omni_nat_seq {
	/* position of the last TCP sequence number modification (if any) */
	u_int32_t correction_pos;

	/* sequence number offset before and after last modification */
	int16_t offset_before, offset_after;
};

/* per conntrack: nat application helper private data */
union omni_conntrack_nat_help {
	/* insert nat helper private data here */
	struct omni_nat_pptp nat_pptp_info;
};

struct omni_track;

/* The structure embedded in the conntrack structure. */
struct omni_track_nat {
	struct hlist_node bysource;
	struct omni_nat_seq seq[OMNI_TRACK_DIR_MAX];
	struct omni_track *track;
	union omni_conntrack_nat_help help;

#if IS_ENABLED(CONFIG_IP_OMNI_TARGET_MASQUERADE) || \
	IS_ENABLED(CONFIG_IP6_OMNI_TARGET_MASQUERADE)
	int masq_index;
#endif/*CONFIG_IP_OMNI_TARGET_MASQUERADE || CONFIG_IP6_OMNI_TARGET_MASQUERADE*/
};

/* Set up the info structure to map into this range. */
extern unsigned int omni_nat_setup_info(struct omni_track *track,
				      const struct omni_nat_range *range,
				      enum omni_nat_manip_type maniptype);

#ifdef CONFIG_OMNI_NAT64
/* Set up the info structure to map into this range. */
extern unsigned int omni_nat64_setup_info(struct omni_track *track,
				      const struct omni_nat_range *range,
				      enum omni_nat_manip_type maniptype);
#endif

/* Is this tuple already taken? (not by us)*/
extern int omni_nat_used_tuple(const struct omni_track_tuple *tuple,
			     const struct omni_track *ignored_conntrack);

static inline struct omni_track_nat *omni_track_nat(const struct omni_track *track)
{
#if defined(CONFIG_OMNI_NAT) || defined(CONFIG_OMNI_NAT_MODULE)
	return omni_track_ext_find(track, OMNI_TRACK_EXT_NAT);
#else
	return NULL;
#endif/*CONFIG_OMNI_NAT*/
}

static inline bool omni_nat_oif_changed(unsigned int hooknum,
				      enum omni_track_info trinfo,
				      struct omni_track_nat *nat,
				      const struct net_device *out)
{
#if IS_ENABLED(CONFIG_IP_OMNI_TARGET_MASQUERADE) || \
    IS_ENABLED(CONFIG_IP6_OMNI_TARGET_MASQUERADE)
	return nat->masq_index && hooknum == OMNI_INET_POST_ROUTING &&
	       TRINFO2DIR(trinfo) == OMNI_TRACK_DIR_ORIGINAL &&		
	       nat->masq_index != out->ifindex;
#else
	return false;
#endif
}

#endif /*_OMNI_NAT_H*/
