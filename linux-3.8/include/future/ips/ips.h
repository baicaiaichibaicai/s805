#ifndef _IPS_H_
#define _IPS_H_
#include <linux/skbuff.h>
#include <future/general.h>
#include <asm/uaccess.h>
//#define MAX_PATTERN_GROUP  128 
//#define _TCP_PORT_MAX	5
//#define _UDP_PORT_MAX	3
#define IS_PORTSCAN_ID(sid) ((sid >= PSNG_TCP_PORTSCAN) && (sid <= PSNG_OPEN_PORT))
#define IS_TCP_PORTSCAN_ID(sid) ((sid >= PSNG_TCP_PORTSCAN) && (sid <= PSNG_TCP_FILTERED_DISTRIBUTED_PORTSCAN))
#define IS_IP_PORTSCAN_ID(sid) ((sid >= PSNG_IP_PORTSCAN) && (sid <= PSNG_IP_FILTERED_DISTRIBUTED_PORTSCAN))
#define IS_UDP_PORTSCAN_ID(sid) ((sid >= PSNG_UDP_PORTSCAN) && (sid <= PSNG_UDP_FILTERED_DISTRIBUTED_PORTSCAN))
#define IS_ICMP_PORTSCAN_ID(sid) ((sid >= PSNG_ICMP_PORTSWEEP) && (sid <= PSNG_OPEN_PORT))

#define CONFIG_IPS_INSPECT 1   

struct act_ips_check {
#ifdef CONFIG_IPS_INSPECT
	enum {
		IPS_CHECK_NFWD = 0, /*Normal Forward*/
		IPS_CHECK_FFWD,		/*Fast Forward*/
		IPS_CHECK_INSP		/*Inspection*/
	} action;
#else
	enum {
		IPS_CHECK_NFWD = 0, 
		IPS_CHECK_FFWD
	} action;
#endif
};


typedef struct
{
	char *p_config;
    char *p_signature; 

    char master_ready; 
    char user_ready; 

    //int init_status; 
    atomic_t init_status; 
} ips_pattern_info_struct;

typedef struct
{
	struct net_device *sbr_dev;
	int ips_profile_id;
} static_bridge_info_struct;

typedef struct _ips_tracking_data
{
    unsigned long ips_start __attribute__((__aligned__(64)));
    uint32_t ips_watchdog ;
} ips_tracking_data;

//typedef struct
//{
//    uint16_t tcp_port[_TCP_PORT_MAX];
//    uint16_t udp_port[_UDP_PORT_MAX];
//
//    int8_t tcp_port_cnt;
//    int8_t udp_port_cnt;
//} split_port_struct;

DECLARE_PER_CPU(unsigned long, ips_start);
DECLARE_PER_CPU(uint32_t, ips_watchdog);

enum
{
    FIPS_INIT_REQ_INIT		= 0,
    FIPS_INIT_REQ_CONF		= 1,
    FIPS_INIT_REQ_MERGE		= 2,
    FIPS_INIT_REQ_ROM   	= 3,
    FIPS_INIT_DONE		= 4,
    FIPS_INIT_ERROR		= -1,
    FIPS_INIT_REQ_PROCFS	= 5,
};

enum
{
    FIPS_FLAG_STANDBY = 0,
    FIPS_FLAG_RUNNING = 1,
    FIPS_FLAG_INIT = 2,
    FIPS_FLAG_ERROR = 3,
};

//#define USE_SEPERATE_LOCK	1  // need to test more
#define DEBUG_LOCK	1
#ifdef DEBUG_LOCK
DECLARE_PER_CPU(int, _cpu_id);
DECLARE_PER_CPU(int, _func_id);
DECLARE_PER_CPU(int, _pcre_len);
#endif

extern ips_pattern_info_struct ips_pattern_info;
//extern char *file_list[MAX_PATTERN_GROUP];
extern int rule_file_cnt;
extern u32 ips_version, old_ips_version;
extern void *g_ips_mem_start, *g_ips_mem_end;
extern int g_ips_ids_mode;
extern char g_ips_ids_interface[16];
extern int g_ips_ids_profile_id;
extern static_bridge_info_struct static_bridge_info[MAX_ETH_COUNT]; 
extern rwlock_t *static_bridge_lock[];

extern void reset_ips_http_port(void);
extern void ips_prm_init(void);

int thread_load_pattern(void *unused);
int thread_audit_log(void *arg);
extern int pre_alloc_pattern_buf(void);
extern void wakeup_dpi_thread(void);

extern int fips_proc(struct sk_buff *skb);
extern int static_bridge_hook(struct sk_buff *skb);

static inline int get_ips_ids_mode (void)
{
    return g_ips_ids_mode;
}
static inline char *get_ips_ids_interface (void)
{
    return g_ips_ids_interface;
}
static inline int get_ips_ids_profile_id (void)
{
    return g_ips_ids_profile_id;
}
#endif // _IPS_H_

