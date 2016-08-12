#ifndef _OMNI_TRACK_SQLNET_H
#define _OMNI_TRACK_SQLNET_H

#include <future/omni/omni_track_ftp.h>

/* SQLNET tracking. */

#ifdef __KERNEL__

#define SQLNET_PORT	1521

#endif /* __KERNEL__ */

struct trans_nt_sb
{
    uint16_t 	totlen;
    uint16_t 	checksum;
    uint8_t 	type;
    uint8_t 	reserved;
    uint16_t	hdcheck;
    uint16_t	datalen;
};

struct omni_track_expect;

/* For NAT to hook in when we find a packet which describes what other
 * connection we should expect. */
extern unsigned int (*omni_nat_sqlnet_hook)(struct sk_buff *skb,
				       enum omni_track_info trinfo,
				       enum omni_track_ftp_type type,
				       unsigned int matchoff,
				       unsigned int matchlen,
				       struct omni_track_expect *exp,
				       u32 *seq);
#endif /* _OMNI_TRACK_SQLNET_H */
