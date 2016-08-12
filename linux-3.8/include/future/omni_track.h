#ifndef _FUTURE_OMNI_TRACK_H
#define _FUTURE_OMNI_TRACK_H

#include <future/omni_track_common.h>
#include <future/omni/omni_timer.h>
#include <future/omni/omni_track_tuple_common.h>
#include <future/omni/omni_track_tcp.h>
#include <future/omni/omni_track_icmp.h>
#include <future/omni/omni_track_acct.h>
/*#include <future/omni/omni_track_firewall.h>*/
#include <future/omni/filter/omni_filter.h>
#include <future/omni/omni_track_ftp.h>
#include <future/omni/omni_track_pptp.h>
#include <future/omni/omni_track_gre.h>
#include <future/omni/omni_track_h323.h>
#include <future/datasheet.h>

#ifdef CONFIG_FERRET_IPSEC
#include <linux/xfrm.h>
#endif

#ifdef __KERNEL__
#include <future/omni/omni_track_tuple.h>
#endif
#include <uapi/linux/if.h>
#define OMNI_ANY	0

enum timeout_type {
	TCP_NONE_TIMEOUT,
	TCP_SYN_SENT_TIMEOUT,
	TCP_SYN_RECV_TIMEOUT,
	TCP_ESTABLISHED_TIMEOUT,
	TCP_FIN_WAIT_TIMEOUT,
	TCP_CLOSE_WAIT_TIMEOUT,
	TCP_LAST_ACK_TIMEOUT,
	TCP_TIME_WAIT_TIMEOUT,
	TCP_CLOSE_TIMEOUT,
	TCP_LISTEN_TIMEOUT,
#define TCP_SYN_SENT2_TIMEOUT	TCP_LISTEN_TIMEOUT
	TCP_MAX_TIMEOUT,
	TCP_IGNORE_TIMEOUT,
	TCP_RETRANS_TIMEOUT,
	TCP_UNACK_TIMEOUT,
	TCP_TIMEOUT_MAX,
	UDP_TIMEOUT,
	UDP_TIMEOUT_STREAM,
	ICMP_TIMEOUT,
	ICMPV6_TIMEOUT,
	TIMEOUT_MAX	// it's size of omni_track_timeout array. don't use this symbol.
};

extern unsigned int omni_track_timeout[];

/* per omni_track: protocol private data */
union omni_track_proto {
	/* insert omni_track proto private data here */
	struct omni_track_tcp tcp;
	struct omni_track_icmp icmp;
   struct omni_track_gre gre;
	/* struct omni_track_icmpv6 icmpv6; */
};

union omni_track_help{
	struct omni_track_ftp_master tr_ftp_info;
	struct omni_track_pptp_master tr_pptp_info;
	struct omni_track_h323_master tr_h323_info;

};

struct omni_track_helper;

struct omni_tr_help{
	struct omni_track_helper *helper;
	union omni_track_help help;

	/* Current number of expected connections */
	unsigned int expecting;
};

#ifdef CONFIG_OMNI_NAT64
struct omni_nat64_range {
	__be32 min_ip;
	__be32 max_ip;
};
#endif

#ifdef CONFIG_FERRET_IPSEC
struct ipsec_info_t {
	__u32 encrypt_spi;
	__u32 encrypt_reqid;
	__u32 decrypt_spi;
	__u32 decrypt_reqid;
	__u8 encrypt_proto;
	__u8 decrypt_proto;
};
#endif

struct omni_track {
	/* 항상 첫번째 엔트리 유지해야 함 */
	struct omni_track_tuple_hash tuplehash[OMNI_TRACK_DIR_MAX];

	spinlock_t lock;

	/* Have we seen traffic both ways yes? (bitset) */
	unsigned long status;
	unsigned long id;

	/* If we were expected by an expectation, this will be it */
	struct omni_track *master;

	/* omni_track expire timeout */
	unsigned long tr_timeout;

	/* for check validation */
	atomic_t expired;

#ifdef CONFIG_OMNI_TIMER
	/* Timer function; */
	struct omni_timer timeout;
#else
	struct timer_list timeout;
#endif

	unsigned long sync_jiffies;

	void (*destroy)(struct omni_track *);

	union omni_track_proto proto;

	/* Extensions */
	struct omni_track_ext *omni_ext;

	/* omni-nat declare */
	struct omni_nat nat;
	struct omni_block block;

    /* for omni sync */
    unsigned char forward;

	/* for multipath routing */
	char mpr_iface;
	char mpr_nh_sel;

	/* for Ferret IPSec */
	bool is_ipsec;

    /* Traffic anomaly & DDoS */
	int tcp_state;
	int param_cnt;
	char zero_window_cnt;
	char win_scale_factor;

    /* Blocked session index */
    int block_index;

	/* stat */
	struct omni_track_counter acct[OMNI_TRACK_DIR_MAX];

	/* receive device */
	char in_dev[8];

	/* initial jiffies */
	unsigned long initial_jiffies;

	/* for Application Control */
	// missed count of packet and data when detected application packet
	struct omni_track_counter missed_acct[OMNI_TRACK_DIR_MAX];

	/* User Awareness */
	char userid[32];

	struct omni_tr_help help;
#ifdef CONFIG_OMNI_NAT64
    struct omni_nat64_range nat64_range;
	struct omni_nat nat64;
#endif
#ifdef CONFIG_FERRET_IPSEC
	struct ipsec_info_t ipsec_info; 
#endif
};

#ifdef CONFIG_FERRET_TRACK_DEBUG
struct debug_track_entry {
	struct list_head list;

