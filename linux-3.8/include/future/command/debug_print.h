/* /include/future/command/debug_print.h
 *
 * (C) 2014 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _COMMAND_DEBUG_PRINT_H
#define _COMMAND_DEBUG_PRINT_H

#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <linux/icmpv6.h>
#include <linux/if_arp.h>

#include <future/prototype.h>
#include <future/vrrp.h>
#include <future/command/vty.h>
#include <future/command/admin.h>


unsigned short parse_vlan_id(struct sk_buff *);

void _print_conn_info(struct sk_buff *skb);
#define print_conn_info(d_fwd, skb) \
		do { _print_conn_info(skb); } while(0)

void _print_conn_info2(struct sk_buff *skb, int verdict);
#define print_conn_info2(d_fwd, skb, verdict) \
		do { _print_conn_info2(skb, verdict); } while(0)

void _print_ip_capture(struct net_device *, struct sk_buff *, char *);
#define print_ip_capture(dev, skb, msg) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE) \
				_print_ip_capture(dev, skb, msg); } while(0)

void _print_ether(struct net_device *, struct sk_buff *, char *);
#define print_ether(dev, skb, msg) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE) \
				_print_ether(dev, skb, msg); } while(0)


void _print_ip(struct net_device *, struct sk_buff *, char *);
#define print_ip(dev, skb, msg) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE) \
				_print_ip(dev, skb, msg); } while(0)

void _print_ip2(struct net_device *, struct sk_buff *, char *);
#define print_ip2(dev, skb, msg) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE) \
				_print_ip2(dev, skb, msg); } while(0)

void _print_ip3(struct net_device *, struct sk_buff *, char *);
#define print_ip3(dev, skb, msg) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE) \
				_print_ip3(dev, skb, msg); } while(0)

#ifdef CONFIG_FERRET_SYSTEM_ADMIN
void _print_create_omni_track(struct sk_buff *, struct omni_track *);
void _print_create_ipv6_omni_track(struct sk_buff *, struct omni_track *);
#define print_create_omni_track(skb, tr) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE) \
			    _print_create_omni_track(skb, tr); \
			else if (common_dl & DL_CAPTURE6) \
				_print_create_ipv6_omni_track(skb, tr); } while(0)
#endif

void _print_close_omni_track(struct omni_track *);
void _print_close_ipv6_omni_track(struct omni_track *);
#define print_close_omni_track(tr) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE) \
			    _print_close_omni_track(tr); \
			else if (common_dl & DL_CAPTURE6) \
				_print_close_ipv6_omni_track(tr); } while(0)

void _print_arp(struct net_device *, struct sk_buff *, char *, int );
#define print_arp(dev, skb, msg, flag) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE) \
				_print_arp(dev, skb, msg, flag); } while(0)

void _print_conn_ipv6_info(struct sk_buff *skb);
#define print_conn_ipv6_info(d_fwd, skb) \
		do { _print_conn_ipv6_info(skb); } while(0)

void _print_conn_ipv6_info2(struct sk_buff *skb, int verdict);
#define print_conn_ipv6_info2(d_fwd, skb, verdict) \
		do { _print_conn_ipv6_info2(skb, verdict); } while(0)

void _print_ipv6(struct net_device *, struct sk_buff *, char *, u8 );
#define print_ip6(dev, skb, msg, flag) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE6) \
				_print_ipv6(dev, skb, msg, flag); } while(0)

void _print_ipv6_capture(struct net_device *, struct sk_buff *, char *, u8 );
#define print_ipv6_capture(dev, skb, msg, flag) \
		do { extern unsigned int common_dl; \
			if (common_dl & DL_CAPTURE6) \
				_print_ipv6_capture(dev, skb, msg, flag); } while(0)
#endif
/*_COMMAND_DEBUG_PRINT_H*/

extern void print_dhcp(struct vty *, struct dhcp_packet *, int);
extern void print_tcp(struct vty *, struct tcphdr *, int);
extern void print_udp(struct vty *, struct udphdr *, int);
extern void print_gre(struct vty *, struct grehdr *, int);
extern void print_esp(struct vty *, struct esp *, int);
extern void print_ah(struct vty *, struct ah *, int);
extern void print_vrrp(struct vty *, struct vrrphdr *, int);
extern void print_icmp(struct vty *, struct icmphdr *, int);
extern void print_icmp6(struct vty *, struct icmp6hdr *, int);                         
