/* PPTP constants and structs */
#ifndef _OMNI_TRACK_PPTP_H
#define _OMNI_TRACK_PPTP_H

#include <future/omni/omni_track_tuple_common.h>

/* state of the control session */
enum omni_pptp_ctrlsess_state {
	OMNI_PPTP_SESSION_NONE,			/* no session present */
	OMNI_PPTP_SESSION_ERROR,			/* some session error */
	OMNI_PPTP_SESSION_STOPREQ,			/* stop_sess request seen */
	OMNI_PPTP_SESSION_REQUESTED,			/* start_sess request seen */
	OMNI_PPTP_SESSION_CONFIRMED,			/* session established */
};

/* state of the call inside the control session */
enum omni_pptp_ctrlcall_state {
	OMNI_PPTP_CALL_NONE,
	OMNI_PPTP_CALL_ERROR,
	OMNI_PPTP_CALL_OUT_REQ,
	OMNI_PPTP_CALL_OUT_CONF,
	OMNI_PPTP_CALL_IN_REQ,
	OMNI_PPTP_CALL_IN_REP,
	OMNI_PPTP_CALL_IN_CONF,
	OMNI_PPTP_CALL_CLEAR_REQ,
};

/* conntrack private data */
struct omni_track_pptp_master {
	enum omni_pptp_ctrlsess_state sstate;	/* session state */
	enum omni_pptp_ctrlcall_state cstate;	/* call state */
	__be16 pac_call_id;			/* call id of PAC */
	__be16 pns_call_id;			/* call id of PNS */

	/* in pre-2.6.11 this used to be per-expect. Now it is per-conntrack
	 * and therefore imposes a fixed limit on the number of maps */
	struct omni_track_gre_keymap *keymap[OMNI_TRACK_DIR_MAX];
};

struct omni_nat_pptp {
	__be16 pns_call_id;			/* NAT'ed PNS call id */
	__be16 pac_call_id;			/* NAT'ed PAC call id */
};

#ifdef __KERNEL__

#define OMNI_PPTP_CONTROL_PORT	1723

#define PPTP_PACKET_CONTROL	1
#define PPTP_PACKET_MGMT	2

#define PPTP_MAGIC_COOKIE	0x1a2b3c4d

struct omni_pptp_pkt_hdr {
	__u16	packetLength;
	__be16	packetType;
	__be32	magicCookie;
};

/* PptpControlMessageType values */
#define PPTP_START_SESSION_REQUEST	1
#define PPTP_START_SESSION_REPLY	2
#define PPTP_STOP_SESSION_REQUEST	3
#define PPTP_STOP_SESSION_REPLY		4
#define PPTP_ECHO_REQUEST		5
#define PPTP_ECHO_REPLY			6
#define PPTP_OUT_CALL_REQUEST		7
#define PPTP_OUT_CALL_REPLY		8
#define PPTP_IN_CALL_REQUEST		9
#define PPTP_IN_CALL_REPLY		10
#define PPTP_IN_CALL_CONNECT		11
#define PPTP_CALL_CLEAR_REQUEST		12
#define PPTP_CALL_DISCONNECT_NOTIFY	13
#define PPTP_WAN_ERROR_NOTIFY		14
#define PPTP_SET_LINK_INFO		15

#define PPTP_MSG_MAX			15

/* PptpGeneralError values */
#define PPTP_ERROR_CODE_NONE		0
#define PPTP_NOT_CONNECTED		1
#define PPTP_BAD_FORMAT			2
#define PPTP_BAD_VALUE			3
#define PPTP_NO_RESOURCE		4
#define PPTP_BAD_CALLID			5
#define PPTP_REMOVE_DEVICE_ERROR	6

struct omni_PptpControlHeader {
	__be16	messageType;
	__u16	reserved;
};

/* FramingCapability Bitmap Values */
#define PPTP_FRAME_CAP_ASYNC		0x1
#define PPTP_FRAME_CAP_SYNC		0x2

