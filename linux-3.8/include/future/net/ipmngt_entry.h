#ifndef __IP_MNG_H__
#define __IP_MNG_H__
#include <linux/timer.h>
#include <future/fw_util.h>

#define __MAX_ETH__		16

#define IPMNG_ENTRY_APP_SIZE	44	
#define IPMNG_NODE_SIZE		sizeof(struct __ipmng_node)

#define IPMNG_NET_BIOS         137
#define IPMNG_DNS              53

#define IPMAC_DENY				2
#define IPMAC_DETECT			3

#define IPMAC_RANGE				0
#define IPMAC_NETMASK			1
#define IPMAC_PREFIX			2

#define IP_MANAGE_LEN 			500
#define IP_MANAGE_FLUSH_TIMEOUT 3600 

#define IPM_HASH_TABLE_LEN		10000

#define is_ipmng(dev) ( dev ? ( g_ifindex_to_map[dev->ifindex] < __MAX_ETH__ ? ( ipmng_rule_count[ g_ifindex_to_map[dev->ifindex] ] > 0 ? 1 : 0 ) : 0 ) : 0 )

enum {
    IPMNG_SINGLE	= 1,
    IPMNG_RANGE,	
    IPMNG_NETMASK,
    IPMNG_SINGLE_WITH_MAC = 11,
    IPMNG_RANGE_WITH_MAC,	
    IPMNG_NETMASK_WITH_MAC,
    IPMNG_TYPE_MAC = 255
};

enum {
    IPMNG_ACCEPT	= 1,
    IPMNG_DENY,
    IPMNG_ACCEPT_MAC,
    IPMNG_DENY_MAC,
    IPMNG_POLICY_MAX	= 255
};

enum {
    IPMNG_STAT_ACCEPT = 1,
    IPMNG_STAT_DENY,	  
    IPMNG_STAT_MAC_COL,	  
    IPMNG_STAT_DETECT,	  
    IPMNG_STAT_BYPASS	  
};

typedef struct __ipmng_node{
    unsigned int	num;
    unsigned char 	iface[8];
    unsigned short	ip_type;
    unsigned short	set;
    unsigned int	ip;
    unsigned int	ip2;
    unsigned char	mac[8];
}ipmng_node; // 32 byte

typedef struct ip_manage_log {
	u_int num;
	u_int ip;
	u_int last_hit_time;
	char  iface[IFNAMSIZ];
	u_char mac[ETH_ALEN];
	short stat;
	struct ip_manage_log        *next;
} __attribute__((packed)) ip_manage_log;

extern int g_ipmngt; 
extern unsigned char g_ipmngt_mac[6];
extern unsigned short ipmng_rule_count[__MAX_ETH__];
extern ipmng_node	*ipmng_table[__MAX_ETH__];

extern int check_ipmng_network(char *ifname, unsigned int ip);
extern int check_ipmng_accept_host(char *ifname, unsigned int ip, unsigned char *mac);
extern int chk_ipmng_arp_rule(struct sk_buff *skb, struct net_device *dev, unsigned int sip, unsigned int tip, struct neigh_table *arp_tbl, const struct arphdr *arp, int action);

#if 0
extern int init_ipm_hash_tbl(void);
extern int add_ipm_host_tbl(void);
#endif

unsigned int ip_manage_hash_function(struct ip_manage_log *log);
void ip_manage_hash_flush(void);
#endif
