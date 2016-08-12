#ifndef _FUTURE_GENERAL_H
#define _FUTURE_GENERAL_H

#define VRRP_TABLE_SIZE					32
#define MAX_IFINDEX_COUNT               4096
#define MAX_ETH_COUNT                   NR_ETH  // MAX_ETH_DEV  

#include <linux/timer.h>
#include <linux/net.h>
#include <future/command/print.h>
#include <future/command/debug_print.h>
#include <future/command/debug_check.h>
#include <future/bypass.h>
#include <future/log/logv2.h>


#define MAX_INTERFACE_COUNT				64      // 0-41(eth0~eth41), 42-51(br0~br9), 52-61(bond0~bond9)
#define BRIDGE_BASE_INDEX				42
#define BOND_BASE_INDEX					52
#define UNDEFINED_INTERFACE_INDEX		63


#define MAX_DEVICE_NAME					64
#define MAX_CID_LEN						128
#define MAX_NET_INFO_NUM				MAX_INTERFACE_COUNT

#define SKB_RSRV_ROOM	64

#define FD_CONSOLE  (struct socket *)0

#define __64GB                          65536
#define __48GB                          49152
#define __32GB                          32768
#define __24GB                          24576
#define __20GB                          20480
#define __16GB                          16384
#define __12GB                          12288
#define __8GB                           8196
#define __6GB                           6144
#define __4GB                           4096
#define __2GB                           2048
#define __1GB                           1024
#define __512MB                         512
#define __256MB                         256

#define SRAM_TRAPBUF_SIZE              0x2000

#define LINKPACK_CMD_MAX_LEN            128

#define get_delta_jiffies(a, b) (time_after_eq(a, b)) ? a - b : (ULONG_MAX - b) + a;

enum license_module {
	LICENSE_SYSTEM      = 0,
	LICENSE_NETWORK,
	LICENSE_BASIC,
	LICENSE_FW,
	LICENSE_IPSEC,
	LICENSE_SSL,
	LICENSE_IPS,
	LICENSE_DDOS,
	LICENSE_WAF,
	LICENSE_AV,
	LICENSE_AS,
	LICENSE_TRACKER,
	LICENSE_MAX_MODULES,
	LICENSE_FLAG,
};

enum license_status {            
	LICENSE_NONE	= 0xF,	// 라이선스 없음
	LICENSE_OFF		= 0x0,	// 라이선스가 있고 off 상태
	LICENSE_ON		= 0x1,	// 라이선스가 있고 on 상태
	LICENSE_EXPIRE	= 0x2,	// 라이선스 만료
	LICENSE_TEMP	= 0x4,	// 임시 라이선스 
};

enum measure_status {
	MS_NO_LOG_BIT 		 = 0,
	MS_NO_DYNAMIC_BIT 	 = 1,
	MS_NO_BRIDGE_BIT 	 = 2,
	MS_NO_FAST_BIT 		 = 3,
	MS_NO_FAST_IPSEC_BIT = 4,
};

typedef struct 
{
	int flag;
	int module[LICENSE_MAX_MODULES];
	int boot_module[LICENSE_MAX_MODULES];
	unsigned long long issue[LICENSE_MAX_MODULES];
	unsigned long long expire[LICENSE_MAX_MODULES];

} license_struct;

typedef struct
{
	uint8_t 			ip_type;
	union log_address 	g_mgt_ip;
}g_mgt_struct;

extern license_struct g_license;

extern int debug_level;
extern int MAX_ETH_DEV;

extern char bootserial[25];
extern char g_serial[25];
extern char g_model[64];
extern char g_model_info;
extern char g_device_name[64];
extern char g_ferret_cid[MAX_CID_LEN];
extern char g_cfid[32];
extern uint g_management_log_ip_t;
extern int g_check_serial2hw;
extern int g_bypass_mode;
extern int g_exist_hdd;
extern int g_process;
extern int g_policy_port;                      //policy port
extern int g_ssh_port;                         //ssh port
extern int g_config_cc;
extern int g_cli_log;
extern int g_timezone;
extern int g_ssl_tunnel_cnt;
extern char g_boot_dev_name[64];
extern char g_agent_passwd[32];

extern uint64_t g_rxtx_bandwidth[2][MAX_ETH_COUNT];
extern unsigned long activation_mode;


extern void flush_sram_debug(void);
extern void flush_print_msg(void);
extern void show_cf_debug(struct vty *);
extern void gate_initialize_final(void);
extern void ferret_serial_init(void);
extern void get_model_by_serial(char *data);
extern void write_serial_bootarg(const char *s);
extern int check_ferret_serial(char *data);
extern void serial_encrypt(char *data);
extern void mount_cf(int verbose);
extern int get_license(int type);
extern int get_boot_license(int type);
extern int license_thread(void *arg);
extern void license_expire_check(void);
extern void license_proc_init(void);
extern int license_expire_day(int module);
extern void get_management_log_ip(void);
extern void get_nic_slot_info(void);

#ifdef CONFIG_FERRET_ERFC_LOOKUP
extern int rfc_proc_init(void);
extern int rfc_proc_init2(void);

extern int cv_history_proc_init(void);
extern int current_cv_proc_init(void);
#endif

#ifdef CONFIG_FERRET_HIPAC
extern int hipac_proc_init(void);
#endif

#endif /* FUTURE_GENERAL_H*/
