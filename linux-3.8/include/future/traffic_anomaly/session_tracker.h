#ifndef __SESSION_TRACKER_H__
#define __SESSION_TRACKER_H__

#include <uapi/linux/if_ether.h>

enum    
{       
	SESSION_STATE_CONN_EST          = 0x01,
	SESSION_STATE_AB_CONN_EST       = 0x02,

	SESSION_STATE_CLI_REQ           = 0x04,
	SESSION_STATE_AB_CLI_REQ        = 0x08,

	SESSION_STATE_SVR_HEADER        = 0x10,
	SESSION_STATE_STATELESS         = 0x20,
	SESSION_STATE_WRONG_COMBI       = 0x40,
	SESSION_STATE_GOT_HEADER        = 0x80,

	SESSION_STATE_CLOSE_OPTION    	= 0x100,
	SESSION_STATE_SVR_RST           = 0x200,

	SESSION_STATE_LEGAL_USER        = 0x1000,
	SESSION_STATE_NORMAL_REQ        = 0x2000,
	SESSION_STATE_INCOMPLITE_REQ    = 0x4000,
	SESSION_STATE_AB_REQUEST        = 0x8000,

	SESSION_STATE_GET_REQ           = 0x10000,
	SESSION_STATE_CC_REQ            = 0x20000,

	SESSION_STATE_OK                        = 0x100000,
	SESSION_STATE_ERR_INCOMPLETE            = 0x200000,
	SESSION_STATE_ERR_POST_ATTACK           = 0x400000,
	SESSION_STATE_ERR_SMALL_PKT_ATTACK      = 0x800000,
	SESSION_STATE_ERR_CONTENT_LEN_ATTACK    = 0x1000000,
	SESSION_STATE_ERR_AB_WIN_ATTACK         = 0x2000000,
	SESSION_STATE_ERR_PARAM_ATTACK          = 0x4000000,
	SESSION_STATE_KV_CLI_REQ				= 0x8000000,

	SESSION_STATE_MAX		                = 0x10000000
}; 

#define is_attack(x) ((x >= SESSION_STATE_ERR_INCOMPLETE) && (x < SESSION_STATE_MAX))

struct mac
{
	unsigned char	h_dest[ETH_ALEN];	/* destination eth addr	*/
	unsigned char	h_source[ETH_ALEN];	/* source ether addr	*/
	char			if_name[IFNAMSIZ];
	char			dst_if_name[IFNAMSIZ];
} __attribute__((packed));


struct tuple_info 
{
	__be32 saddr;
	__be32 daddr;
	__be16 s_port;
	__be16 d_port;
	__u8   protocol;
};

struct session_list_node 
{ 
	struct list_head list; 
	struct tuple_info session_info;
	struct mac mac_info;
	unsigned long key; 
	unsigned long session_id; 
	unsigned int pkt_len;
	int last_con_time;
	int ab_pkt_cnt;
	int ab_session_cnt;
	u_int8_t dir;
	char is_fin_sent;
};

int validate_packet(struct sk_buff *skb, struct omni_track *ot);
int anomaly_detect_thread(void *arg);

#endif
