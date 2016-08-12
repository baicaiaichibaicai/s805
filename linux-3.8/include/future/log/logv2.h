#ifndef _LOGV2_H_ 
#define _LOGV2_H_ 

#ifndef _FERRET_LOG_FUNC_H_
#include <linux/types.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <future/log/logevent.h>
#include <future/omni/user/auth_user.h>
#endif
#define MMAPNOPAGE_DEV_NAME		"/dev/log_mmap"
#define MMAPNOPAGE_DEV_MAJOR	240

//odroid
//#define SHARE_MEM_PAGE_COUNT	100000	// TODO. 모델 별로 스펙 제어
#define SHARE_MEM_PAGE_COUNT    1000  // TODO. 모델 별로 스펙 제어

#define SHARE_MEM_SIZE			(PAGE_SIZE*SHARE_MEM_PAGE_COUNT)

#define LOG_BASE_DIR 	"/ferret/log/klog/"
#define APP_LOG     	"/proc/ferret/log/klog"
#define KERN_IPS_AUDIT_DATA		"/ferret/log/audit"
#define FLOGS_CONF		"/etc/ferret/conf/log/flogs.conf"
#define LOG_MSG_SIZE	sizeof(log_config_struct)
#define LOG_TH_WRITE_PERCENT    10
#define LOG_TH_WAKEUP_PERCENT   70
#define LOG_FLUSH_TIMEOUT       1 // 10 -> 1
#define LOG_TIMEOUT             1
#define LOG_VERSION				4

#define LOG_LEV_BIT     	12
#define LOG_LEV_MASK    	0x0000f000 
#define LOG_CODE_BIT    	24
#define LOG_CODE_MASK   	0xff000000 
#define LOG_MODULE_BIT    	28
#define LOG_MODULE_MASK   	0xf0000000

#define LOG_LEV_MISMATCH	-1
#define LOG_DESC_LEN		128
#define MAX_EXTEND_LEN		304
#define MAX_AUDIT_DATA_SIZE 3500 

#define LOG_SYNC_BIND		1
#define LOG_SYNC_RELESE		2

#define SERVER_INDEX            3

#define SMC_LOGSERVER       1
#define WEGUARDIA_LOGSERVER 0

// syslog type
#define LOGSERVER2          0
#define WELF                1
#define SYSLOG              3

// Sync log mode
#define M_MODE      0x01
#define S_MODE      0x02    
#define MM_MODE     0x03 

#define LOG_EVENT_MAX		150

// fw acct log
#define ACCT_KILO  1000
#define ACCT_MEGA  1000000
#define ACCT_GIGA  1000000000
#define ACCT_TERA  1000000000000
#define ACCT_PETA  1000000000000000

typedef struct 
{   
	char system_lev;
	char network_lev;
	char vpn_lev; 
	char ssl_lev; 
	char ips_lev; 
	char ddos_lev;
	char waf_lev;
	char av_lev;
	char as_lev;
}log_lev_struct;

/*
typedef struct {
	u_int   hdd_save;
	u_int   send_mail; 
	u_int   send_alert;
	u_int   syslog;
	u_int	smc_log;
	char    ename[64];
}log_table_data_struct;
*/

typedef struct {
	unsigned char	hdd_save;
	unsigned char	mail;
	unsigned char	send_alert;
	unsigned char	syslog;
	unsigned char	smc_log;
	unsigned char	loglev;
	unsigned char	rvd2;
	unsigned char	rvd3;

}log_setting_struct;

typedef struct {
	union {
		long	flag;
		log_setting_struct set;
	}log;
	char    ename[64];
}log_table_data_struct;

typedef struct {
	u_int   event;
}log_table_key_struct;

#if 0
typedef struct
{
	unsigned int      use_cpu_full          :1,
			  use_mem_full          :1,
			  use_hdd_alarm         :1,
			  hdd_alarm_val         :1,
			  hdd_alarm_type        :1,
			  use_sess_full         :1,
			  use_log_remove        :1,
			  use_integrity_fail    :1,
			  use_ike_fail          :1,
			  use_change_obj        :1,
			  use_apply_obj         :1,
			  use_system_boot       :1,
			  use_login_success     :1,
			  use_login_fail        :1,
			  use_system_stop       :1,
			  use_system_mb         :1,
			  use_system_bm         :1,
			  use_line_faulty		:1,
			  use_ips_detect        :1,
			  use_ips_block         :1,
			  use_dos_deny          :1,
			  use_dos_find          :1,
			  use_ddos_deny         :1,
			  use_ddos_find         :1,
			  use_pow1_stat         :1,
			  use_pow2_stat         :1,
			  use_del_turnel        :1,
			  use_cre_turnel        :1;
}alert_mail_struct;
#endif

