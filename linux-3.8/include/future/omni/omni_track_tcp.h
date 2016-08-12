#ifndef _OMNI_TRACK_TCP_H
#define _OMNI_TRACK_TCP_H
#include <linux/types.h>

enum tcp_omni_track {
	TCP_OMNI_TRACK_NONE,
	TCP_OMNI_TRACK_SYN_SENT,
	TCP_OMNI_TRACK_SYN_RECV,
	TCP_OMNI_TRACK_ESTABLISHED,
	TCP_OMNI_TRACK_FIN_WAIT,
	TCP_OMNI_TRACK_CLOSE_WAIT,
	TCP_OMNI_TRACK_LAST_ACK,
	TCP_OMNI_TRACK_TIME_WAIT,
	TCP_OMNI_TRACK_CLOSE,
	TCP_OMNI_TRACK_LISTEN,	/* obsolete */
#define TCP_OMNI_TRACK_SYN_SENT2	TCP_OMNI_TRACK_LISTEN
	TCP_OMNI_TRACK_MAX,
	TCP_OMNI_TRACK_IGNORE,
	TCP_OMNI_TRACK_RETRANS,
	TCP_OMNI_TRACK_UNACK,
	TCP_OMNI_TRACK_TIMEOUT_MAX
};

struct omni_track_tcp_state {
	u_int32_t	td_end;		/* max of seq + len */
	u_int32_t	td_maxend;	/* max of ack + max(win, 1) */
	u_int32_t	td_maxwin;	/* max(win) */
	u_int32_t	td_maxack;	/* max of ack */
	u_int8_t	td_scale;	/* window scale factor */
	u_int8_t	flags;		/* per direction options */
	u_int32_t	dir_ack;	/* last ack, for sqlnet MTS mode */
};

struct omni_track_tcp {
	struct omni_track_tcp_state seen[2];	/* connection parameters per direction */
	u_int8_t	state;		/* state of the connection (enum tcp_conntrack) */
	/* For detecting stale connections */
	u_int8_t	last_dir;	/* Direction of the last packet (enum ip_conntrack_dir) */
	u_int8_t	retrans;	/* Number of retransmitted packets */
	u_int8_t	last_index;	/* Index of the last packet */
	u_int32_t	last_seq;	/* Last sequence number seen in dir */
	u_int32_t	last_ack;	/* Last sequence number seen in opposite dir */
	u_int32_t	last_end;	/* Last seq + len */
	u_int16_t	last_win;	/* Last window advertisement seen in dir */
	/* For SYN packets while we may be out-of-sync */
	u_int8_t	last_wscale;	/* Last window scaling factor seen */
	u_int8_t	last_flags;	/* Last flags set */
	u_int8_t	append_flags; /* Append flag set */
};

/* Window scaling is advertised by the sender */
#define IP_OMNI_TCP_FLAG_WINDOW_SCALE		0x01

/* SACK is permitted by the sender */
#define IP_OMNI_TCP_FLAG_SACK_PERM		0x02

/* This sender sent FIN first */
#define IP_OMNI_TCP_FLAG_CLOSE_INIT		0x04

/* Be liberal in window checking */
#define IP_OMNI_TCP_FLAG_BE_LIBERAL		0x08

/* Has unacknowledged data */
#define IP_OMNI_TCP_FLAG_DATA_UNACKNOWLEDGED	0x10

/* The field td_maxack has been set */
#define IP_OMNI_TCP_FLAG_MAXACK_SET		0x20

struct omni_track_tcp_flags {
	__u8 flags;
	__u8 mask;
};  

#endif