/* BearerCapability Bitmap Values */
#define PPTP_BEARER_CAP_ANALOG		0x1
#define PPTP_BEARER_CAP_DIGITAL		0x2

struct omni_PptpStartSessionRequest {
	__be16	protocolVersion;
	__u16	reserved1;
	__be32	framingCapability;
	__be32	bearerCapability;
	__be16	maxChannels;
	__be16	firmwareRevision;
	__u8	hostName[64];
	__u8	vendorString[64];
};

/* PptpStartSessionResultCode Values */
#define PPTP_START_OK			1
#define PPTP_START_GENERAL_ERROR	2
#define PPTP_START_ALREADY_CONNECTED	3
#define PPTP_START_NOT_AUTHORIZED	4
#define PPTP_START_UNKNOWN_PROTOCOL	5

struct omni_PptpStartSessionReply {
	__be16	protocolVersion;
	__u8	resultCode;
	__u8	generalErrorCode;
	__be32	framingCapability;
	__be32	bearerCapability;
	__be16	maxChannels;
	__be16	firmwareRevision;
	__u8	hostName[64];
	__u8	vendorString[64];
};

/* PptpStopReasons */
#define PPTP_STOP_NONE			1
#define PPTP_STOP_PROTOCOL		2
#define PPTP_STOP_LOCAL_SHUTDOWN	3

struct omni_PptpStopSessionRequest {
	__u8	reason;
	__u8	reserved1;
	__u16	reserved2;
};

/* PptpStopSessionResultCode */
#define PPTP_STOP_OK			1
#define PPTP_STOP_GENERAL_ERROR		2

struct omni_PptpStopSessionReply {
	__u8	resultCode;
	__u8	generalErrorCode;
	__u16	reserved1;
};

struct omni_PptpEchoRequest {
	__be32 identNumber;
};

/* PptpEchoReplyResultCode */
#define PPTP_ECHO_OK			1
#define PPTP_ECHO_GENERAL_ERROR		2

struct omni_PptpEchoReply {
	__be32	identNumber;
	__u8	resultCode;
	__u8	generalErrorCode;
	__u16	reserved;
};

/* PptpFramingType */
#define PPTP_ASYNC_FRAMING		1
#define PPTP_SYNC_FRAMING		2
#define PPTP_DONT_CARE_FRAMING		3

/* PptpCallBearerType */
#define PPTP_ANALOG_TYPE		1
#define PPTP_DIGITAL_TYPE		2
#define PPTP_DONT_CARE_BEARER_TYPE	3

struct omni_PptpOutCallRequest {
	__be16	callID;
	__be16	callSerialNumber;
	__be32	minBPS;
	__be32	maxBPS;
	__be32	bearerType;
	__be32	framingType;
	__be16	packetWindow;
	__be16	packetProcDelay;
	__be16	phoneNumberLength;
	__u16	reserved1;
	__u8	phoneNumber[64];
	__u8	subAddress[64];
};

/* PptpCallResultCode */
#define PPTP_OUTCALL_CONNECT		1
#define PPTP_OUTCALL_GENERAL_ERROR	2
#define PPTP_OUTCALL_NO_CARRIER		3
#define PPTP_OUTCALL_BUSY		4
#define PPTP_OUTCALL_NO_DIAL_TONE	5
#define PPTP_OUTCALL_TIMEOUT		6
#define PPTP_OUTCALL_DONT_ACCEPT	7

struct omni_PptpOutCallReply {
	__be16	callID;
	__be16	peersCallID;
	__u8	resultCode;
	__u8	generalErrorCode;
	__be16	causeCode;
	__be32	connectSpeed;
	__be16	packetWindow;
	__be16	packetProcDelay;
	__be32	physChannelID;
};

