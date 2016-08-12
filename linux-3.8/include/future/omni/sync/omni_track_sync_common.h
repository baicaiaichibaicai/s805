
#define VERSION	"2.74"
#define IP_NAME_SZ 32
#define MAX_MPLS_LABELS 16 /* This is the max label stack depth */
#define MPLS_STACK_BOTTOM htonl(0x00000100)
#define IP_NAME_SZ 32
#define func_enter() pr_debug("entering %s\n", __func__);

/* Device flag bits */
#define F_IPSRC_RND   (1<<0)	/* IP-Src Random  */
#define F_IPDST_RND   (1<<1)	/* IP-Dst Random  */
#define F_UDPSRC_RND  (1<<2)	/* UDP-Src Random */
#define F_UDPDST_RND  (1<<3)	/* UDP-Dst Random */
#define F_MACSRC_RND  (1<<4)	/* MAC-Src Random */
#define F_MACDST_RND  (1<<5)	/* MAC-Dst Random */
#define F_TXSIZE_RND  (1<<6)	/* Transmit size is random */
#define F_IPV6        (1<<7)	/* Interface in IPV6 Mode */
#define F_MPLS_RND    (1<<8)	/* Random MPLS labels */
#define F_VID_RND     (1<<9)	/* Random VLAN ID */
#define F_SVID_RND    (1<<10)	/* Random SVLAN ID */
#define F_FLOW_SEQ    (1<<11)	/* Sequential flows */
#define F_IPSEC_ON    (1<<12)	/* ipsec on for flows */
#define F_QUEUE_MAP_RND (1<<13)	/* queue map Random */
#define F_QUEUE_MAP_CPU (1<<14)	/* queue map mirrors smp_processor_id() */
#define F_NODE          (1<<15)	/* Node memory alloc*/

/* Thread control flag bits */
#define T_STOP        (1<<0)	/* Stop run */
#define T_RUN         (1<<1)	/* Start run */
#define T_REMDEVALL   (1<<2)	/* Remove all devs */
#define T_REMDEV      (1<<3)	/* Remove one dev */

/* If lock -- can be removed after some work */
#define   if_lock(t)           spin_lock(&(t->if_lock));
#define   if_unlock(t)           spin_unlock(&(t->if_lock));

/* Used to help with determining the pkts on receive */
#define PKTGEN_MAGIC 0xbe9be955
#define PG_PROC_DIR "pktgen"
#define PGCTRL	    "pgctrl"

#define MAX_CFLOWS  65536

#define VLAN_TAG_SIZE(x) ((x)->vlan_id == 0xffff ? 0 : 4)
#define SVLAN_TAG_SIZE(x) ((x)->svlan_id == 0xffff ? 0 : 4)


/* flow flag bits */
#define F_INIT   (1<<0)		/* flow has been initialized */

struct pktgen_dev {
	/*
	 * Try to keep frequent/infrequent used vars. separated.
	 */
	struct proc_dir_entry *entry;	/* proc file */
	struct pktgen_thread *pg_thread;/* the owner */
	struct list_head list;		/* chaining in the thread's run-queue */

	int running;		/* if false, the test will stop */

	/* If min != max, then we will either do a linear iteration, or
	 * we will do a random selection from within the range.
	 */
	__u32 flags;
	int removal_mark;	/* non-zero => the device is marked for
				 * removal by worker thread */

	int min_pkt_size;
	int max_pkt_size;
	int pkt_overhead;	/* overhead for MPLS, VLANs, IPSEC etc */
	int nfrags;
	struct page *page;
	u64 delay;		/* nano-seconds */

	__u64 count;		/* Default No packets to send */
	__u64 sofar;		/* How many pkts we've sent so far */
	__u64 tx_bytes;		/* How many bytes we've transmitted */
	__u64 errors;		/* Errors when trying to transmit, */

	/* runtime counters relating to clone_skb */

	__u64 allocated_skbs;
	__u32 clone_count;
	int last_ok;		/* Was last skb sent?
				 * Or a failed transmit of some sort?
				 * This will keep sequence numbers in order
				 */
	ktime_t next_tx;
	ktime_t started_at;
	ktime_t stopped_at;
	u64	idle_acc;	/* nano-seconds */

	__u32 seq_num;

	int clone_skb;		/*
				 * Use multiple SKBs during packet gen.
				 * If this number is greater than 1, then
				 * that many copies of the same packet will be
				 * sent before a new packet is allocated.
				 * If you want to send 1024 identical packets
				 * before creating a new packet,
				 * set clone_skb to 1024.
				 */

	char dst_min[IP_NAME_SZ];	/* IP, ie 1.2.3.4 */
	char dst_max[IP_NAME_SZ];	/* IP, ie 1.2.3.4 */
	char src_min[IP_NAME_SZ];	/* IP, ie 1.2.3.4 */
	char src_max[IP_NAME_SZ];	/* IP, ie 1.2.3.4 */

