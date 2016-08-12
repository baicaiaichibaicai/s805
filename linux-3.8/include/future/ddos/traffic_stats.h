#ifndef __TRAFFIC_STATS_H__
#define __TRAFFIC_STATS_H__

#include <asm/div64.h>
#include <asm/bitops.h>
#include <linux/tcp.h>

//the below based on threshold
enum 
{
#if 0
	TCP_SYN_ATTACK_IDX,
	TCP_SYNACK_ATTACK_IDX,
	TCP_FIN_ATTACK_IDX,
	TCP_ACK_ATTACK_IDX,
	TCP_RST_ATTACK_IDX,
	TCP_PSH_ATTACK_IDX,
	TCP_URG_ATTACK_IDX,
	TCP_CONN_ATTACK_IDX,
	HTTP_CONN_ATTACK_IDX,
	HTTP_REQ_ATTACK_IDX,
#endif 
	UDP_TA_ATTACK_IDX,
	ICMP_TA_ATTACK_IDX,
//	ICMP_PINGSIZE_ATTACK_IDX,
	IGMP_TA_ATTACK_IDX,
	DNS_TA_ATTACK_IDX,
	DHCP_TA_ATTACK_IDX,
	MAX_ATTACK_TYPE
};

enum tcp_flags
{
	TCP_SYN_SET,
	TCP_SYNACK_SET,
	TCP_FIN_SET,
	TCP_ACK_SET,
	TCP_RST_SET,
	TCP_PSH_SET,
	TCP_URG_SET,
	TCP_NONE_SET
};

enum l4_proto
{
	UDP_PKT_SET = TCP_NONE_SET,
	ICMP_PKT_SET,
	IGMP_PKT_SET,
	MAX_L4_PROTO
};

static inline uint64_t div64(uint64_t dividend, uint64_t divisor)
{   
	uint32_t d = divisor;

	/* Scale divisor to 32 bits */
	if (divisor > 0xffffffffULL) {
		unsigned int shift = fls(divisor >> 32);

		d = divisor >> shift;
		dividend >>= shift;
	}   

	/* avoid 64 bit division if possible */
	if (dividend >> 32)
		do_div(dividend, d);
	else
		dividend = (uint32_t) dividend / d;

	return dividend;
}

static inline uint64_t calc_rate(uint64_t numerator, uint64_t denominator)
{
	if( !denominator ) 
		return 0;

	return div64((numerator*100), (numerator > denominator ? numerator : denominator));
}

#endif  // __TRAFFIC_STATS_H__
