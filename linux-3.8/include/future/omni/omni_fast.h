#ifndef _OMNI_FAST_H
#define _OMNI_FAST_H

#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>

#define OMNI_DUMP	printk

extern unsigned int omni_fast_debug;
extern __be32 omni_dump_cond;

static inline struct tcphdr *omni_tcp_hdr(const struct sk_buff *skb)
{
	return (struct tcphdr *) (skb_network_header(skb) + 
		(ip_hdr(skb)->ihl * 4));
}

static inline struct udphdr *omni_udp_hdr(const struct sk_buff *skb)
{
	return (struct udphdr *) (skb_network_header(skb) + 
		(ip_hdr(skb)->ihl * 4));
}

static inline struct icmphdr *omni_icmp_hdr(const struct sk_buff *skb)
{
	return (struct icmphdr *) (skb_network_header(skb) + 
		(ip_hdr(skb)->ihl * 4));
}

static inline unsigned int skb_proto_dump(const struct sk_buff *skb)
{
	struct iphdr *iph = ip_hdr(skb);

	switch (iph->protocol) {
	case IPPROTO_TCP:
		OMNI_DUMP("TCP %u:%u ", 
				ntohs(omni_tcp_hdr(skb)->source), 
				ntohs(omni_tcp_hdr(skb)->dest));
		if (omni_tcp_hdr(skb)->syn) OMNI_DUMP("S");
		if (omni_tcp_hdr(skb)->fin) OMNI_DUMP("F");
		if (omni_tcp_hdr(skb)->ack) OMNI_DUMP("A");
		if (omni_tcp_hdr(skb)->rst) OMNI_DUMP("R");
		if (omni_tcp_hdr(skb)->urg) OMNI_DUMP("U");
		if (omni_tcp_hdr(skb)->psh) OMNI_DUMP("P");
		break;
	case IPPROTO_UDP:
		OMNI_DUMP("UDP %u:%u ",
				ntohs(omni_udp_hdr(skb)->source),
				ntohs(omni_udp_hdr(skb)->dest));
		break;
	case IPPROTO_ICMP:
		OMNI_DUMP("ICMP ");
		switch (omni_icmp_hdr(skb)->type) {
		case 0:
			OMNI_DUMP("Echo Rep [id=%04x, seq=%04x] ", 
					ntohs(omni_icmp_hdr(skb)->un.echo.id),
					ntohs(omni_icmp_hdr(skb)->un.echo.sequence));
			break;
		case 3:
			OMNI_DUMP("Dest ");
			switch (omni_icmp_hdr(skb)->code) {
			case 0: OMNI_DUMP("net_unreach "); break;
			case 1: OMNI_DUMP("host_unreach "); break;
			case 2: OMNI_DUMP("prot_unreach "); break;
			case 3: OMNI_DUMP("port_unreach "); break;
			default: OMNI_DUMP("unreach code=%d ", omni_icmp_hdr(skb)->code); break;
			}
			break;
		case 5:
			OMNI_DUMP("Redirect ");
			break;
		case 8:
			OMNI_DUMP("Echo Req [id=%04x, seq=%04x] ", 
					ntohs(omni_icmp_hdr(skb)->un.echo.id),
					ntohs(omni_icmp_hdr(skb)->un.echo.sequence));
			break;
		default:
			OMNI_DUMP("type=0x%02x, code=0x%02x ",
					omni_icmp_hdr(skb)->type,
					omni_icmp_hdr(skb)->code);
		}

		return 0;
	}
	return 0;
}

#define omni_fast_dump(format, ...) \
	if (omni_fast_debug) OMNI_DUMP(format, ##__VA_ARGS__)

#define omni_skb_dump4(skb, fmt, ...)                \
	do {                                             \
	if (omni_dump_cond &&                            \
			(omni_dump_cond == ip_hdr(skb)->saddr || \
			omni_dump_cond == ip_hdr(skb)->daddr)) { \
		OMNI_DUMP("%s: %pI4 - %pI4 ",                \
			__func__,                                \
			&(ip_hdr(skb)->saddr),                   \
			&(ip_hdr(skb)->daddr));                  \
		skb_proto_dump(skb);                         \
		OMNI_DUMP("\n");                             \
		OMNI_DUMP(fmt, ##__VA_ARGS__);               \
	}                                                \
	} while (0) 

static inline void omni_track_tuple_dump4(struct omni_track_tuple *tuple)
{
	OMNI_DUMP("dump tuple : %pI4 - %pI4 [%u] %u - %u\n", 
			&tuple->src.u3.ip,
			&tuple->dst.u3.ip,
			tuple->dst.protonum,
			ntohs(tuple->src.u.all),
			ntohs(tuple->dst.u.all));
}

#define omni_tuple_dump4(skb, tuple)                     \
	do {                                                 \
		if (omni_dump_cond &&                            \
				(omni_dump_cond == ip_hdr(skb)->saddr || \
				 omni_dump_cond == ip_hdr(skb)->daddr))  \
			omni_track_tuple_dump4(tuple);               \
	} while (0)

extern int omni_fast_init(void);
extern int omni_fast_rx_handle(struct sk_buff *skb);
extern int omni4_l3_fast_rcv(struct sk_buff *skb);
extern bool omni_fast_ready(__be16 eth_proto);
#endif
