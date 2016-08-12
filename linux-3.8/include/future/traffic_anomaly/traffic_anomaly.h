#ifndef __TRAFFIC_ANOMALY_H__
#define __TRAFFIC_ANOMALY_H__

#define FLOODING_PASS				0x00
#define FLOODING_ICMP_SIZE_OVER		0x01
#define FLOODING_DOS_ATTACK			0x02
#define FLOODING_DDOS_ATTACK		0x04
#define FLOODING_HTTP_ATTACK		0x08

#define FLOOD_DETECT_NONE			0x00
#define FLOOD_DETECT_SYN_PPS		0x01
#define FLOOD_DETECT_SYN_ACK_PPS	0x02
#define FLOOD_DETECT_ACK_PPS		0x04
#define FLOOD_DETECT_FIN_PPS		0x08
#define FLOOD_DETECT_UDP_PPS		0x10
#define FLOOD_DETECT_ICMP_PPS		0x20
#define FLOOD_DETECT_HTTP_PPS		0x40

#define FLOOD_DETECT_UDP_BPS     	0x80
#define FLOOD_DETECT_ICMP_BPS    	0x100

#define FLOOD_DETECT_SYN_DOD		0x200
#define FLOOD_DETECT_SYN_ACK_DOD	0x400
#define FLOOD_DETECT_ACK_DOD		0x800
#define FLOOD_DETECT_FIN_DOD		0x1000
#define FLOOD_DETECT_UDP_DOD		0x2000
#define FLOOD_DETECT_ICMP_DOD		0x4000
#define FLOOD_DETECT_HTTP_DOD		0x8000

#define FLOOD_DETECT_HTTP_CC_ATTACK 0x10000
#define FLOOD_DETECT_DNS_QUERY_PPS	0x20000
#define FLOOD_DETECT_SQL_QUERY_PPS	0x40000
#define FLOOD_DETECT_DNS_QUERY_DOD	0x80000
#define FLOOD_DETECT_SQL_QUERY_DOD	0x100000

#define DDOS_SIP_CNT				12
#define BPS_PROTO_MAX 				2
#define HTTP_CONF_INDEX				0

// check get_ta_proto_str
enum {
	UDP,
	ICMP,
	SYN,
	SYN_ACK,
	ACK,
	FIN,
	HTTP_GET_REQ,
	DNS_QUERY,
	SQL_QUERY,
	PROTO_MAX
};

enum {
	ATTACK_DETECT,
	ATTACK_BLOCK
};

enum {
	HTTP_USE_PORT,
	DNS_USE_PORT,
	SQL_USE_PORT,
	USE_PORT_MAX
};

struct dos_node_struct {
	uint64_t pps[PROTO_MAX];
	uint64_t bps[PROTO_MAX];

	u_int   		attack_status;
	unsigned long 	jiffies;
	
	int             cc_cnt;
	unsigned long 	cc_jiffies;
};

struct ddos_node_struct {
	uint64_t dod[PROTO_MAX]; // degree of dispersion
	uint64_t pps[PROTO_MAX];
	uint64_t bps[PROTO_MAX];

	u_int ddos_sip[PROTO_MAX][DDOS_SIP_CNT];

	u_int   		attack_status;
	unsigned long 	jiffies;
};

struct ipv6_ddos_node_struct {
	uint64_t dod[PROTO_MAX]; // degree of dispersion
	uint64_t pps[PROTO_MAX];
	uint64_t bps[PROTO_MAX];

	struct in6_addr ddos_sip[PROTO_MAX][DDOS_SIP_CNT];

	u_int   		attack_status;
	unsigned long 	jiffies;
};

struct traffic_anomaly_config {
	int dos_use;
	int dos_action;
	u_int dos_blk_time;
	int icmp_size_threshold;
	uint64_t dos_pps_threshold[PROTO_MAX];
	uint64_t dos_bps_threshold[PROTO_MAX];

	int ddos_use;
	int ddos_action;
	u_int ddos_blk_time;
	int ddos_dod_sensitive;
	uint64_t ddos_dod[PROTO_MAX]; // each degree of dispersion
	uint64_t ddos_pps_threshold[PROTO_MAX];
	uint64_t ddos_bps_threshold[PROTO_MAX];

	int dns_use;
	int dns_action;
	u_int dns_blk_time;

	int sql_use;
	int sql_action;
	u_int sql_blk_time;
};

struct ta_hash_key 
{           
	__be32 daddr;
	__be32 saddr; 
}; 

struct ta_ipv6_hash_key 
{           
	struct in6_addr daddr;
	struct in6_addr saddr;
}__attribute__((packed));

extern struct traffic_anomaly_config traffic_anomaly_conf;

static inline int get_dos_use(void)
{
	return traffic_anomaly_conf.dos_use;
}

static inline int get_dos_action(void)
{
	return traffic_anomaly_conf.dos_action;
}

static inline u_int get_dos_block_time(void)
{
	return traffic_anomaly_conf.dos_blk_time;
}

static inline int get_ddos_use(void)
{
	return traffic_anomaly_conf.ddos_use;
}

static inline int get_ddos_action(void)
{
	return traffic_anomaly_conf.ddos_action;
}

static inline u_int get_ddos_block_time(void)
{
	return traffic_anomaly_conf.ddos_blk_time;
}

static inline int get_http_get_flooding(void)
{
	return traffic_anomaly_conf.dos_pps_threshold[HTTP_GET_REQ]? traffic_anomaly_conf.dos_pps_threshold[HTTP_GET_REQ] :
			traffic_anomaly_conf.ddos_pps_threshold[HTTP_GET_REQ]? traffic_anomaly_conf.ddos_pps_threshold[HTTP_GET_REQ] : 0;
}

void anomaly_ddos_table_alloc(void);

int traffic_anomaly_proc_init(struct proc_dir_entry *p_proc_entry);

int traffic_anomaly_detect_core(struct sk_buff *skb, int *detect_proto, int http_type);

int dos_hashtable_chk(int http_use);

void set_http_conf(void *http_config);

u_int get_block_time(int type, int detect_type);

int get_http_use(void);
int get_http_action(void);
int get_server_rst_use(void);
int get_pkt_anomaly_use(void);
int get_tcp_window_use(void);
int get_post_hdr_use(void);
int get_cc_attack_use(void);
int get_http_port_use(void);
int get_dns_use(void);
int get_dns_action(void);
u_int get_dns_block_time(void);
int get_sql_use(void);
u_int get_sql_block_time(void);
int get_sql_action(void);

u_int get_http_blk_time(void);
u_int get_pkt_size(void);
u_int get_pkt_cnt(void);
u_int get_pkt_interval(void);
u_int get_request_timeout(void);
u_int get_win_size(void);
u_int get_zero_win_cnt(void);
u_int get_content_len(void);
u_int get_post_param_cnt(void);
u_int get_detect_cnt(void);
u_int get_detect_interval(void);
int get_traffic_anomaly_status(void);
int get_traffic_anomaly_use(void);
void set_traffic_anomaly_use(int);

int chk_use_port(int port_idx, __be16 port);
void add_use_port(int port_idx, __be16 port);
void init_use_port(void);

#endif
