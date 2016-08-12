#ifndef _OMNI_NAT_HELPER_H
#define _OMNI_NAT_HELPER_H
/* NAT protocol helper routines. */
#include <future/omni_track.h>
#include <future/omni/omni_track_expect.h>

struct sk_buff;

/* These return true or false. */
extern int __omni_nat_mangle_tcp_packet(struct sk_buff *skb,
				      struct omni_track *track,
				      enum omni_track_info trinfo,
				      unsigned int protoff,
				      unsigned int match_offset,
				      unsigned int match_len,
				      const char *rep_buffer,
				      unsigned int rep_len, bool adjust);

static inline int omni_nat_mangle_tcp_packet(struct sk_buff *skb,
					   struct omni_track *track,
					   enum omni_track_info trinfo,
					   unsigned int protoff,
					   unsigned int match_offset,
					   unsigned int match_len,
					   const char *rep_buffer,
					   unsigned int rep_len)
{
	return __omni_nat_mangle_tcp_packet(skb, track, trinfo, protoff,
					  match_offset, match_len,
					  rep_buffer, rep_len, true);
}

extern int omni_nat_mangle_udp_packet(struct sk_buff *skb,
				    struct omni_track *track,
				    enum omni_track_info trinfo,
				    unsigned int protoff,
				    unsigned int match_offset,
				    unsigned int match_len,
				    const char *rep_buffer,
				    unsigned int rep_len);

extern void omni_nat_set_seq_adjust(struct omni_track *track,
				  enum omni_track_info trinfo,
				  __be32 seq, s16 off);
extern int omni_nat_seq_adjust(struct sk_buff *skb,
			     struct omni_track *track,
			     enum omni_track_info trinfo,
			     unsigned int protoff);
extern int (*omni_nat_seq_adjust_hook)(struct sk_buff *skb,
				     struct omni_track *track,
				     enum omni_track_info trinfo,
				     unsigned int protoff);

/* Setup NAT on this expected conntrack so it follows master, but goes
 * to port track->master->saved_proto. */
extern void omni_nat_follow_master(struct omni_track *track,
				 struct omni_track_expect *this);

extern s16 omni_nat_get_offset(const struct omni_track *track,
			     enum omni_track_dir dir,
			     u32 seq);

extern void omni_nat_tcp_seq_adjust(struct sk_buff *skb, struct omni_track *track,
				  u32 dir, int off);

#endif /*_OMNI_NAT_HELPER_H*/
