#ifndef __FUTURE_NETIF_H__
#define __FUTURE_NETIF_H__

#include <linux/in.h>
#include <linux/in6.h>

#include <future/general.h>



#define HA_ACTIVE_STATE					1
#define HA_STANDBY_STATE				2

/* VRRP state machine -- rfc2338.6.4 */
#define VRRP_STATE_INIT 1       /* rfc2338.6.4.1 */
#define VRRP_STATE_BACK 2       /* rfc2338.6.4.2 */
#define VRRP_STATE_MAST 3       /* rfc2338.6.4.3 */
#define VRRP_STATE_NONE 99      /* internal */

#define HA_STANDBY_MODE_NONE				0
#define HA_STANDBY_MODE_MAIN				1
#define HA_STANDBY_MODE_SUB					2

/* Future FOS20 define */
//#define RB_SET_STATE					0x0001
//#define SERIAL_SET_RANGE				0x0002
//#define SERIAL_SET_STATE				0x0004
//#define SERIAL_GET_STATE				0x0008
#define VPN_GET_STATE           		0x0040
//#define INTERNET_SET_STATE			0x0100
#define ROUTE_SET_SIGNAL				0x0200
//#define ROUTE_CLR_SIGNAL				0x0400
#define ROUTE_SET_BACKUP				0x0800

#define ZEBRA_REPORT_IPCHANGED 			0x1000
//#define LINELB_REPORT_IPCHANGED 		0x1000
//#define LINELB_REPORT_UPDOWN			0x2000
//#define LINELB_REPORT_ENABLE			0x4000
//#define LINELB_REPORT_DISABLE			0x8000
//#define LINELB_REPORT_LLB_INTERFACE	0x8100

#define PROXY_CHECKER_DOWN              0x8200
#define PROXY_CHECKER_UP                0x8201

#define VRRP_SET_VIPTABLE				0xa000
#define VRRP_DEL_VIPTABLE				0xa001
#define VRRP_INIT_VIPTABLE              0xa002
#define VRRP_VPN_DECIDE                 0xa003

#define FAILOVER_SET_RUNSTATE			0xb002
#define GATEVIP_SET_VAL					0xb003
#define ADD_MCAST_GROUP_IP				0xb004
#define DEL_MCAST_GROUP_IP				0xb005
#define SERIAL_NOTIFY_SEND				0xb006
//#define SERIAL_NOTIFY_DISABLE			0xb007
#define FAILOVER_GET_VIP				0xb008
#define FAILOVER_GET_ALL_INTERFACE		0xb010
#define FAIlOVER_CHECK_VRRP_STATE       0xb030

//#define SET_PSTN_IPSEC_BY_PASS			0xc001
//#define SET_PSTN_CENTER_PRI_IP			0xc002
//#define SET_PSTN_SYSLOG_PRI_IP			0xc003

//#define SESSION_GUARANTEE				0xD000
#define CHECKER_REPORT_UP               0xD001
#define CHECKER_REPORT_DOWN             0xD002
#define CHECKER_REPORT_DISABLE          0xD003
#define CHECKER_REPORT_ENABLE           0xD004

#define INSTALL_PROXY_VIP               0xD005 
#define UNINSTALL_PROXY_VIP             0xD006

#define HA_MODE_MASTER					0xD007
#define HA_MODE_BACKUP					0xD008
#define HA_MODE_BRIDGE					0xD009
#define HA_MODE_NONE					0xD00A


#define HA_MODE_BRANCH_MAIN             0xD00B      // ioctl 용으로 사용되지 않음
#define HA_MODE_BRANCH_SUB              0xD00C      // ioctl 용으로 사용되지 않음 
#define HA_CHECK_SWITCH_TRUNK           0xD00D      // ioctl 용으로 사용되지 않음 

#define HA_MODE_VRRP_MASTER             0xD00E		// 사용되지 않는 것으로 보임.
#define HA_MODE_VRRP_BACKUP             0xD00F		// 사용되지 않는 것으로 보임.

#define HA_MODE_KEEP_SYNCHRONOUS        0xD010
#define HA_MODE_KEEP_ASYNCHRONOUS       0xD011
#define HA_MODE_KEEP_SETTING            0xD012
#define HA_MODE_KEEP_CHANGING           0xD013
//#define HA_MODE_KEEPSTATE             0xD013      // KEEP_CHANGING..
#define HA_MODE_CHECKER					0xD014
#define HA_MODE_VRRP					0xD015

#define FUTURE_FMAS_MAINFO              0xe1000001
#define FUTURE_FMAS_CINFO               0xe1000002
#define GET_LOCAL_INTERFACE_STATE       0XD100
#define GET_MPR_INTERFACE_STATE       	0XD101


#define GET_BOOT_STATUS					0XFFF1


#define CMD_GET_GATEINFO                0x00000003 

/* Future UTM20 define */
#define FUTURE_NETIF_TEST				0xeeeeeeee

