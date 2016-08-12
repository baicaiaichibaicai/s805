/* /include/future/command/debug_check.h
 *
 * (C) 2014 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _COMMAND_DEBUG_CHECK_H
#define _COMMAND_DEBUG_CHECK_H


#include <linux/skbuff.h>
#include <linux/ip.h>
#include <future/general.h>

extern void check_debug_opt2(struct sk_buff *skb);
extern void check_debug_ipv6_opt2(struct sk_buff *skb);
extern void check_ip_debug(struct sk_buff *skb);
extern void check_ip6_debug(struct sk_buff *skb);
extern void check_arp_debug(struct sk_buff *skb);
extern void check_8021Q_ip_debug(struct sk_buff *skb);

static inline void check_debug(struct sk_buff *skb)
{
	switch (ntohs(skb->protocol)) {
		case ETH_P_IP:
			check_ip_debug(skb);
			break;
		case ETH_P_ARP:
			check_arp_debug(skb);
			break;
		case ETH_P_8021Q:
			check_8021Q_ip_debug(skb);
			break;
		case ETH_P_IPV6:  
			check_ip6_debug(skb);
			break;
		case ETH_P_LLTD:
			check_ip_debug(skb);
			break;
	}
}

static inline void check_debug_by_ipver(struct sk_buff *skb)
{
	switch(ip_hdr(skb)->version) {
		case 4:
			check_ip_debug(skb);
			break;
		case 6:
			check_ip6_debug(skb);
			break;
		default:
			if (skb->d_fwd)
				debug(DL_SLOG, "unknown ip version[%d]\n", ip_hdr(skb)->version);
	}
}

int check_debug_from_opt2(struct capture_info *, u32 , u32 , u32);
int check_debug_from_ipv6_opt2(struct capture_info *, u32 , u32 , u32);
extern int check_debug_from_ip(struct capture_info *, int, int);
extern int check_debug_from_ip6(struct capture_info *, struct in6_addr, struct in6_addr);

#endif/*_COMMAND_DEBUG_CHECK_H*/

