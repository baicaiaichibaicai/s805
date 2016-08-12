#ifndef _FUTURE_MA_H
#define _FUTURE_MA_H

#include <linux/in.h>
#include <linux/if.h>
#include <future/object.h>

#define SECOND          *HZ
typedef struct {
    int version;
    obj_id_struct gate_uuid;
    char gate_name[64];
    short time_zone;
} __attribute__((packed)) gate_base_object;

/******************Gate status info********************/
typedef struct
{
	u_int               link_on         ;       // 0: Link fail, 1: Link Up
	u_int               link_speed      ;       // 10: 10M, 100: 100M
	u_int               link_duplex     ;       // 0: Half 1: Full
	u_int               rx_pkt_count    ;
	u_int               rx_tcp_count    ;
	u_int               rx_udp_count    ;
	u_int               rx_icmp_count   ;
	u_int               tx_pkt_count    ;
	u_int               tx_tcp_count    ;
	u_int               tx_udp_count    ;
	u_int               tx_icmp_count   ;
	u_int               spoof_packet    ;
	u_int               utilization     ;
} ether_status_struct;  

typedef struct          // Packet Filtering Module
{
	u_int       rx_packet_count ;
	u_int       tx_packet_count ;
	u_int       accepted_pkt_count;
	u_int       dropped_pkt_count;
	u_int       rejected_pkt_count;
	u_int       total_session   ;
	u_int       active_session  ;
} pf_status_struct;

typedef struct
{
	u_int       user_auth_success;
	u_int       user_auth_fail  ;
	u_int       user_hash_error ;
	u_int       user_misc_error ;
	u_int       isakmp_kt_count ;
	u_int       isakmp_sa_count ;
	u_int       isakmp_kt_error ;
	u_int       isakmp_timeout  ;
	u_int       isakmp_comm_error;
	u_int       ipsec_enc_success;
	u_int       ipsec_enc_fail  ;
	u_int       ipsec_dec_success;
	u_int       ipsec_dec_fail  ;
} auth_status_struct;

typedef struct          
{       
	u_int               version ;       // 1 or 2
	ether_status_struct eth[MAX_NET_INFO_NUM];
	pf_status_struct    pf      ;
	auth_status_struct  auth    ;
} __attribute__((packed)) vgate_info_struct;



typedef struct {
    obj_id_struct c_uuid;
    char center_name[60];
	struct in6_addr c_ip6[2];
    u_int dr_c_ip;
    u_int c_ip[2];
    u_short mac_port;
    u_short sec_port;
    u_int enc_algo;
    u_int auth_algo;

} __attribute__((packed)) host_info;

typedef struct {
    int center_use;
	int center_type;
	gate_base_object gate_base;
	host_info   hinfo;
	mk_token mk;
	u_int center_ip;	// 실제 통신하고 있는 center ip 
} __attribute__((packed)) appnet_gate;

typedef struct
{
	int         sys_status;             // System Init Status
	// Inited, Keyed, Uninited
	int         err_status;             // System Error Status
	// Normal, Warning, Fatal

	int         pri_sec;                // Primary or Secondary Center(0, 1)
	volatile int c_sts;                 // Center Status
	volatile int ifval;                 // Center is connected to this Interface
	// Else ifval is -1;
	int         obj_loaded;             // Whether Object is loaded into DRAM
	int         obj_count;              // Loaded Object Count
	int         obj_size;               // Loaded Object Size  : 32 Byte

	struct timer_list t_cfind;          // Timer for Center Find  : 28 Byte
	u_int       last_send_spi;          // Last Sended SPI(Request)
	u_int       last_recv_spi;          // Last Received SPI(Response)
	u_int       sock_index;             // Last Received Socket Index (si)

	int         thread_enabled;
	int         reject_packet;          // Center Inform This
	int         boot_count;             // System Boot Count

	int         mflag;                  // Misc. Flag           00/11/16
	int         login_status;           // login status
	int         cmd_timeout;            // Command timeout
	int         l2tp_timeout;           // l2tp timeout
	u_short     max_syn_count;          // Max Syn Count        00/11/16
	u_short     max_ping_count;         // Max Ping Count       00/11/16
	u_short     syn_drop_timeout;       // Syn Drop Timeout     00/11/16
	u_short     ping_drop_timeout;      // Ping Drop Timeout    00/11/16  : 48
	u_short     fw_fcs;                 // 2003.03.20 by konan
	u_short     obj_fcs;                // 2003.03.20 by konan
#ifdef __BIG_ENDIAN_BITFIELD
	__u8        revd_int        : 6,
				obj_int         : 1,
				fw_int          : 1;
#else
	__u8        fw_int          : 1,
				obj_int         : 1,
				revd_int        : 6;
#endif
	char        log_full;
	char        rsvd[6];        // 8 -> 6
} ma_info_struct;

extern appnet_gate cinfo;
extern ma_info_struct mainfo;

#endif/*_FUTURE_MA_H*/
