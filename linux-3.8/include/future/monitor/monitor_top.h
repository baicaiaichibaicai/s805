//#ifndef __FUTURE_MONITOR__
#define __FUTURE_MONITOR__

/* Top-100 user section */
struct trf_user_key {
	__u32   addr;
};

struct trf_user_entry {
	__u32   addr;
	__u32   bps;
	__u32   pps;
	__u32   bytes;
	__u32   packets;
	u_long  last;
};

struct rank_user_entry {
	struct list_head list;
	__u32 addr;
	__u32 bps;
	__u32 pps;
	__u32 stamp;
	u_long update_jiffies;
};

/* Top-100 svc section */
struct trf_svc_entry {
	__u32   port;
	__u16	proto;
	__u32   bps;
	__u32   pps;
	__u32   bytes;
	__u32   packets;
	u_long  last;
};

struct trf_p_num_entry {
	__u16   p_num;
	__u16   action;
	__u32   bps;
	__u32   pps;
	__u32   bytes;
	__u32   packets;
	u_long  last;
};


struct rank_svc_entry {
	struct list_head list;
	__u32 port;
	__u16 proto;
	__u32 bps;
	__u32 pps;
	__u32 stamp;
	u_long update_jiffies;
};

struct rank_p_num_entry {
	struct list_head list;
	__u16 p_num;
	__u16 action;
	__u32 bps;
	__u32 pps;
	__u32 stamp;
	u_long update_jiffies;
};


struct summary_user_entry {
	struct rank_user_entry src;
	struct rank_user_entry dst;

	unsigned int s_cnt;
	unsigned int d_cnt;
};

struct summary_svc_entry {
	struct rank_svc_entry svc;
	unsigned int cnt;
};

struct summary_pnum_entry {
	struct rank_p_num_entry pnum;
	unsigned int cnt;
};

#define TRF_TH_DELTA_TIME               5
#define TRF_USER_TABLE_SIZE             10000
#define TRF_USER_CLEANUP_TIME           30
#define TRF_USER_EXPIRE_TIME            15
#define TRF_USER_RANK_SIZE              100
#define TRF_USER_BUFFER_SIZE            ((TRF_USER_RANK_SIZE * 2) + 1) * \
	sizeof("255.255.255.255 4294967295 4294967295 4294967295 ")

#define TRF_SRC                         1
#define TRF_DEST                        2

#define TRF_SVC_TABLE_SIZE              2048
#define TRF_SVC_KEY_TCP					0
#define TRF_SVC_KEY_UDP					1024
#define TRF_SVC_CLEANUP_TIME            30
#define TRF_SVC_EXPIRE_TIME             15
#define TRF_SVC_RANK_SIZE               100
#define TRF_SVC_BUFFER_SIZE             (TRF_SVC_RANK_SIZE + 1) *  \
	sizeof("4294967295 4294967295 4294967295 4294967295 ")

#define TRF_P_NUM_TABLE_SIZE            5000
#define TRF_P_NUM_CLEANUP_TIME          30
#define TRF_P_NUM_EXPIRE_TIME           15
#define TRF_P_NUM_RANK_SIZE             100
#define TRF_P_NUM_BUFFER_SIZE           (TRF_P_NUM_RANK_SIZE + 1) * \
	sizeof("4294967295 2 4294967295 4294967295 4294967295 ")

/* TOP-100 flow section */
struct mon_flow_key {
	u32 saddr;
	u32 daddr;
};

struct mon_flow_entry {
	u32 saddr;
	u32 daddr;
	u32 bps;
	u32 pps;
	u32 bytes;
	u32 packets;
	u_long last;
};

struct rank_flow_entry {
	struct list_head list;
	u32 saddr;
	u32 daddr;
	u32 bps;
	u32 pps;
	u32 stamp;
	u_long update_jiffies;
};

#define MAX_IFACE_MON_HOUR 6
#define MAX_U64_SIZE    ~((u64)0x00) - 1

struct summary_flow_entry {
	struct rank_flow_entry flow;
	unsigned int cnt;
};

#define MON_FLOW_DELTA_TIME             5
#define MON_FLOW_TABLE_SIZE             10000
#define MON_FLOW_CLEANUP_TIME           30
#define MON_FLOW_EXPIRE_TIME            15
#define MON_FLOW_RANK_SIZE              100
#define MON_FLOW_BUFFER_SIZE            (MON_FLOW_RANK_SIZE + 1) * \
	sizeof("255.255.255.255(255.255.255.255) 255.255.255.255(255.255.255.255) \
			4294967295 4294967295 4294967295 ")

#define MON_TIMEOUT                     60
#define MON_INTERVAL                    30

#define get_delta_jiffies(a, b) (time_after_eq(a, b)) ? a - b : (ULONG_MAX - b) + a;

struct sk_buff;
extern int lookup_svc(struct sk_buff *);

extern int mon_iface_proc_init(void);

extern int mon_dist_proc_init(void);
extern int mon_dist2_proc_init(void);

extern int lookup_flow(u32 key_saddr, u32 key_daddr, u32 length);
extern void top_traffic_mon(struct sk_buff *skb);
extern int stat_policy_proc_init(void);
extern int log_mon_proc_init(void);
extern int top10_mon_proc_init(void);
extern int mon_flow_proc_init(void);
extern int on_top;

int get_mon_flow(struct rank_flow_entry *tbl);
int get_mon_src(struct rank_user_entry *tbl);
int get_mon_dst(struct rank_user_entry *tbl);
int get_mon_svc(struct rank_svc_entry *tbl);

extern char *adjust_scale(char *buf, u32 val);
extern int g_num_cpus;
extern int g_p_num_flag;

extern char uon, son, fon;
extern struct timer_list utimeout;
extern struct timer_list stimeout;
extern struct timer_list ftimeout;
extern void disable_traffic_mon(unsigned long ptr);

extern int gbTrafficViewFinished;

int traffic_p_num_proc(struct sk_buff *skb, int verdict,int hook);
int stat_p_num_proc_read(char *buffer, char **buffer_location, off_t offset,int buffer_length, int *zero, void
		*ptr);
void lookup_protocol(struct sk_buff *skb);
int traffic_p_num_proc(struct sk_buff *skb, int verdict,int hook);
void update_top_mon(void);

#define MAX_INTERFACE_COUNT 64
struct interface_tunnel {
	unsigned int count[MAX_INTERFACE_COUNT];
};
extern struct interface_tunnel iface_tunnel[NR_CPUS];

//#endif  /* __FUTURE_MONITOR */


