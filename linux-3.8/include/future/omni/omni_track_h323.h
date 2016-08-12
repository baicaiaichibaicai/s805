#ifndef	_OMNI_TRACK_H323_H
#define _OMNI_TRACK_H323_H

#ifdef __KERNEL__

#include <linux/netfilter/nf_conntrack_h323_asn1.h>


#define RAS_PORT 1719
#define Q931_PORT 1720
#define H323_RTP_CHANNEL_MAX 4	/* Audio, video, FAX and other */

/* This structure exists only once per master */
struct omni_track_h323_master {

	/* Original and NATed Q.931 or H.245 signal ports */
	u_int16_t sig_port[OMNI_TRACK_DIR_MAX];

	/* Original and NATed RTP ports */
	u_int16_t rtp_port[H323_RTP_CHANNEL_MAX][OMNI_TRACK_DIR_MAX];

	union {
		/* RAS connection timeout */
		u_int32_t timeout;

		/* Next TPKT length (for separate TPKT header and data) */
		u_int16_t tpkt_len[OMNI_TRACK_DIR_MAX];
	};
};

struct omni_track_expect;

extern int omni_get_h225_addr(unsigned char *data, TransportAddress * addr,
			 __be32 * ip, u_int16_t * port);
extern void omni_track_h245_expect(struct omni_track *new,
				     struct omni_track_expect *this);
extern void omni_track_q931_expect(struct omni_track *new,
				     struct omni_track_expect *this);
extern int (*omni_set_h245_addr_hook) (struct sk_buff * skb,
				  unsigned char *data, int dataoff,
				  H245_TransportAddress * addr,
				  __be32 ip, u_int16_t port);
extern int (*omni_set_h225_addr_hook) (struct sk_buff *skb,
				  unsigned char *data, int dataoff,
				  TransportAddress * addr,
				  __be32 ip, u_int16_t port);
extern int (*omni_set_sig_addr_hook) (struct sk_buff *skb,
				 struct omni_track *tr,
				 enum omni_track_info ctinfo,
				 unsigned char *data,
				 TransportAddress * addr, int count);
extern int (*omni_set_ras_addr_hook) (struct sk_buff *skb,
				 struct omni_track * tr,
				 enum omni_track_info trinfo,
				 unsigned char *data,
				 TransportAddress * addr, int count);
extern int (*omni_nat_rtp_rtcp_hook) (struct sk_buff *skb,
				 struct omni_track * tr,
				 enum omni_track_info trinfo,
				 unsigned char *data, int dataoff,
				 H245_TransportAddress * addr,
				 u_int16_t port, u_int16_t rtp_port,
				 struct omni_track_expect * rtp_exp,
				 struct omni_track_expect * rtcp_exp);
extern int (*omni_nat_t120_hook) (struct sk_buff *skb, struct omni_track * tr,
			     enum omni_track_info trinfo,
			     unsigned char *data, int dataoff,
			     H245_TransportAddress * addr, u_int16_t port,
			     struct omni_track_expect * exp);
extern int (*omni_nat_h245_hook) (struct sk_buff *skb, struct omni_track * tr,
			     enum omni_track_info trinfo,
			     unsigned char *data, int dataoff,
			     TransportAddress * addr, u_int16_t port,
			     struct omni_track_expect * exp);
extern int (*omni_nat_callforwarding_hook) (struct sk_buff *skb,
				       struct omni_track * tr,
				       enum omni_track_info trinfo,
				       unsigned char *data, int dataoff,
				       TransportAddress * addr,
				       u_int16_t port,
				       struct omni_track_expect * exp);
extern int (*omni_nat_q931_hook) (struct sk_buff *skb, struct omni_track * tr,
			     enum omni_track_info trinfo,
			     unsigned char *data, TransportAddress * addr,
			     int idx, u_int16_t port,
			     struct omni_track_expect * exp);

#endif

#endif