#define FUTURE_IPS_BLOCK_SESSION_LIST	0xe0000001
#define FUTURE_IPS_BLOCK_SESSION_DEL	0xe0000002
#define FUTURE_IPS_TOP					0xe0000003
#define FUTURE_FMAS_MAINFO     		 	0xe1000001
#define FUTURE_FMAS_CINFO      		 	0xe1000002
#define FUTURE_FMAS_MKINFO       		0xe1000003
#define FUTURE_FMAS_SYSTEM_INFO			0xe1000004
#define RTM_SA1_TUNNEL_INFO				0xe1000005
//#define RTM_REAL_LOG_REQ                0xe1000006 disable
#define RTM_REAL_LOG_REQ                0xe1000007
#define RTM_NMS_TUNNEL_INFO             0xe1000008
#define FUTURE_NETIF_INTERFACE_USE		0xf0000002
#define FUTURE_NETIF_FLOGS_PID          0xf0000003
#define FUTURE_NETIF_PRESS_TYPE			0xf0000004
#define FUTURE_NETIF_FDEBUG_PID			0xf0000005
#define FUTURE_NETIF_FDEBUG_MSG			0xf0000006
#define FUTURE_NETIF_LOG_DETAIL			0xf0000007
#define FUTURE_NETIF_SIMS_INFO			0xf0000008
#define FUTURE_NETIF_SIMS_FW            0xf0000009
#define FUTURE_NETIF_SIMS_IPS           0xf000000a
#define FUTURE_NETIF_LOG_BASE	        0xf000000b
#define FUTURE_FW_POLICY_STAT_REQ		0xff000001
#ifdef CONFIG_FUTURE_PKI
#define FUTURE_PKI_REQ					0xff000002
#define FUTURE_PKI_RCV					0xff000003
#define FUTURE_PKI_CHECK				0xff000004
#define FUTURE_PKI_SAVE					0xff000005
#endif
#define FUTURE_LICENSE					0xff000006
#define FUTURE_BLOCK_SESS               0xff0000ff
#define FUTURE_NETIF_LOG_KEY			0xff000100
enum {
	CHECKER_TYPE_NORMAL = 0,
	CHECKER_TYPE_PROXY_PRIMARY,
	CHECKER_TYPE_PROXY_BACKUP,
	CHECKER_TYPE_PROXY,
	CHECKER_TYPE_LINELB,
	CHECKER_TYPE_SERIAL_STATUS,
	CHECKER_TYPE_HA_LINK,
	CHECKER_TYPE_STANDBY,
	CHECKER_TYPE_VPN,
	CHECKER_TYPE_HA_INFO,
	CHECKER_TYPE_PROXY2,
	CHECKER_TYPE_ROUTE,
#ifdef CONFIG_XTM_TODO
	CHECKER_TYPE_FUTURE_ICMP,
#endif
	CHECKER_TYPE_MAX
};

#define CHECKER_DEV_NAME_LEN		16	
#define CHECKER_TAG_NAME_LEN		16
#define CHECKER_TYPE_STR_LEN		32

struct checker_iface_info {
	struct in_addr              ip1;
	struct in_addr              ip2;
	struct in6_addr             ip6_1;
	struct in6_addr             ip6_2;
	char                        dev[CHECKER_DEV_NAME_LEN];
	char                        mac[6];
	char                        status;
	char                        ip_type;
	char                        tag_name[CHECKER_TAG_NAME_LEN];
};

struct checker_info {
	long long                   interface_flag;
	char                        type_str[CHECKER_TYPE_STR_LEN];
	struct checker_iface_info   list[MAX_INTERFACE_COUNT];
}; 

struct aa_checker_info {
	long long 					local_interface_flag;
	long long 					remote_interface_flag;
	int       					priority;
};
struct vrrp_table {
	struct in_addr vip;
	int  vid;
	int  status;
	int  priority;
	char dev[16]; 
};

struct fnetifreq {
	struct sockaddr 	addr;
	char 				dev[CHECKER_DEV_NAME_LEN];
	int 				status;
	int 				llb_count;
	long long 			llb_interface;
	int                 ip_type; 
	struct sockaddr_in6 addr6;
};


struct fnetifreq2 {
	struct in_addr      ip1;
	struct in_addr      ip2;
	struct in6_addr     ip6_1;
	struct in6_addr     ip6_2;
	char                dev[CHECKER_DEV_NAME_LEN];
	char                mac[6]; 
	char                status;
	char				ip_type; 
	char                tag_name[CHECKER_TAG_NAME_LEN];
	char                iface;
	char                type;
	char				rvd[2];
	int					priority;
	long long           interface_flag;
} 
#ifdef CONFIG_X86_64
__attribute__((packed aligned(sizeof(int))))
#endif
	;



extern int g_ha_setup, g_ha_link_status;
extern int g_ha_mode;
extern int g_ha_state;
extern int g_install_primary_ip_flag, g_install_backup_ip_flag;
extern int g_ha_priority, g_ha_peer_priority;
extern u64 g_proxy_checker_status;


extern struct checker_info checker[CHECKER_TYPE_MAX];
extern struct aa_checker_info g_aa_checker_info;
extern struct vrrp_table vrrp_table[VRRP_TABLE_SIZE];



char *get_ha_mode_string(void);

#endif