	struct in6_addr in6_saddr;
	struct in6_addr in6_daddr;
	struct in6_addr cur_in6_daddr;
	struct in6_addr cur_in6_saddr;
	/* For ranges */
	struct in6_addr min_in6_daddr;
	struct in6_addr max_in6_daddr;
	struct in6_addr min_in6_saddr;
	struct in6_addr max_in6_saddr;

	/* If we're doing ranges, random or incremental, then this
	 * defines the min/max for those ranges.
	 */
	__be32 saddr_min;	/* inclusive, source IP address */
	__be32 saddr_max;	/* exclusive, source IP address */
	__be32 daddr_min;	/* inclusive, dest IP address */
	__be32 daddr_max;	/* exclusive, dest IP address */

	__u16 udp_src_min;	/* inclusive, source UDP port */
	__u16 udp_src_max;	/* exclusive, source UDP port */
	__u16 udp_dst_min;	/* inclusive, dest UDP port */
	__u16 udp_dst_max;	/* exclusive, dest UDP port */

	/* DSCP + ECN */
	__u8 tos;            /* six MSB of (former) IPv4 TOS
				are for dscp codepoint */
	__u8 traffic_class;  /* ditto for the (former) Traffic Class in IPv6
				(see RFC 3260, sec. 4) */

	/* MPLS */
	unsigned int nr_labels;	/* Depth of stack, 0 = no MPLS */
	__be32 labels[MAX_MPLS_LABELS];

	/* VLAN/SVLAN (802.1Q/Q-in-Q) */
	__u8  vlan_p;
	__u8  vlan_cfi;
	__u16 vlan_id;  /* 0xffff means no vlan tag */

	__u8  svlan_p;
	__u8  svlan_cfi;
	__u16 svlan_id; /* 0xffff means no svlan tag */

	__u32 src_mac_count;	/* How many MACs to iterate through */
	__u32 dst_mac_count;	/* How many MACs to iterate through */

	unsigned char dst_mac[6];
	unsigned char src_mac[6];

	__u32 cur_dst_mac_offset;
	__u32 cur_src_mac_offset;
	__be32 cur_saddr;
	__be32 cur_daddr;
	__u16 ip_id;
	__u16 cur_udp_dst;
	__u16 cur_udp_src;
	__u16 cur_queue_map;
	__u32 cur_pkt_size;
	__u32 last_pkt_size;

	__u8 hh[14];
	/* = {
	   0x00, 0x80, 0xC8, 0x79, 0xB3, 0xCB,

	   We fill in SRC address later
	   0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	   0x08, 0x00
	   };
	 */
	__u16 pad;		/* pad out the hh struct to an even 16 bytes */

	struct sk_buff *skb;	/* skb we are to transmit next, used for when we
				 * are transmitting the same one multiple times
				 */
	struct net_device *odev; /* The out-going device.
				  * Note that the device should have it's
				  * pg_info pointer pointing back to this
				  * device.
				  * Set when the user specifies the out-going
				  * device name (not when the inject is
				  * started as it used to do.)
				  */
	char odevname[32];
	struct flow_state *flows;
	unsigned int cflows;	/* Concurrent flows (config) */
	unsigned int lflow;		/* Flow length  (config) */
	unsigned int nflows;	/* accumulated flows (stats) */
	unsigned int curfl;		/* current sequenced flow (state)*/

	u16 queue_map_min;
	u16 queue_map_max;
	__u32 skb_priority;	/* skb priority field */
	int node;               /* Memory node */

#ifdef CONFIG_XFRM
	__u8	ipsmode;		/* IPSEC mode (config) */
	__u8	ipsproto;		/* IPSEC type (config) */
#endif
    __u32 sync_size;
    void *data;
	char result[512];
};

struct pktgen_hdr {
	__be32 pgh_magic;
	__be32 seq_num;
	__be32 tv_sec;
	__be32 tv_usec;
};

struct flow_state {
	__be32 cur_daddr;
	int count;
#ifdef CONFIG_XFRM
	struct xfrm_state *x;
#endif
	__u32 flags;
};

struct sync_table {
    unsigned int seq_num;
	int loss;
	unsigned int sec;
    unsigned int usec;
};

struct sync_line_conf {
    int use;
	unsigned char   dev[10];
	unsigned int    ip;
	struct in6_addr	ip6;
	unsigned char mac[6];
    int ver;
};

#define TOT_SIZE 200
//#define DEBUG_OMNI_SYNC 1
#ifdef DEBUG_OMNI_SYNC
#define dprintf(format, args...)  printk(format , ## args)
#else
#define dprintf(format, args...)
#endif

extern struct sync_table sync_table_test[TOT_SIZE];
extern struct sync_line_conf line_conf;
int omni_track_sync_rcv(struct sk_buff *skb);
int sync_packet_send(struct pktgen_dev *pkt_dev);
int omni_track_sync_line_proc_read(char *page, char **start,off_t off, int count, int *eof,void *data);
int omni_track_sync_line_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);

