/* /include/future/command/capture.h
 *
 * (C) 2014 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _COMMAND_CAPTURE_H
#define _COMMAND_CAPTURE_H

#include <linux/if.h>
#include <uapi/linux/in6.h>

enum capture_type {
	CAPTURE_TYPE_DISABLED = 0,
	CAPTURE_TYPE_SINGLE_IP_ADDR = (1 << 0),
	CAPTURE_TYPE_MULTI_IP_ADDR_AND = (1 << 1),
	CAPTURE_TYPE_MULTI_IP_ADDR_OR = (1 << 2),
	CAPTURE_TYPE_MULTI_IP_ADDR = (CAPTURE_TYPE_MULTI_IP_ADDR_AND | CAPTURE_TYPE_MULTI_IP_ADDR_OR),
	CAPTURE_TYPE_IP = (CAPTURE_TYPE_SINGLE_IP_ADDR | CAPTURE_TYPE_MULTI_IP_ADDR),

	CAPTURE_TYPE_PROTOCOL = (1 << 3),
	CAPTURE_TYPE_PORT = (1 << 4),
	CAPTURE_TYPE_IFACE = (1 << 5),

	CAPTURE_TYPE_OPT1 = (CAPTURE_TYPE_IP | CAPTURE_TYPE_PROTOCOL | CAPTURE_TYPE_PORT | CAPTURE_TYPE_IFACE),
	CAPTURE_TYPE_UID_SPD = (1 << 6),
	CAPTURE_TYPE_UID_NAT = (1 << 7),
	CAPTURE_TYPE_UID = (CAPTURE_TYPE_UID_SPD | CAPTURE_TYPE_UID_NAT),
	CAPTURE_TYPE_DROP = (1 << 8),
	CAPTURE_TYPE_OPT2 = (CAPTURE_TYPE_UID | CAPTURE_TYPE_DROP),
	CAPTURE_TYPE_ARP = (1 << 9),

	CAPTURE_TYPE_SINGLE_IPV6_ADDR = (1 << 10),
	CAPTURE_TYPE_MULTI_IPV6_ADDR_AND = (1 << 11),
	CAPTURE_TYPE_MULTI_IPV6_ADDR_OR = (1 << 12),
	CAPTURE_TYPE_MULTI_IPV6_ADDR = (CAPTURE_TYPE_MULTI_IPV6_ADDR_AND | CAPTURE_TYPE_MULTI_IPV6_ADDR_OR),
	CAPTURE_TYPE_IPV6 = (CAPTURE_TYPE_SINGLE_IPV6_ADDR | CAPTURE_TYPE_MULTI_IPV6_ADDR),
};

struct capture_ip {
	__be32 addr;
	__be32 mask;
	__u16 mask_len;
};

struct capture_ip6 {
	struct in6_addr addr;
	__be32 mask;
	__u16 mask_len;
};

struct capture_info {
	enum capture_type type;
	struct capture_ip ip1;
	struct capture_ip ip2;
	struct capture_ip6 ip6_1;
	struct capture_ip6 ip6_2;
	__be32 arp;
	__u8 protocol;
	__be16 port;
	char iface[IFNAMSIZ];
	__u16 uid;
	__u16 opt1:1,
		  opt2:1,
		  drop:1;

	//must be last
	__u16 capture:1,
		  trace:1,
		  mac:1,
		  kbd_timeout:1; //nothing to do
};

#endif/*_COMMAND_CAPTURE_H*/

