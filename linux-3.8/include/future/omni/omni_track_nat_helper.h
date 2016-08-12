#ifndef _NF_NAT_HELPER_H
#define _NF_NAT_HELPER_H
/* NAT protocol helper routines. */

#include <future/omni_track.h>

struct sk_buff;

/* These return true or false. */
extern int omni_nat_mangle_tcp_packet(struct sk_buff **skb,
				    struct omni_track *ct,
				    enum omni_track_info ctinfo,
				    unsigned int match_offset,
				    unsigned int match_len,
				    const char *rep_buffer,
				    unsigned int rep_len);
extern int nf_nat_mangle_udp_packet(struct sk_buff **skb,
				    struct omni_track *ct,
				    enum omni_track_info ctinfo,
				    unsigned int match_offset,
				    unsigned int match_len,
				    const char *rep_buffer,
				    unsigned int rep_len);
extern int nf_nat_seq_adjust(struct sk_buff **pskb,
			     struct omni_track *ct,
			     enum omni_track_info ctinfo);

extern unsigned int omni_nat_adjust(struct sk_buff *skb,
					const struct net_device *in,
					const struct net_device *out);

extern int omni_nat_seq_adjust(struct sk_buff *skb,
			     struct omni_track *tr,
			     enum omni_track_info trinfo,
				 unsigned int protoff);

/* Setup NAT on this expected conntrack so it follows master, but goes
 * to port ct->master->saved_proto. */
extern void omni_nat_follow_master(struct omni_track *ct,
				 struct omni_track_expect *this);

extern void omni_adjust_tcp_sequence(u32 seq, int sizediff, struct omni_track *ct, enum omni_track_info ctinfo);

extern void omni_track_tcp_update(struct sk_buff *skb, unsigned int dataoff, struct omni_track *track, int dir);
#endif