	struct omni_track_tuple tuple;

	unsigned long ppath[10];
	unsigned int confirm;
};
#endif

static inline struct omni_track *
omni_track_tuplehash_to_track(const struct omni_track_tuple_hash *hash)
{
	return container_of(hash, struct omni_track,
						tuplehash[hash->tuple.dst.dir]);
}

static inline u_int16_t omni_l3num(const struct omni_track *track)
{
	return track->tuplehash[OMNI_TRACK_DIR_ORIGINAL].tuple.src.l3num;
}

static inline u_int8_t omni_protonum(const struct omni_track *track)
{
	return track->tuplehash[OMNI_TRACK_DIR_ORIGINAL].tuple.dst.protonum;
}

#define omni_tuple(track, dir) (&(track)->tuplehash[dir].tuple)

DECLARE_PER_CPU(struct omni_track_stat, omni_track_stat);
#define OMNI_TRACK_STAT_INC(count) (__get_cpu_var(omni_track_stat).count++)

extern void __omni_track_refresh(struct omni_track *track,
									  unsigned long extra_jiffies);
extern void __omni_track_acct(struct omni_track *track,
					   enum omni_track_info trinfo,
					   const struct sk_buff *skb);
extern bool __omni_track_kill_acct(struct omni_track *track,
		       enum omni_track_info trinfo,
		       const struct sk_buff *skb,
		       int do_acct);

/* Refresh omnitrack for this many jiffies and do accounting */
static inline void omni_track_refresh(struct omni_track *track,
									  unsigned long extra_jiffies)
{
	__omni_track_refresh(track, extra_jiffies);
}

static inline void omni_track_acct(struct omni_track *track,
					   enum omni_track_info trinfo,
					   const struct sk_buff *skb)
{
	__omni_track_acct(track, trinfo, skb);
}

/* kill omni-track and do accounting */
static inline bool omni_track_kill_acct(struct omni_track *track,
				   enum omni_track_info trinfo,
				   struct sk_buff *skb)
{
	return __omni_track_kill_acct(track, trinfo, skb, 1);
}

/* kill omni-track without accounting */
static inline bool omni_track_kill(struct omni_track *track)
{
	return __omni_track_kill_acct(track, 0, NULL, 0);
}

extern bool omni_track_kill_and_clean(struct sk_buff *skb);

/* These are for NAT.  Icky. */
extern s16 (*omni_track_nat_offset)(const struct omni_track *track,
			       enum omni_track_dir dir,
			       u32 seq);

/* It's confirmed if it is, or has been in the hash table. */
static inline int omni_track_is_confirmed(struct omni_track *track)
{
	return test_bit(OTS_CONFIRMED_BIT, &track->status);
}

static inline void omni4_track_tuple_make(struct omni_track_tuple *t, u8 l3num,
						 __be32 src, __be32 dst,
						 __be16 s_port, __be16 d_port, u8 proto)
{
	if (proto == IPPROTO_ICMP) {
		s_port = 0;
		d_port = 0;
	}

	t->src.l3num = l3num;
	t->src.u3.ip = src;
	t->dst.u3.ip = dst;
	t->src.u.all = s_port;
	t->dst.u.all = d_port;
	t->dst.protonum = proto;
}

static inline void omni6_track_tuple_make(struct omni_track_tuple *t, u8 l3num,
						 __be32 src[], __be32 dst[],
						 __be16 s_port, __be16 d_port, u8 proto)
{
	t->src.l3num = l3num;
	if (src)
		memcpy(t->src.u3.ip6, src, sizeof(t->src.u3.ip6));
	if (dst)
		memcpy(t->dst.u3.ip6, dst, sizeof(t->dst.u3.ip6));
	t->src.u.all = s_port;
	t->dst.u.all = d_port;
	t->dst.protonum = proto;
}

extern struct omni_track_tuple_hash *omni_track_find_prev(struct omni_track_tuple_hash *cur);
extern int omni_track_delete(struct omni_track_tuple_hash *cur, struct omni_track_tuple_hash *prev);
static inline int get_track_cnt(void)
{
	return (get_session_cnt() + (num_possible_cpus() * 32));
}

extern void __omni_track_put(struct omni_track *track);
extern int omni_track_find_and_del(struct omni_track_tuple_hash *cur);
extern int omni_track_find_and_del2(struct omni_track_tuple_hash *cur, struct omni_track_tuple_hash *prev);
extern void omni_track_destroy(struct omni_track *track);
extern unsigned int omni_track_htable_size __read_mostly;
extern struct llist_head *omni_track_hash __read_mostly;
extern int omni_track_max __read_mostly;
extern unsigned int omni_track_hash_rnd;
extern unsigned long omni_mempool_bootmem_size;

extern struct omni_track *
omni_track_alloc(const struct omni_track_tuple *orig,
	             const struct omni_track_tuple *repl);
extern int omni_track_hash_check_insert(struct omni_track *track);
extern void omni_track_free(struct omni_track *track);
extern void omni_track_cache_init(void);
extern void omni_track_disable(struct omni_track *track);

#ifdef CONFIG_FERRET_TRACK_DEBUG
extern void
add_debug_track_path(struct sk_buff *skb);
#endif

#ifdef CONFIG_OMNI_TIMER
extern void omni_death_by_timeout(struct omni_timer *timer, void *arg);
#else
extern void omni_death_by_timeout(unsigned long ul_omni_track);
#endif
u_int32_t hash_omni_track(const struct omni_track_tuple *tuple);

#endif
