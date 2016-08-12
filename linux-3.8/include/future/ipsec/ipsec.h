#ifndef _IPSEC_H_
#define _IPSEC_H_

#ifdef CONFIG_FERRET
// function define
#define FERRET_VPN_RANGE
#define FERRET_VPN_MULTINET
#define FERRET_VPN_MULTI_POLICY


// general define
#define MAX_MULTINET		100

//#define FERRET_VPN_NETLINK_BUF	20480UL // strongswan netlink_buf_t size
#define FERRET_VPN_NETLINK_BUF	8192UL // strongswan netlink_buf_t size
#define ZAMDBG	printk("%s : %s[%d]\n", __FILE__, __FUNCTION__, __LINE__)

#define VPN_NATT_PORT		4500
#define IPSEC_DROP			-1
#define IPSEC_SUCCESS		0
#define IPSEC_BYPASS		1
#define IPSEC_PROCESS		2
#define IPSEC_SKIP			3
#define IPSEC_ASYNC			4

#endif
#endif