typedef struct
{
	short       logserver_flag;
	u_short     logserver_port;
	int			logserver_ipsec;
	char        logserver_addr[64];

	short 		st_server_cnt;
	short		st_server_num[SERVER_INDEX];
	short       st_server_use[SERVER_INDEX];
	short       st_server_type[SERVER_INDEX];
	u_short     st_server_port[SERVER_INDEX];

	char        st_server_address[SERVER_INDEX][64];

	short       st_server_case[SERVER_INDEX];
	short		st_server_ipsec[SERVER_INDEX];

}syslog_server_struct;

typedef struct
{
	u_int use;
	u_short mode;
	u_short port;
	char iface[16];
	char ip[64];
	char m_name[64];
	char s_name[64];
} sync_log_struct;

typedef struct
{
	uint64_t timer;
	char iface[16];		// Interface	
	short cnt;		// Count
	short sec;		// Second
	short occ_cnt;		// occurrence count
	uint8_t dumy[10];

} threshold_iface;

typedef struct
{
	int flogs_rev;
	int use_hdd_log;
	int timezone;
	int freeze;

	int session;
	int freeze_val;

	int use_smc_syslog;
	int use_rtm;

	char admin_id[64];
	char admin_pw[64];
	char rtm_address[32];
	u_int iface_info[8];

	int work_network_cnt;
	int work_ip_range[10][2];
	sync_log_struct sync_log;
	syslog_server_struct    syslog_server;
	threshold_iface			threshold[8];
	char resv[120];
}__attribute__((packed)) log_base_struct;

/* log type */
enum LOG_TYPE {
	SYSTEM_LOG 	= 0x1,
	NETWORK_LOG = 0x2,
	FW_LOG		= 0x3,
	VPN_LOG		= 0x4,
	SSL_LOG		= 0x5,
	IPS_LOG		= 0x6,
	DDOS_LOG	= 0x7,
	WAF_LOG		= 0x8,
	AV_LOG		= 0x9,
	AS_LOG		= 0xa,
	GARBAGE_LOG	= 0xf,
};

enum EXTEND_TYPE {
	SYSTEM_EXT_LOG 	= 0x1,
	NETWORK_EXT_LOG = 0x2,
	FW_EXT_LOG		= 0x3,
	VPN_EXT_LOG		= 0x4,
	SSL_EXT_LOG		= 0x5,
	IPS_EXT_LOG		= 0x6,
	DDOS_EXT_LOG	= 0x7,
	WAF_EXT_LOG		= 0x8,
	AV_EXT_LOG		= 0x9,
	AS_EXT_LOG		= 0xa,
	MAX_EXT_LOG     = 0xf,
};

enum
{       
	LOGLEV_CRITICAL     = 0x1,
	LOGLEV_SERIOUS      = 0x2,
	LOGLEV_WARNING      = 0x3,
	LOGLEV_NORMAL       = 0x4,
	LOGLEV_INFO         = 0x5,
};

enum LOG_ACTION {
	LOG_ACCEPT	= 0x01,
	LOG_DROP	= 0x02,
	LOG_DETECT	= 0x03,
	LOG_APPROVE	= 0x04,
};

enum profile_status {
	PROFILE_APP_BIT = 0,
	PROFILE_APP = (1 << PROFILE_APP_BIT),

	PROFILE_HTTP_BIT = 1,
	PROFILE_HTTP = (1 << PROFILE_HTTP_BIT),

	PROFILE_RPC_BIT = 2,
	PROFILE_RPC = (1 << PROFILE_RPC_BIT),

	PROFILE_IPSEC_BIT = 3,
	PROFILE_IPSEC = (1 << PROFILE_IPSEC_BIT),

	PROFILE_IPS_BIT = 4,
	PROFILE_IPS = (1 << PROFILE_IPS_BIT),

	PROFILE_DDOS_BIT = 5,
	PROFILE_DDOS = (1 << PROFILE_DDOS_BIT),

	PROFILE_AV_BIT = 6,
	PROFILE_AV = (1 << PROFILE_AV_BIT),

	PROFILE_AS_BIT = 7,
	PROFILE_AS = (1 << PROFILE_AS_BIT),
};

union log_address {
	uint32_t	ip;
	uint32_t	ip6[4];
};

typedef struct _log_addr_param
{
	union log_address addr;
	int family;
} log_addr_param;

typedef struct
{
	char user[64];
} log_ext_system;

typedef struct
{
	uint32_t	spd_id;
} log_ext_network;

enum log_ext_fw_type {
    LOG_EXT_FW_CREATE = 0,
    LOG_EXT_FW_CLOSE
};

