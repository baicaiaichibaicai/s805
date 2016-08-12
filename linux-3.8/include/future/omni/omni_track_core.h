#ifndef _OMNI_TRACK_CORE_H
#define _OMNI_TRACK_CORE_H
#include <future/omni.h>
#include <future/omni_track.h>
#include <future/omni/omni_track_l3proto.h>
#include <future/omni/omni_track_l4proto.h>
#include <future/omni/sync/omni_track_sync_session.h>
#include <future/log/logv2.h>
extern int __omni_track_confirm(struct omni_track *track);
extern void omni_track_log(struct omni_track *track, __be16 id, u_int event);


enum omni_martian_type
{
	OMNI_MARTIAN_SRC=0,
	OMNI_MARTIAN_SRC_KEEP_ERR,
	OMNI_MARTIAN_DST,
	OMNI_MARTIAN_MAX
};

void omni_martian_filter_log(struct sk_buff *skb,enum omni_martian_type e_type);

//by ung
//extern void omni_track_dump_tuple(struct omni_track_tuple *tuple);
extern int omni_track_get_tuple(const struct sk_buff *skb,
		unsigned int nhoff,
		unsigned int dataoff,
		u_int16_t l3num,
		u_int8_t protonum,
		struct omni_track_tuple *tuple,
		const struct omni_track_l3proto *l3proto,
		const struct omni_track_l4proto *l4proto);

extern int omni_track_invert_tuple(struct omni_track_tuple *inverse,
		const struct omni_track_tuple *orig,
		const struct omni_track_l3proto *l3proto,
		const struct omni_track_l4proto *l4proto);

struct omni_track_tuple_hash *
init_omni_sync_track(const struct omni_track_tuple *tuple);

static inline __be16 get_pkt_id(struct sk_buff *skb)
{
#ifdef CONFIG_OMNI
	struct omni_track *track = (struct omni_track *) skb->track;
	const struct iphdr *iph;

    if (track->tuplehash[OMNI_TRACK_DIR_ORIGINAL].tuple.src.l3num == AF_INET) {
        iph = ip_hdr(skb);
        return iph->id;
    }

#endif
    return 0;
}

static inline int omni_track_confirm(struct sk_buff *skb)
{
#ifdef CONFIG_OMNI
	struct omni_track *track = (struct omni_track *) skb->track;

	int ret = OMNI_ACCEPT;

	if (track) {
		if (!omni_track_is_confirmed(track)) {
			ret = __omni_track_confirm(track);
			if (ret == OMNI_ACCEPT)
			    omni_track_log(track, get_pkt_id(skb), LOG_FW_FILTER_SESSION_CREATE);
		}

		if (ret && omni_track_sync_decider(track))
			omni_track_sync_sender(track);
	}

	return ret;
#else
	return 0;
#endif
}

extern struct omni_track_tuple_hash *
omni_track_find(const struct omni_track_tuple *tuple);
extern int omni_track_count(void);
extern int omni_track_accumulate(void);
extern unsigned int omni_track_in(u_int16_t pf, struct sk_buff *skb,
		enum omni_track_proc mode);
extern unsigned int omni4_track_defrag(unsigned int hooknum,
		struct sk_buff *skb);

extern __sum16 omni_ip_checksum(struct sk_buff *skb,
				   unsigned int dataoff, u_int8_t protocol);

extern __sum16 omni_ip6_checksum(struct sk_buff *skb,
			     unsigned int dataoff, u_int8_t protocol);

extern int traffic_anomaly_detect(struct sk_buff *pskb, struct omni_track *ot);
extern void omni_drop_log(struct sk_buff *pskb, __be16 id, u_int event);
extern void abnormal_packet_log(struct sk_buff *skb, u_int event);
extern struct omni_track *init_omni_block_track(const struct omni_track_tuple *tuple,
    struct omni_track_l3proto *l3proto,
    struct omni_track_l4proto *l4proto);

extern int g_stateful;
extern spinlock_t omni_exp_lock;

extern int omni_track_register_cache(const char *name, int n, size_t size);
#endif
