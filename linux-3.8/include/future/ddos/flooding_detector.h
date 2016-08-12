#ifndef __FLOODING_DETECTOR_H__
#define __FLOODING_DETECTOR_H__

#include <linux/smp.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/skbuff.h>
#include <future/general.h>
#include <future/ddos/traffic_stats.h>
#include <future/ddos/ddos_util.h>

#define MAX_UDP_PORT_CNT    10
#define MAX_UDP_PORT        65536 / 8

#define MAX_CLIENT_QUEUE    100 
#define MAX_DETECTED_QUEUE  4 
#define DIST_ARR_SIZE       512
#define DDOS_PROFILE_NAME_LEN   16

//#define DDOS_PROFILE_INIT_READY      0x01
//#define DDOS_PROFILE_INIT_DONE       0x02

#define CONVERT_PROFILE_TABLE(dpt) ((struct ddos_profile_table *) dpt)
#define CONVERT_USER_TABLE(dut) ((struct ddos_user_table *) dut)

#define MODE_NOT_USED       0x00
#define MODE_DETECT         0x01
#define MODE_BLOCK          0x02

#define DNS_PORT            53
#define DHCP_PORT           67

enum
{
	CLIENT_SIDE,
	SERVER_SIDE,
	BOTH_SIDE
};

enum
{
	LOW_SENSITIVE_LEVEL,
	MID_SENSITIVE_LEVEL,
	HIGH_SENSITIVE_LEVEL,
	MAX_SENSITIVE_LEVEL
};

enum
{
	TCP_BLOCK_IDX,
	UDP_BLOCK_IDX,
	ICMP_BLOCK_IDX,
	IGMP_BLOCK_IDX,
	MAX_BLOCK_IDX,
};

enum
{
	STATE_READY_TO_DETECT        = 0x0001,
	STATE_UNDER_ATTACK           = 0x0002,
	STATE_LEGITIMATE             = 0x0004,
	STATE_WHITE_USER             = 0x0008,
	STATE_BLACK_USER             = 0x0010,
	STATE_NOT_SPOOFED            = 0x0020,
	STATE_COMPLETE_TRANSACTION   = 0x0040,
	STATE_DISCARD_USER           = 0x0080,
};

struct sensitive_table
{
	u_int max_dmd;
	u_int min_pps;
	u_int min_tcp;
	u_int dmd_rate;
	u_int half_rate;
	u_int burst_rate;
	u_int udp_pps_rate;
	u_int udp_bps_rate;
	u_int icmp_pps_rate;
	u_int icmp_bps_rate;
	u_int igmp_pps_rate;
	u_int igmp_bps_rate;
	u_int illegal_rate;
	int bps_check;
};

struct threshold_table
{
	u_int l4_pps[MAX_ATTACK_TYPE];
	char chg_state[MAX_ATTACK_TYPE];
} __attribute__((packed));

struct block_info
{
	__be32 saddr;
	__be32 daddr;
	__be16 sport;
	__be16 dport;
	__u8 protocol;
	struct timer_list timeout;
} __attribute__((packed));

struct drop_info
{
	uint64_t bps;
	u_int pps;
};

struct attack_info
{
	uint64_t byte;
	uint64_t drop_byte;
	u_int pkt_cnt;
	u_int snapshot;
	u_int drop_cnt;
};

struct rate_limit
{
	u_int pps;
	u_int bps;
	u_int cps;
	u_int rps;
};

struct detect_record
{
	struct block_info blk_info[MAX_BLOCK_IDX];
	uint64_t accum_byte[MAX_ATTACK_TYPE];
	uint64_t drop_accum_byte[MAX_ATTACK_TYPE];
	u_int detect_queue[MAX_ATTACK_TYPE];
	u_int accum_pkt[MAX_ATTACK_TYPE];
	u_int drop_accum_pkt[MAX_ATTACK_TYPE];
	u_int snapshot_pkt[MAX_ATTACK_TYPE];
	uint64_t drop_byte;
	u_int attack_type_map;
	u_int drop_cnt;
	u_short block_attack_map;
	u_short detect_queue_cnt;
	char under_nto1_attack;
} __attribute__((packed));