typedef struct
{
#if 0
	uint32_t	retrans_count;
	uint32_t	profile_num;
	char 		host[64];
	uint32_t	res_type;
	uint32_t	url_category;
	uint64_t    in_packet;
	uint64_t    out_packet;
	uint64_t    in_bytes; 
	uint64_t    out_bytes;

	uint32_t	spd_id;
	uint32_t	nat_id;
	uint16_t	type;
	uint16_t	action;
#else
	uint8_t 	url[64];
	uint8_t 	userid[32];
	uint32_t	url_category;
	uint32_t	retrans_count; 		// 재전송
	uint64_t    in_packet;
	uint64_t    out_packet;
	uint64_t    in_bytes; 
	uint64_t    out_bytes;
	uint32_t	spd_id;				// 필터링 정책ID
	uint32_t	nat_id;				// NAT 정책ID
	uint16_t	status;				// 상태 (close, ..)
	uint16_t	action;				// 행위 (accept, drop, detect, appove)
	uint32_t	holding_time;		// 유지시간 
	uint32_t 	sid;				// application 이름  
	uint32_t	profile_num;		// 프로파일 
#endif
} log_ext_fw;

typedef struct
{
	char		user[64];
	int			tunnel_id;			// 터널 ID
	uint64_t    in_packet;			// rx packet cnt
	uint64_t    out_packet;			// tx packet cnt
	uint64_t    in_bytes;			// rx packet byte
	uint64_t    out_bytes;			// tx packet byte
	uint16_t 	action; 			// 행위 ( allow, deny )
} log_ext_ssl;

typedef struct
{
	char		profile_name[32];		// 프로파일 이름
	char		file_name[20];			// 바이러스 검사된 파일
	char		virus_info[32];			// 바이러스 정보(위협정보, 바이러스 이름)
	char		sub_desc[32];			// 상세정보
	uint16_t	profile_id;				// 프로파일 ID
	uint16_t	action;					// 차단 : 0, 탐지 : 1, 통과 : 2
} log_ext_av;

typedef struct
{
	char	 	profile_name[32];		// 프로파일 이름
	char		file_name[20];			// 스팸 대상 첨부 파일
	char		spam_info[32];			// 스팸 기능에 감지된 위협
	char		sub_desc[32];			// 상세정보
	char		sender[32];				// 발신자 메일주소
	char		receiver[32];			// 수신자 메일주소
	uint16_t	profile_id;				// 프로파일 ID
	uint16_t	action;					// 차단 : 0, 탐지 : 1, 통과 : 2
} log_ext_as;

typedef struct
{
	uint64_t audit_log_id;
	uint64_t track_id;			// track id
	uint64_t block_byte;		// 차단 바이트 수
	uint32_t spd_id;			// 필터링 정책 ID
	int profile_id;				// ips 프로파일 ID
	int ips_group_id;			// 그룹ID
	int action;					// alert, drop
	int block_type;				// 차단 유형(1:1, 1:N, N:1 등 .....)
	int block_time;				// 차단 시간
	int block_count; 			// 차단 패킷 수
	int priority;				// 위험도

	u_int ips_start_time;		// 탐지,차단 시작 시간
	u_int ips_end_time;			// 탐지,차단 마지막 시간

	char pattern_name[96];		// 시그너처이름
	uint32_t sid;				// fsid
	uint16_t pattern_name_size;	// 시그너처 이름 길이

} log_ext_ips;

typedef struct
{
	uint64_t audit_log_id;
	uint64_t track_id;			// track id
	uint64_t block_byte;		// 차단 바이트 수
	uint64_t detect_byte;		// 탐지 바이트 수

	u_int ddos_start_time;		// 탐지, 차단 시작시간
	u_int ddos_end_time;		// 탐지, 차단 마지막 시간

	int profile_id;				// ddos 프로파일 ID
	int action;					// 탐지, 차단
	int attack_type;			// 공격 type
	int attack_pps;				// 공격 pps
	int block_type;				// 차단 유형(1:1, N:1)
	int block_time;				// 차단 시간
	int block_count;			// 차단 패킷 수

} log_ext_ddos;

typedef struct 
{
	char name[64];
} ipsec_name_info;

typedef struct
{
	uint64_t inpacket;	// 패킷(RX)
	uint64_t outpacket; // 패킷(TX)
	uint64_t inbyte;	// 사용량(RX)
	uint64_t outbyte;	// 사용량(TX)
	ipsec_name_info serial;	// 시리얼 넘버
	ipsec_name_info device; // 대상 장비
} log_ext_vpn;

