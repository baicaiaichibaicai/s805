/* /include/future/prototype.h
 *
 * (C) 2014 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _FUTURE_PROTO_TYPE_H
#define _FUTURE_PROTO_TYPE_H

#include <linux/in.h>
#include <future/object.h>

#define FTP	21
#define DHCP	67
#define PORTMAP	111
#define RTM	506

#define GETPORT 3

#define DHCP_SNAME_LEN	63
#define DHCP_FILE_LEN	128

/* DHCP messages types. */
#define	DHCPDISCOVER	1
#define	DHCPOFFER	2
#define	DHCPREQUEST	3
#define	DHCPDECLINE	4
#define	DHCPACK		5
#define	DHCPNAK		6
#define	DHCPRELEASE	7
#define	DHCPINFORM	8


struct ah {
	__u8	ah_nh;	/* Next hader (protocol) */
	__u8	ah_hl;	/* AH length, in 32-bit words */
	__u16	ah_rv;	/* reserved, must be 0 */
	__u32	ah_spi;	/* Security Paramters Index */
	__u32	ah_rpl;	/* Replay preventrion */
};

struct esp {
	__u32	esp_spi;	/* Security Parameter Index */
	__u32	esp_rpl;	/* Replay counter */
	__u8	esp_iv[8];	/* IV */
};

struct grehdr {
	__u16	flags;
	__u16	proto;
};

struct rpc {
	u16 header;
	u16 length;
	u32 xid;
	u32 msg_type;
	u32 program;
	u32 version;
	u32 procedure;
};

struct dhcp_packet {
    __u8 op;		/* Message opcode/type */
    __u8 htype;		/* Hardware addr type */
    __u8 hlen;		/* Hardware addr length */
    __u8 hops;		/* Number of relay agent hops from client */
    __u32 xid;		/* Transaction ID */
    __u16 secs;		/* Seconds since client started looking */
    __u16 flags;	/* Flag bits */
    struct in_addr ciaddr; /* Client IP address (if already in use) */
    struct in_addr yiaddr; /* Client IP address */
    struct in_addr siaddr; /* IP address of next server to talk to */
    struct in_addr giaddr; /* DHCP relay agent IP address */
    __u8 chadr[16]; /* Client hardware address */
    __u8 sname[DHCP_SNAME_LEN]; /* Server name */
    __u8 file[DHCP_FILE_LEN]; /* Boot filename */
    __u8 options[0]; /* Optional parameters */
} __attribute__ ((packed));

//from fos4.0 (include/future/ma.h)
typedef struct
{
    char		hi_ver	; // V15:3  V20:4
    char		lo_ver	; // 0
    char		type	; // Message Type
    char		code	; // Message Code
    short		error	; // Message Error Code
    char		mode	; // 0=Primary, 1=Backup
    char		rsrvd	; // Reserved
    u_short		tot_seq	; // Total Sequence Number(1 ~ 65535)
    u_short		cur_seq	; // Current Sequence Number(<= tot_seq)
    u_int		length	; // Total Message Length(header+data+mac)
    u_int		spi	; // Message Unique ID
    obj_id_struct	s_id	; // Sender ID
    obj_id_struct	r_id	; // Recipient ID
    u_short		enc_alg	; // Encryption Algorithm( 0 if not used )
    u_short		mac_alg	; // MAC Algorithm( 0 if not used )

    char		data[0]	;
} __attribute__((packed)) msg_header; // 56(=0x38) bytes

struct vrrphdr {        /* rfc2338.5.1 */
	uint8_t            vers_type;      /* 0-3=type, 4-7=version */
	uint8_t            vrid;           /* virtual router id */
	uint8_t            priority;       /* router priority */
	uint8_t            naddr;          /* address counter */
	uint8_t            auth_type;      /* authentification type */
	uint8_t            adver_int;      /* advertissement interval(in sec) */
	uint16_t           chksum;         /* checksum (ip-like one) */
	/* here <naddr> ip addresses */
	/* here authentification infos */
};

#endif/*_FUTURE_PROTO_TYPE_H*/

