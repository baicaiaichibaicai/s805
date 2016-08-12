#ifndef _OMNI_TRACK_CACHE_H
#define _OMNI_TRACK_CACHE_H

#define OMNI_CACHE_L1	0
#define OMNI_CACHE_L2	1
#define OMNI_CACHE_L3	2
#define OMNI_CACHE_MAX	3

/**
  general purpose cache
	features:
	- if restored policy, flush that cached data.
	- allows a value only. DON'T USE POINTER.
 */
struct omni_cache_l1 {
    struct omni_filter filter;
	unsigned char status;
	unsigned char op_mode;
	unsigned char cos;
	unsigned char dscp;
	unsigned int verdict;
	unsigned int qos_id[OMNI_TRACK_DIR_MAX];
	unsigned int app_id;
	
    int white;
};

struct omni_dynamic {
	atomic_t revision;
	unsigned int count;
	unsigned int last;
	unsigned int *sche_index;
	unsigned int *rule_index;
};

struct omni_track_expect_cache {
	struct omni_track_expect *exp;
//	u_int8_t type;
};

/**
  purpose that keep the status
  	features:
	- keep the status when destroy a track.
	- allows a pointer reference.
 */
struct omni_cache_l2 {
	struct omni_dynamic dynamics;
	struct omni_track_expect_cache exp_cache;
};

/**
  statistics cache
  	features:
	- accounting data
	- write operation frequently
 */
struct omni_cache_l3 {
	struct omni_track_counter acct[OMNI_TRACK_DIR_MAX];
};

struct omni_cache {
	union {
		struct omni_cache_l1 l1;
		struct omni_cache_l2 l2;
		struct omni_cache_l3 l3;
	} u;
};

extern void omni_cache_flush(void);
extern void omni_cache_entry_init(int layer, unsigned long id);
extern struct omni_cache *omni_cache_get(int layer, unsigned long id);
extern bool omni_track_is_cached(struct sk_buff *skb);
extern bool omni_track_is_white(struct sk_buff *skb);
extern bool omni_track_is_trust(struct sk_buff *skb);
extern void omni_cache_state_flush(void);

extern void omni_cache_free(unsigned long id);

static inline struct omni_cache_l1 *omni_cache_l1_get(unsigned long id)
{
	return (struct omni_cache_l1 *)omni_cache_get(OMNI_CACHE_L1, id);
}

static inline struct omni_cache_l2 *omni_cache_l2_get(unsigned long id)
{
	return (struct omni_cache_l2 *)omni_cache_get(OMNI_CACHE_L2, id);
}

static inline struct omni_cache_l3 *omni_cache_l3_get(unsigned long id)
{
	return (struct omni_cache_l3 *)omni_cache_get(OMNI_CACHE_L3, id);
}

#define omni_cache_ptr(cache, member) ({ &cache->member; })
#define omni_cache_value(cache, member) ({ cache->member; })

#endif
