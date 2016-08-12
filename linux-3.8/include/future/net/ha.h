#ifndef __NET_HA_H__
#define __NET_HA_H__


int ha_icmp_rcv_process(struct sk_buff *skb, struct icmphdr *icmph);
int ha_icmp6_rcv_process(struct sk_buff *skb, struct icmp6hdr *hdr, const struct in6_addr *saddr);

#endif


