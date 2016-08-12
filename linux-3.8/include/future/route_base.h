#ifndef __ROUTEBASE_H__
#define __ROUTEBASE_H__

#include <linux/if_ether.h>
#include <uapi/linux/if.h>
#include <uapi/linux/in6.h>
#include <future/command/vty.h>
#include <future/general.h>

#define GBUF_SIZE       2048
#define LINE_BUF        256


typedef enum rt_access_list_type_st {
    ROUTE_ACCESS_LIST_TYPE_SINGLE = 1,
    ROUTE_ACCESS_LIST_TYPE_NETMASK = 4,
    ROUTE_ACCESS_LIST_TYPE_RANGE = 7,
    ROUTE_ACCESS_LIST_TYPE_ANY = 9,
    ROUTE_ACCESS_LIST_TYPE_EVEN,
    ROUTE_ACCESS_LIST_TYPE_ODD,
    ROUTE_ACCESS_LIST_TYPE_MAX,
} rt_access_lise_type;

typedef enum rt_access_list_mode_st {
    ROUTE_ACCESS_LIST_MODE_ACCEPT,
    ROUTE_ACCESS_LIST_MODE_REDIRECT,
    ROUTE_ACCESS_LIST_MODE_DENY,
    ROUTE_ACCESS_LIST_MODE_PERMIT,
    ROUTE_ACCESS_LIST_MODE_BYPASS,
} rt_access_lise_mode;

struct serial_sync_str {
	u64                         flag;
	char						use;
	char						internal_state;
	char                        mac[ETH_ALEN];
	char                        if_name[IFNAMSIZ];
};

#define s4_addr			in6_u.u6_addr8
#define s4_addr32		in6_u.u6_addr32[0]
typedef struct in6_addr in6_addr;

typedef struct rt_access_list_type {
    u32                         type;   // Single(NET_SINGLE), Range(NET_RANGE), Bitmask(NET_BITMASK), Odd, Even, Any
    u16                         mode;
	u16							prefix;
    in6_addr                    value1;
    in6_addr                    value2;
} rt_access_list;


typedef struct line_mng_list_type {
	u_int			iface;
	u_int			limit;
	u_int			time;
	u_int			type;
} line_mng_list;

#define ROUTE_IP_REDIRECT_TABLE_MAX		128
#define EXPLICIT_ROUTING_MAX			64
typedef struct rt_access_network_node {
    u32                         addr;
    u16                         mode;   // 0:Accept 1:Redirect
    u16                         iface;	// using explicit_routing,
    rt_access_list              src;
    rt_access_list              dst;
} rt_access_network_list;

extern int g_multipath_type;
extern int g_trap_version;
extern int g_ha_flag;
extern int g_ha_standby;
extern int g_inout_sync_drop_ip;

#ifdef CONFIG_FERRET_IPSEC
extern int g_aria256_future;
#endif

extern int g_blackhole_pass;

extern line_mng_list g_line_mng_list[MAX_INTERFACE_COUNT];
extern struct serial_sync_str g_serial_sync;
extern int rt_serial_network_table_count;
extern int rt_serial_network_v6_table_count;
extern rt_access_network_list rt_serial_network_table[ROUTE_IP_REDIRECT_TABLE_MAX];
extern rt_access_network_list rt_serial_network_v6_table[ROUTE_IP_REDIRECT_TABLE_MAX];

extern int g_explicit_routing_count;
extern rt_access_network_list rt_explicit_table[EXPLICIT_ROUTING_MAX];

int rt_script_load(void);
int rt_script_in_serial_network_v4(__be32 sip, __be32 dip);
int rt_script_in_serial_network_v6(in6_addr src_addr,in6_addr dst_addr);
int rt_script_in_explicit_routing_v4(__be32 sip, __be32 dip);
int cmd_print_serial_network_table(struct vty* vty);
int cmd_print_explicit_routing_table(struct vty* vty);
#endif
