#ifndef _OMNI_TRACK_COMMON_H
#define _OMNI_TRACK_COMMON_H
#include <linux/skbuff.h>

enum omni_track_proc 
{
	/* slow-path processing */
	OMNI_TRACK_NORMAL,

	/* fast-path processing (OMNI-FAST) */
	OMNI_TRACK_FAST,
};

enum omni_track_info
{
	/* Parts of an established connection (either direction). */
	OMNI_TRACK_ESTABLISHED,

	/* Like NEW, but related to an existing connection, or ICMP error
	   (in either direction). */
	OMNI_TRACK_RELATED,

	/* Started a new connection to track (only
	   OMNI_TR_DIR_ORIGINAL); may be a retransmission. */
	OMNI_TRACK_NEW,

	/* >= this indicates reply direction */
	OMNI_TRACK_IS_REPLY,

	OMNI_TRACK_ESTABLISHED_REPLY = OMNI_TRACK_ESTABLISHED + OMNI_TRACK_IS_REPLY,
	OMNI_TRACK_RELATED_REPLY = OMNI_TRACK_RELATED + OMNI_TRACK_IS_REPLY,
	OMNI_TRACK_NEW_REPLY = OMNI_TRACK_NEW + OMNI_TRACK_IS_REPLY,	

	/* Number of distinct OMNI_TRACK types (no NEW in reply dirn). */
	OMNI_TRACK_NUMBERS = OMNI_TRACK_IS_REPLY * 2 - 1
};

/* Bitset representing status of connection. */
enum omni_track_status {
	/* It's an expected connection: bit 0 set. This bit never changed. */
	OTS_EXPECTED_BIT = 0,
	OTS_EXPECTED = (1 << OTS_EXPECTED_BIT),

	/* We've seen packets both ways: bit 1 set. Can be set, not unset. */
	OTS_SEEN_REPLY_BIT = 1,
	OTS_SEEN_REPLY = (1 << OTS_SEEN_REPLY_BIT),

	/* Omnitrack should never be early-expired. */
	OTS_ASSURED_BIT = 2,
	OTS_ASSURED = (1 << OTS_ASSURED_BIT),

	/* Connection is confirmed: originating packet has left box */
	OTS_CONFIRMED_BIT = 3,
	OTS_CONFIRMED = (1 << OTS_CONFIRMED_BIT),

	/* Connection needs src nat in orig dir.  This bit never changed. */
	OTS_SRC_NAT_BIT = 4,
	OTS_SRC_NAT = (1 << OTS_SRC_NAT_BIT),

	/* Connection needs dst nat in orig dir.  This bit never changed. */
	OTS_DST_NAT_BIT = 5,
	OTS_DST_NAT = (1 << OTS_DST_NAT_BIT),

	/* Both together. */
	OTS_NAT_MASK = (OTS_DST_NAT | OTS_SRC_NAT),

	/* Connection needs TCP sequence adjusted. */
	OTS_SEQ_ADJUST_BIT = 6,
	OTS_SEQ_ADJUST = (1 << OTS_SEQ_ADJUST_BIT),

	/* NAT initialization bits. */
	OTS_SRC_NAT_DONE_BIT = 7,
	OTS_SRC_NAT_DONE = (1 << OTS_SRC_NAT_DONE_BIT),

	OTS_DST_NAT_DONE_BIT = 8,
	OTS_DST_NAT_DONE = (1 << OTS_DST_NAT_DONE_BIT),

	/* Both together */
	OTS_NAT_DONE_MASK = (OTS_DST_NAT_DONE | OTS_SRC_NAT_DONE),

	/* Connection is dying (removed from lists), can not be unset. */
	OTS_DYING_BIT = 9,
	OTS_DYING = (1 << OTS_DYING_BIT),

	/* Connection has fixed timeout. */
	OTS_FIXED_TIMEOUT_BIT = 10,
	OTS_FIXED_TIMEOUT = (1 << OTS_FIXED_TIMEOUT_BIT),

	/* Omnitrack is a fake untracked entry */
	OTS_UNTRACKED_BIT = 12,
	OTS_UNTRACKED = (1 << OTS_UNTRACKED_BIT),

    OTS_TRACK_SYNC_BIT = 13,
	OTS_TRACK_SYNC = (1 << OTS_TRACK_SYNC_BIT),

    OTS_TRACK_DELETE_BIT = 14,
	OTS_TRACK_DELETE = (1 << OTS_TRACK_DELETE_BIT),

    /* Transparent NAT */
	OTS_TNAT_BIT = 15,
	OTS_TNAT = (1 << OTS_TNAT_BIT),

    /* SIP Expectation */
	OTS_EXP_RTP_BIT = 16,
	OTS_EXP_RTP = (1 << OTS_EXP_RTP_BIT),

	/* Update Bit for Sync Session */
	OTS_UPDATE_BIT = 17,
	OTS_UPDATE = (1 << OTS_UPDATE_BIT),
};

enum omni_flow_status {
	OMNI_FORWARD_BIT = 0,
	OMNI_FORWARD = (1 << OMNI_FORWARD_BIT),

	OMNI_LOCAL_IN_BIT = 1,
	OMNI_LOCAL_IN = (1 << OMNI_LOCAL_IN_BIT),

	OMNI_LOCAL_OUT_BIT = 2,
	OMNI_LOCAL_OUT = (1 << OMNI_LOCAL_OUT_BIT),
};
#endif