struct detect_record_disp
{
	uint64_t drop_bps;
	uint64_t detected_bps;
	u_int drop_pps;
	u_int detected_pps;
};

struct dist_element
{
	__be32 ip;
};

struct pkt_rate
{
	u_int dmd_rate[MAX_L4_PROTO];
	u_int tcp_pps_rate;
	u_int tcp_bps_rate;
	u_int udp_pps_rate;
	u_int udp_bps_rate;
	u_int icmp_pps_rate;
	u_int icmp_bps_rate;
	u_int igmp_pps_rate;
	u_int igmp_bps_rate;
	u_int tcp_conn_rate;
} __attribute__((packed));

struct packet_accum
{
	u_int spu[MAX_L4_PROTO];
	u_int cli_request;
	u_int half_open;
	u_int conn_est;
	u_int http_conn_est;
	u_int legal_conn;
	u_int dns_pps;
	u_int dhcp_pps;
	u_int ab_conn_est;
	u_int ab_cli_req;
	u_int ab_http_conn;
	uint64_t udp_bps;
	uint64_t dns_bps;
	uint64_t dhcp_bps;
	uint64_t icmp_bps;
	uint64_t igmp_bps;
	uint64_t tcp_bps;
};

struct pkt_state
{
	u_int proto[MAX_L4_PROTO];
	u_int dmd[MAX_L4_PROTO];

	__u64 tcp_bps;
	__u64 udp_bps;
	__u64 dns_bps;
	__u64 dhcp_bps;
	__u64 icmp_bps;
	__u64 igmp_bps;

	u_int con_dmd;
	u_int dns_dmd;
	u_int dhcp_dmd;
	u_int conn_est;
	u_int half_open;
	u_int tcp_pps;

	u_int http_connect;
	u_int http_req;
	u_int dns_pps;
	u_int dhcp_pps;
	u_int svr_load;
	u_int ab_conn_est;
	u_int ab_cli_req;
	u_int ab_http_conn;

	struct pkt_rate rate;
} __attribute__((packed));

struct update_period
{
	u_long jiffies;
	u_long udp_jiffies;
	u_long cc_jiffies;
};

struct udp_port_state
{
	atomic_t state;
	u_short port;
} __attribute__((packed));


struct ddos_server_state
{
	struct pkt_state svr_pkt_state;
	unsigned long jiffies;
} __attribute__((packed));

struct ddos_user_table
{
	struct packet_accum user_pkt_accum;
	struct update_period user_upd_period;
	struct detect_record user_detect_record;
	struct udp_port_state oneway_udp_state[MAX_UDP_PORT_CNT];
	u_short fpt_map[63];
	u_long status;
	int proxy_state_map;
	int proxy_complete_map;
	__be32 addr;
	atomic_t count;
	atomic_t cc_attack_cnt;
} __attribute__((packed));

struct http_attack_configuration
{
	u_int http_blk_time;
	u_int pkt_size;
	u_int pkt_cnt;
	u_int pkt_interval;
	u_int request_timeout;
	u_int win_size;
	u_int zero_win_cnt;
	u_int content_len;
	u_int post_param_cnt;
	u_int detect_cnt;
	u_int detect_interval;

	int http_use;
	int http_action;
	int server_rst_use;
	int pkt_anomaly_use;
	int tcp_window_use;
	int post_hdr_use;
	int cc_attack_use;
	int http_port_use;
};

struct policy_configuration
{
	struct http_attack_configuration http_conf;

	u_int learn_term;
	u_int time_slice;
	u_int detect_delicacy;
	u_int server_blk_time;
	u_int client_blk_time;

	char detection_server;
	char detection_client;

	char clearly_done;

} __attribute__((packed));

struct ddos_profile_table;

