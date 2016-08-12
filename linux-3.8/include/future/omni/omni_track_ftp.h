#ifndef _OMNI_TRACK_FTP_H
#define _OMNI_TRACK_FTP_H

#include <future/omni_track_common.h>
#include <linux/skbuff.h>

/* FTP tracking. */

/* This enum is exposed to userspace */
enum omni_track_ftp_type
{
	/* PORT command from client */
	OMNI_TR_FTP_PORT,
	/* PASV response from server */
	OMNI_TR_FTP_PASV,
	/* EPRT command from client */
	OMNI_TR_FTP_EPRT,
	/* EPSV response from server */
	OMNI_TR_FTP_EPSV,
};

#ifdef __KERNEL__

#define FTP_PORT	21

#define NUM_SEQ_TO_REMEMBER 2
/* This structure exists only once per master */
struct omni_track_ftp_master {
	/* Valid seq positions for cmd matching after newline */
	u_int32_t seq_aft_nl[OMNI_TRACK_DIR_MAX][NUM_SEQ_TO_REMEMBER];
	/* 0 means seq_match_aft_nl not set */
	int seq_aft_nl_num[OMNI_TRACK_DIR_MAX];
};

struct omni_track_expect;

/* For NAT to hook in when we find a packet which describes what other
 * connection we should expect. */
extern unsigned int (*omni_nat_ftp_hook)(struct sk_buff **pskb,
				       enum omni_track_info track_info,
				       enum omni_track_ftp_type type,
				       unsigned int matchoff,
				       unsigned int matchlen,
				       struct omni_track_expect *exp,
				       u32 *seq);

#ifdef CONFIG_NAT_PT
struct omni_track_ftp
{
    /* This tells NAT-PT that this is an ftp connection */
    int is_ftp;
    /* seq is sequence number of first character of
     *      *          * TEXT ADDRESS STRING in payload -jclee */
    u_int32_t seq;
    /* 0 means not found yet */
    /* len is length of TEXT ADDRESS STRING in payload -jclee */
    u_int32_t len;
    /* ftp command received lastly which we are interested in -jclee */
    enum omni_track_ftp_type ftptype;
    /* Port that was to be used */
    u_int16_t port;
    /* Next valid seq position for cmd matching after newline */
    u_int32_t seq_aft_nl[OMNI_TRACK_DIR_MAX];
    /* 0 means seq_match_aft_nl not set */
    int seq_aft_nl_set[OMNI_TRACK_DIR_MAX];
    /* ftp command received lastly -jclee */
    enum omni_track_ftp_type command;
};
#endif

extern unsigned int omni_masq_ftp(struct sk_buff **pskb, 
	enum omni_track_info track_info,
	enum omni_track_ftp_type type,
	unsigned int matchoff,
	unsigned int matchlen,
	struct omni_track_expect *exp,
	u32 *seq);

#endif /* __KERNEL__ */

#endif /* _OMNI_TRACK_FTP_H */
