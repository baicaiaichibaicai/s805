#define REALTIME_SESS_PAYLOAD_LEN 20

#define IPPROTO_ANY 0

#define DIR_FORWARD		1
#define DIR_BACKWARD	2
#define DIR_BOTH		3

#define RPM_DROP		0
#define RPM_ACCEPT		1
#define RPM_IPSEC		2
#define RPM_WHITE		3
#define RPM_BLACK		4

#define RPM_READ_SIZE	100

typedef struct
{
	u_int			sip;
	u_int			dip;
	u_int			nat_sip;
	u_int			nat_dip;
	u_short			sport[2];
	u_short			dport[2];
	u_short			nat_sport;
	u_short			nat_dport;

	unsigned int	timeout;
	unsigned int	policy_num;

	unsigned int	nat_policy_num;
	unsigned short	length;
	char			protocol;

	char			ppp_flag	: 1,
					iface		: 7;
	char			action;

	char			frag_off	: 1,
					direction	: 2,
					rvd			: 5;

	short			id;
	char			payload[REALTIME_SESS_PAYLOAD_LEN];

	union _desc {
		struct _frag {
			int		offset;
		}frag;
		struct _tcp {
			char	syn	: 1,
					fin : 1,
					ack : 1,
					rst : 1,
					urg : 1,
					psh : 1,
					rvd : 2;
		}tcp;
		struct _icmp {
			unsigned char type;
			unsigned char code;
		}icmp;
	}desc;
}sess_info_str;

typedef struct
{
	u_int 			uid;
	u_int			source_ip[2];
	u_short			source_port[2];
	u_int			dest_ip[2];
	u_short			dest_port[2];
	char			action;
	char			eth;				// 0 : all, 1,2,3, : ethN
	char			uid_type;
	char			rsvd;
	u_int			protocol;
	u_int			dir;				// 1:->, 2:<-, 3:<->
}rpm_filter_str;

typedef struct
{
	u_int			flag;
	u_int			r_index;
	rpm_filter_str	rpm_filter;
}rpm_info_struct;

typedef struct
{
	__be32		sip;
	__be32		dip;
	__be16		sport;
	__be16		dport;
	u8			proto;
}block_sess_info;

enum {      
	RPM_PARAM_FLAG = 0,
	RPM_PARAM_UID_TYPE,
	RPM_PARAM_UID,
	RPM_PARAM_SIP1,
	RPM_PARAM_SIP2,
	RPM_PARAM_DIP1,
	RPM_PARAM_DIP2,
	RPM_PARAM_PROTOCOL,
	RPM_PARAM_SPORT1,
	RPM_PARAM_SPORT2,
	RPM_PARAM_DPORT1,
	RPM_PARAM_DPORT2,
	RPM_PARAM_ACTION,
	RPM_PARAM_MAX
};

enum {
	BLOCK_PARAM_SIP = 0,
	BLOCK_PARAM_DIP,
	BLOCK_PARAM_SPORT,
	BLOCK_PARAM_DPORT,
	BLOCK_PARAM_PROTO,
	BLOCK_PARAM_MAX
};

extern rpm_info_struct g_rpm_info;

extern int rpm_pool_init(void);
extern int rpm_pool_free(void);
extern int rpm_log(struct sk_buff *skb, int verdict);

extern int init_rpm_module(void);