struct ddos_inst_attribute {
	struct attribute attr;
	ssize_t (*show)(struct ddos_profile_table *ddosi, char *buf);
	ssize_t (*store)(struct ddos_profile_table *ddosi, const char *buf, size_t len);
} __attribute__((packed));

#define DECLARE_DDOS_ATTR(name) \
	struct ddos_inst_attribute name

struct profile_sysfs
{
	struct kobject kobj;

	DECLARE_DDOS_ATTR(time_slice);
	DECLARE_DDOS_ATTR(learn_term);

	// detection
	DECLARE_DDOS_ATTR(detection_delicacy);
	DECLARE_DDOS_ATTR(detection_server);
	DECLARE_DDOS_ATTR(server_block_time);
	DECLARE_DDOS_ATTR(detection_client);
	DECLARE_DDOS_ATTR(client_block_time);
	DECLARE_DDOS_ATTR(client_threshold);
	DECLARE_DDOS_ATTR(server_threshold);

	// http config
	DECLARE_DDOS_ATTR(http_action);
	DECLARE_DDOS_ATTR(pkt_size);
	DECLARE_DDOS_ATTR(pkt_cnt);
	DECLARE_DDOS_ATTR(pkt_interval);
	DECLARE_DDOS_ATTR(win_size);
	DECLARE_DDOS_ATTR(zero_win_cnt);
	DECLARE_DDOS_ATTR(content_len);
	DECLARE_DDOS_ATTR(post_param_cnt);
	DECLARE_DDOS_ATTR(cc_attack_use);
	DECLARE_DDOS_ATTR(use_port);
	DECLARE_DDOS_ATTR(http_use);
	DECLARE_DDOS_ATTR(http_block_time);
	DECLARE_DDOS_ATTR(server_rst_use);
	DECLARE_DDOS_ATTR(pkt_anomaly_use);
	DECLARE_DDOS_ATTR(request_timeout);
	DECLARE_DDOS_ATTR(tcp_window_use);
	DECLARE_DDOS_ATTR(post_hdr_use);
	DECLARE_DDOS_ATTR(detect_cnt);
	DECLARE_DDOS_ATTR(detect_interval);
	DECLARE_DDOS_ATTR(http_port_use);

} __attribute__((packed));

struct ddos_profile_table
{
	atomic_t used;
//	rwlock_t svr_lock;

	struct profile_sysfs profile_sys;
	struct pkt_state score;
	struct pkt_state process;
	struct threshold_table threshold[BOTH_SIDE];
	struct dist_element syn_dmd[DIST_ARR_SIZE];
	struct dist_element con_dmd[DIST_ARR_SIZE];
	struct dist_element tcp_dmd[DIST_ARR_SIZE];
	struct dist_element udp_dmd[DIST_ARR_SIZE];
	struct dist_element icmp_dmd[DIST_ARR_SIZE];

	struct policy_configuration config;

	struct ddos_server_state svr_state;
	struct timer_list timeout;
	struct detect_record server_detect_record;


	int pnum;
//	int status;

//	char name[DDOS_PROFILE_NAME_LEN];

} __attribute__((packed));

struct ddos_skb_attributes 
{
	struct ddos_user_table *user;
	struct ddos_profile_table *profile;
	int profile_num;
};

struct user_hash_key
{
	__be32 saddr;
};

struct ddos_flood_result
{
	struct fld_policy_table *fpt;
	void *current_client;
	char log[64];
	u_int net_type;
	u_int attack_type;
};

void set_flooding_func_and_value(void);

typedef int (*check_client_flood)(struct ddos_profile_table *dpt, struct ddos_user_table *dut);
typedef int (*check_server_flood)(struct ddos_profile_table *dpt, struct pkt_state *sa, struct sk_buff *skb);

struct fld_dyn_link
{
	struct sensitive_table s_table[MAX_SENSITIVE_LEVEL];
	check_client_flood chk_cf;
	check_server_flood chk_sf;
};

#endif  // __FLOODING_DETECTOR_H__
