#ifndef _OMNI_TRACK_TFTP_H
#define _OMNI_TRACK_TFTP_H

#define TFTP_PORT 69

struct tftphdr {
	__be16 opcode;
};

#define TFTP_OPCODE_READ	1
#define TFTP_OPCODE_WRITE	2
#define TFTP_OPCODE_DATA	3
#define TFTP_OPCODE_ACK		4
#define TFTP_OPCODE_ERROR	5

extern unsigned int (*omni_nat_tftp_hook)(struct sk_buff **pskb,
				        enum omni_track_info ctinfo,
				        struct omni_track_expect *exp);

#endif /* _OMNI_TRACK_TFTP_H */
