#ifndef __FW_UTIL_H__
#define __FW_UTIL_H__

#include <future/general.h>
#ifdef CONFIG_FERRET_IP_MANAGE
#include <future/net/heckler.h>
#endif

#define PROTOCOL(skb)			(ip_hdr(skb)->protocol)
#define SADDR(skb)				(ip_hdr(skb)->saddr)
#define DADDR(skb)				(ip_hdr(skb)->daddr)
#define SPORT(skb)				(tcp_hdr(skb)->th_sport)
#define DPORT(skb)				(tcp_hdr(skb)->th_dport)
#define SERVICE(skb)			(tcp_hdr(skb)->th_dport)

#define MAX_TRUST_HOST_COUNT	200

#ifdef CONFIG_X86
#define MAX_IPM_NETWORK_COUNT			10
#define MAX_IPM_ACCEPT_HOST_COUNT		1000
#else
#define MAX_IPM_NETWORK_COUNT			5
#define MAX_IPM_ACCEPT_HOST_COUNT		500
#endif

#define BONDPACK_GROUP1         2
#define BONDPACK_GROUP2         4
#define BONDPACK_GROUP3         8
#define BONDPACK_GROUP4         16

#define TRUST_HOST_RULE_LOOKUP -1
#define TRUST_HOST_ACCEPT		1
#define TRUST_HOST_DROP			0

#define uint64_calc_inc(cur, prev) (prev > cur ? ((ULLONG_MAX-prev)+cur) : (cur-prev))
extern struct net_device        *netdev_global[NR_ETH];
extern int                      g_use_bond_pack;                        // bond_pack 사용 여부
extern int                      g_use_bond_pack_num;
extern int                      g_bond_pack_set[NR_ETH];                // bond_pack seting flag
extern struct net_device *netdev_global[NR_ETH];
extern unsigned int g_fw_ids_mode;			//for fw ids mode

int fw_timer_thread(void *arg);
int get_ifindex_from_name(char* name);

// for trust host ip
struct _trust_host_ip {
	int type;
	int addr1;
	int addr2;
	char user[32];
};

extern int g_trust_host_ip_cnt;
extern int g_multipath_timeout;
int is_trust_host_ip(unsigned int addr);
int get_trust_host_ip(char *buf);
int show_trust_host_ip(char *buf);
int check_management_service(struct sk_buff *skb);
int check_service_on(struct sk_buff *skb, u_int8_t protonum);
void trust_host_set_skip_path(struct omni_track *track);
#ifdef CONFIG_FERRET_IP_MANAGE
extern struct ipm_network_struct ipm_network[MAX_IPM_NETWORK_COUNT];
extern struct ipm_accept_host_struct ipm_accept_host[MAX_IPM_ACCEPT_HOST_COUNT];
int get_ip_manage_network(char *buf, int idx);
int show_ip_manage_network(char *page, int ret);
int get_ip_manage_accept_host(char *buf, int idx);
int get_ipm_accept_host_ip(char *buf, int idx);
int get_ipm_accept_host_mac(char *buf, int idx);
#endif

int redirect_packet_v4(struct sk_buff* skb, struct net_device* dev);
int redirect_packet_v6(struct sk_buff* skb, struct net_device* dev);
void mac_get_by_name(void);

typedef struct _iface_map 
{
	char 	name[IFNAMSIZ];
	int		ifindex;
	u_short	primary_flag;
	u_short backup_flag;
	struct net_device	*dev;
	u_int 				ipv4;
	u_char				ipv6[16];

} iface_map;

extern iface_map	g_map_to_ifindex[MAX_INTERFACE_COUNT];
extern int g_selected_iface; // mpr_iface
extern uint64_t g_bandwidth[MAX_ETH_COUNT];

extern int g_line_mng_bandwidth_down_duration;
extern uint64_t g_line_mng_bandwidth;

#define MAX_LINE_MNG_TIME_ENTRY 20
#define LINE_MANAGE_UPDOWN      1
#define LINE_MANAGE_BANDWIDTH   2
struct _line_mng_entry {
	int         position;
	time_t      down_time[MAX_LINE_MNG_TIME_ENTRY];
};

struct _line_mng_base {
	int                         check_interval;
	int                         check_count;
	int                         force_down_duration;
	struct _line_mng_entry      entry[MAX_ETH_COUNT];
};

struct _line_mng_timer {
	int                         status;
	struct timer_list           timer;
};

extern struct _line_mng_timer g_line_mng_timer[];
extern struct _line_mng_base *g_line_mng_base;


void str2mac(const char *macaddr, unsigned char mac[6]);

static inline int get_fw_ids_mode (void)
{
	return g_fw_ids_mode;
}
#endif