struct omni_PptpInCallRequest {
	__be16	callID;
	__be16	callSerialNumber;
	__be32	callBearerType;
	__be32	physChannelID;
	__be16	dialedNumberLength;
	__be16	dialingNumberLength;
	__u8	dialedNumber[64];
	__u8	dialingNumber[64];
	__u8	subAddress[64];
};

/* PptpInCallResultCode */
#define PPTP_INCALL_ACCEPT		1
#define PPTP_INCALL_GENERAL_ERROR	2
#define PPTP_INCALL_DONT_ACCEPT		3

struct omni_PptpInCallReply {
	__be16	callID;
	__be16	peersCallID;
	__u8	resultCode;
	__u8	generalErrorCode;
	__be16	packetWindow;
	__be16	packetProcDelay;
	__u16	reserved;
};

struct omni_PptpInCallConnected {
	__be16	peersCallID;
	__u16	reserved;
	__be32	connectSpeed;
	__be16	packetWindow;
	__be16	packetProcDelay;
	__be32	callFramingType;
};

struct omni_PptpClearCallRequest {
	__be16	callID;
	__u16	reserved;
};

struct omni_PptpCallDisconnectNotify {
	__be16	callID;
	__u8	resultCode;
	__u8	generalErrorCode;
	__be16	causeCode;
	__u16	reserved;
	__u8	callStatistics[128];
};

struct omni_PptpWanErrorNotify {
	__be16	peersCallID;
	__u16	reserved;
	__be32	crcErrors;
	__be32	framingErrors;
	__be32	hardwareOverRuns;
	__be32	bufferOverRuns;
	__be32	timeoutErrors;
	__be32	alignmentErrors;
};

struct omni_PptpSetLinkInfo {
	__be16	peersCallID;
	__u16	reserved;
	__be32	sendAccm;
	__be32	recvAccm;
};

union omni_pptp_ctrl_union {
	struct omni_PptpStartSessionRequest	sreq;
	struct omni_PptpStartSessionReply	srep;
	struct omni_PptpStopSessionRequest	streq;
	struct omni_PptpStopSessionReply	strep;
	struct omni_PptpOutCallRequest	ocreq;
	struct omni_PptpOutCallReply		ocack;
	struct omni_PptpInCallRequest	icreq;
	struct omni_PptpInCallReply		icack;
	struct omni_PptpInCallConnected	iccon;
	struct omni_PptpClearCallRequest	clrreq;
	struct omni_PptpCallDisconnectNotify disc;
	struct omni_PptpWanErrorNotify	wanerr;
	struct omni_PptpSetLinkInfo		setlink;
};

/* crap needed for nf_conntrack_compat.h */
struct omni_track;
struct omni_track_expect;

extern int
(*omni_nat_pptp_hook_outbound)(struct sk_buff **pskb,
			     struct omni_track *track, enum omni_track_info trinfo,
			     struct omni_PptpControlHeader *ctlh,
			     union omni_pptp_ctrl_union *pptpReq);

extern int
(*omni_nat_pptp_hook_inbound)(struct sk_buff **pskb,
			    struct omni_track *track, enum omni_track_info trinfo,
			    struct omni_PptpControlHeader *ctlh,
			    union omni_pptp_ctrl_union *pptpReq);

extern void
(*omni_nat_pptp_hook_exp_gre)(struct omni_track_expect *exp_orig,
			    struct omni_track_expect *exp_reply);

extern void
(*omni_nat_pptp_hook_expectfn)(struct omni_track *track,
			     struct omni_track_expect *exp);

#if defined(CONFIG_FUTURE_OMNI_TRACK_SYNC)
extern int exp_gre_for_sb(struct omni_track *track, __be16 callid, __be16 peer_callid);
extern int dummy_gre_for_sb(struct omni_track *track, __be16 callid, __be16 peer_callid);
#endif

#endif /* __KERNEL__ */
#endif /* _NF_CONNTRACK_PPTP_H */