typedef struct
{
	unsigned long jiff;
	uint64_t in_packets;
	uint64_t out_packets;
	uint64_t in_bytes;
	uint64_t out_bytes;
	uint32_t url_category_idx;
	char 	url[128];
	char	desc[LOG_DESC_LEN];
} log_ext_fw_web; //for webfilter log

typedef struct
{
	uint32_t	version;
	uint32_t	event;

	uint32_t	timestamp;
	uint32_t	microtime;

	union log_address src;
	union log_address dest;
	union log_address nat_src;
	union log_address nat_dest;

	char		desc[LOG_DESC_LEN];

	uint16_t	protocol;
	uint8_t     ip_type;
	uint8_t     direction;
	uint16_t	packet_id;
	uint16_t    tcp_flag;

	uint16_t	src_port;
	uint16_t	dest_port;
	uint16_t	nat_src_port;
	uint16_t	nat_dest_port;

	char		iface[16];

	union {
		uint8_t	        dummy[MAX_EXTEND_LEN];
		log_ext_system	system;
		log_ext_network	network;
		log_ext_fw		fw;
		log_ext_vpn		vpn;
		log_ext_ssl		ssl;
		log_ext_ips		ips;
		log_ext_ddos	ddos;
		log_ext_av		av;
		log_ext_as		as;
	};

	uint32_t        extend_type; // EXTEND_TYPE enum 참조, 없는건 추가해서 사용
	char			name[32];
}__attribute__((packed)) log_param_struct;

typedef struct
{
	union {
		long	flag;
		log_setting_struct set;
	}log;
	log_param_struct log_param;
}__attribute__((packed)) log_config_struct;

struct audit_log_node{
	u_int timestamp;
	u_int microtime;
	int index;
	int size;
	char data[MAX_AUDIT_DATA_SIZE];
};

extern log_lev_struct log_lev;
extern log_table_data_struct log_table_data[LOG_EVENT_MAX];
extern log_base_struct log_base;
extern log_table_key_struct log_table_key;
extern char *log_share_memory;


int chk_log_config(log_config_struct *log_config, u_int event, struct omni_filter *omnifilter);
void log_parse(char *buf);
void read_flogs_config(void);
int thread_logd(void *);
int log_syncd(void *);
void fla_init(void);

/*vty prototypes*/
struct vty *vty_new(void);
int display_fla_meminfo(struct vty *vty);

extern u_int get_log_use(void);

#ifdef CONFIG_FERRET
#ifndef _FERRET_LOG_FUNC_H_
void add_log(char *buff);
void garbage_log(u_int event, int log_ext_type, char *desc);
void system_log_general(struct sk_buff *skb, char *iface, uint sip, char *user, u32 event, char *desc);
void system_log(u_int event, char *desc);
void cli_command_log(u_int sip, char *user, u32 event, char *desc);
void network_log(u_int spd_id, log_addr_param *saddr, log_addr_param *daddr, u_int event, u_char *smac, u_char *dmac, char *dev, char *desc);
void ips_log(u_int event, void *otn ,void *packet, struct sk_buff *skb);
void ips_block_close_log(u_int event, uint64_t bytes, int count, uint64_t block_key, struct omni_track *track);
void ips_spoofing_log(u_int event, struct sk_buff *skb, char *desc);
void ddos_log(u_int event, void *packet, char *desc, int action, u_int block_time, u_int sec_time, u_int usec_time);
void ddos_block_close_log(u_int event, uint64_t bytes, int count, uint64_t block_key);
void app_log(u_int event, struct omni_track *track, u_short id, char dir, const char *iface, char *desc, int flag);
void http_log(u_int event, struct omni_track *track, u_short id, char dir, const char *iface, u_int app, char *desc);
void user_log(u_int event, u_int rescode, const char *msg, struct auth_user *user);
void fw_log(u_int event, struct omni_track *track, uint16_t id, char dir, const char *iface, __u32 app,char *desc,log_ext_fw_web *fw_web_log, struct omni_filter *filter);
void vpn_log(u_int event,  struct omni_track *track, char *desc, struct iphdr *iph);
void ssl_log(u_int event, char* user, char *desc);
void l2_flooding_log(u32 event, u8 log_type, struct sk_buff *skb, const char *d);
void av_log(u_int event, u_int profile_id, u_int action, char *desc);
void as_log(u_int event, u_int profile_id, u_int action, char *desc);
int init_active_log_sock(int type);
void fw_none_policy_log(u_int event, struct omni_track *track, uint16_t id, char dir, char *desc, unsigned char op_mode);
#endif
#endif
#ifdef CONFIG_FERRET_IP_MANAGE
int ip_manage_monitor_proc_init(void);
#endif

#endif /* _LOGV2_H_ */
