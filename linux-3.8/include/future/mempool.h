#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/threads.h>
#include <linux/smp.h>
#include <linux/percpu.h>
#include <asm/string.h>
#include <asm/errno.h>
#include <linux/bottom_half.h>

//#include <future/omni_track.h>

#include "queue.h"

#define CACHE_LINE_SIZE	64
#define CACHE_LINE_MASK	(CACHE_LINE_SIZE-1)

#define __omni_cache_aligned __attribute__((__aligned__(CACHE_LINE_SIZE)))

#define MP_MAX_MEMSEG	32
#define MP_MAX_MEMZONE	512
#define MP_MEMPOOL_CACHE_MAX_SIZE 512
#define MP_LOG		printk

#define MP_PKTMBUF_HEADROOM 128
#define MBUF_SIZE (2048 + sizeof(struct omni_mbuf) + MP_PKTMBUF_HEADROOM)
#define NB_MBUF   8192

#define MP_BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#define MPLOCKED	"lock;"	/* multi-core platform only --sekim */

#ifdef CONFIG_X86
static inline int
omni_atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src)
{
	uint8_t res;

	asm volatile(
			"  " MPLOCKED "                    "
			"  cmpxchgl %[src],%[dst] ;        "
			"       sete       %[res] ;        "
			: [res] "=a" (res),        /* 0 */
			[dst] "=m" (*dst)          /* 1 */
			: [src] "r" (src),         /* 2 */
			"a" (exp),                 /* 3 */
			"m" (*dst)                 /* 4 */
			: "memory");

	return res;
}
#else
#include <asm/cmpxchg.h>
static inline int
omni_atomic32_cmpset(volatile uint32_t *dst, uint32_t exp, uint32_t src)
{
	uint32_t ret=0;
	ret = cmpxchg(dst,exp,src);
	if (ret == exp)
		return 1;
	else
		return 0;
}
#endif

struct omni_memseg {
	void *addr;
	uint64_t len;
};

struct omni_memzone {
#define MP_MEMZONE_NAMESIZE 32
	char name[MP_MEMZONE_NAMESIZE];

	void *addr;
	uint64_t len;

	unsigned int flags;
};

struct omni_master_config {
	struct omni_memseg memseg[MP_MAX_MEMSEG];
	struct omni_memzone memzone[MP_MAX_MEMZONE];
};

struct omni_ring {
	TAILQ_ENTRY(omni_ring) next;	/**< Next in list. */

#define MP_RING_NAMESIZE 32
	char name[MP_RING_NAMESIZE];	/**< Name of the ring. */
	int flags;						/**< Flags supplied at creation. */

	/* producer status */
	struct {
		volatile uint32_t bulk_default;	/**< Default bulk count. */
		uint32_t watermark;				/**< Maximum items before EDQUOT. */
		uint32_t sp_enqueue;			/**< True, if single producer. */
		uint32_t size;					/**< Size of ring. */
		uint32_t mask;					/**< Mask (size-1) of ring. */
		volatile uint32_t head;			/**< Producer head. */
		volatile uint32_t tail;			/**< Producer tail. */
	} prod __omni_cache_aligned;	/**< Ring producer status. */

	/* consumer status */
	struct {
		volatile uint32_t bulk_default;	/**< Default bulk count. */
		uint32_t sc_dequeue;			/**< True, if single consumer. */
		uint32_t size;					/**< Size of the ring. */
		uint32_t mask;					/**< Mask (size-1) of ring. */
		volatile uint32_t head;			/**< Consumer head. */
		volatile uint32_t tail;			/**< Consumer tail. */
	} cons __omni_cache_aligned;	/**< Ring consumer status. */

	void *ring[0];					/**< Memory space of ring starts here. */
};

#if MP_MEMPOOL_CACHE_MAX_SIZE > 0
/**             
 * A structure that stores a per-core object cache.
 */     
struct omni_mempool_cache {
	unsigned int len;
	void *objs[MP_MEMPOOL_CACHE_MAX_SIZE];
} __omni_cache_aligned;
#endif /* MP_MEMPOOL_CACHE_MAX_SIZE > 0 */

#define MP_MEMPOOL_NAMESIZE 32 /**< Maximum length of a memory pool. */

/**
 * The MP mempool structure. 
 */                
struct omni_mempool {
	TAILQ_ENTRY(omni_mempool) next;	/**< Next in list. */

	char name[MP_MEMPOOL_NAMESIZE];	/**< Name of mempool. */
	struct omni_ring *ring;			/**< Ring to store objects. */
	int flags;						/**< Flags of the mempool. */
	uint32_t size;					/**< Size of the mempool. */
	uint32_t bulk_default;			/**< Default bulk count. */
	uint32_t cache_size;			/**< Size of per-lcore local cache. */

	uint32_t elt_size;				/**< Size of an element. */
	uint32_t header_size;			/**< Size of header (before elt). */
	uint32_t trailer_size;			/**< Size of trailer (after elt). */

	unsigned private_data_size;		/**< Size of private data. */

#if MP_MEMPOOL_CACHE_MAX_SIZE > 0
	/** Per-lcore local cache. */
	struct omni_mempool_cache local_cache[NR_CPUS];
#endif
} __omni_cache_aligned;

struct omni_data {
	void *data;			/**< Start address of data in segment buffer. */
	uint16_t data_len;	/**< Amount of data in segment buffer. */
};

struct omni_mbuf {
	struct omni_mempool *pool;	/**< Pool from which mbuf was allocated. */
	void *buf_addr;				/**< Virtual address of segment buffer. */
	uint32_t buf_len;			/**< Length of segment buffer. */
	uint32_t uid;			/**< Unique Identifier of memory buffer. */

	struct omni_data e;
};

struct omni_common_pool_private {
	uint16_t mbuf_data_room_size;	/**< Size of data space in each mbuf.*/
};


typedef void (omni_mempool_obj_ctor_t)(struct omni_mempool *, void *, void *, unsigned int);
typedef void (omni_mempool_ctor_t)(struct omni_mempool *, void *);

#define RING_F_SP_ENQ 0x0001	/**< The default enqueue is "single-producer". */
#define RING_F_SC_DEQ 0x0002	/**< The default dequeue is "single-consumer". */

#define MEMPOOL_F_NO_SPREAD      0x0001	/**< Do not spread in memory. */
#define MEMPOOL_F_NO_CACHE_ALIGN 0x0002	/**< Do not align objs on cache lines.*/
#define MEMPOOL_F_SP_PUT         0x0004	/**< Default put is "single-producer".*/
#define MEMPOOL_F_SC_GET         0x0008	/**< Default get is "single-consumer".*/

extern struct omni_master_config omni_config;
extern struct omni_mempool *pktmbuf_pool;
extern struct omni_mempool *omni_track_pool;


extern int mp_init(void);
extern int omni_memzone_init(void);
extern const struct omni_memseg *omni_get_physmem_layout(void);
extern void omni_memzone_dump(void);
extern const struct omni_memzone *omni_memzone_lookup(const char *name);
extern const struct omni_memzone *omni_memzone_reserve(const char *name, uint64_t len, unsigned int flags);

extern struct omni_mempool *omni_mempool_create(const char *name, unsigned int n, 
		unsigned int elt_size, unsigned int cache_size, 
		unsigned int private_data_size,	
		omni_mempool_ctor_t *omni_init, void *omni_init_arg, 
		omni_mempool_obj_ctor_t *obj_init, void *obj_init_arg,
		unsigned int flags);


extern struct omni_ring *omni_ring_lookup(const char *name);
extern struct omni_ring *omni_ring_create(const char *name, unsigned int count, unsigned int flags);
extern void omni_mempool_list_dump(void);
extern void omni_ring_list_dump(void);

static inline struct omni_master_config *
omni_get_master_config(void)
{
	return &omni_config;
}

static inline struct omni_mempool **__mempool_from_obj(const void *obj)
{
	struct omni_mempool **mpp;
	unsigned off;

	off = sizeof(struct omni_mempool *);
	mpp = (struct omni_mempool **)((const char *)obj - off);
	return mpp;
}

static inline void *omni_mempool_get_priv(struct omni_mempool *mp)
{
	return (char *)mp + sizeof(struct omni_mempool);
}

static inline int
omni_ring_mp_enqueue_bulk(struct omni_ring *r, void * const *obj_table,
		unsigned int n)
{       
	uint32_t prod_head, prod_next;
	uint32_t cons_tail, free_entries;
	int success;
	unsigned i;
	uint32_t mask = r->prod.mask;
	int ret;

	/* move prod.head atomically */
	do {    
		prod_head = r->prod.head;
		cons_tail = r->cons.tail;
		free_entries = (mask + cons_tail - prod_head);

		/* check that we have enough room in ring */
		if (unlikely(n > free_entries)) {
			return -ENOBUFS;
		}

		prod_next = prod_head + n;
		success = omni_atomic32_cmpset(&r->prod.head, prod_head,
				prod_next);
	} while (unlikely(success == 0));

	/* write entries in ring */
	for (i = 0; likely(i < n); i++)
		r->ring[(prod_head + i) & mask] = obj_table[i];
	wmb();

	/* return -EDQUOT if we exceed the watermark */
	if (unlikely(((mask + 1) - free_entries + n) > r->prod.watermark)) {
		ret = -EDQUOT;
	} else {
		ret = 0;
	}

	/*
	 * If there are other enqueues in progress that preceeded us,
	 * we need to wait for them to complete
	 */
	while (unlikely(r->prod.tail != prod_head))
		;

	r->prod.tail = prod_next;
	return ret;
}

static inline int
omni_ring_sp_enqueue_bulk(struct omni_ring *r, void * const *obj_table,
		unsigned int n)
{
	uint32_t prod_head, cons_tail;
	uint32_t prod_next, free_entries;
	unsigned int i;
	uint32_t mask = r->prod.mask;
	int ret;

	prod_head = r->prod.head;
	cons_tail = r->cons.tail;
	free_entries = (mask + cons_tail - prod_head) & mask;

	/* check that we have enough room in ring */
	if (unlikely(n > free_entries)) {
		//__RING_STAT_ADD(r, enq_fail, n);
		return -ENOBUFS;
	}

	prod_next = prod_head + n;
	r->prod.head = prod_next;

	/* write entries in ring */
	for (i = 0; likely(i < n); i++)
		r->ring[(prod_head + i) & mask] = obj_table[i];
	wmb();

	/* return -EDQUOT if we exceed the watermark */
	if (unlikely(((mask + 1) - free_entries + n) > r->prod.watermark)) {
		ret = -EDQUOT;
		//__RING_STAT_ADD(r, enq_quota, n);
	}
	else {
		ret = 0;
		//__RING_STAT_ADD(r, enq_success, n);
	}

	r->prod.tail = prod_next;
	return ret;
}

static inline int
omni_ring_sp_enqueue(struct omni_ring *r, void *obj)
{               
	return omni_ring_sp_enqueue_bulk(r, &obj, 1);
}      

static inline unsigned
omni_ring_count(const struct omni_ring *r)
{       
	uint32_t prod_tail = r->prod.tail;
	uint32_t cons_tail = r->cons.tail;
	return ((prod_tail - cons_tail) & r->prod.mask);
}

static inline unsigned
omni_ring_free_count(const struct omni_ring *r) 
{       
	uint32_t prod_tail = r->prod.tail;
	uint32_t cons_tail = r->cons.tail;
	return ((cons_tail - prod_tail - 1) & r->prod.mask);
}

static inline int
omni_ring_mc_dequeue_bulk(struct omni_ring *r, void **obj_table, unsigned n)
{               
	uint32_t cons_head, prod_tail;
	uint32_t cons_next, entries;
	int success;
	unsigned int i;
	uint32_t mask = r->prod.mask;

	/* move cons.head atomically */
	do {    
		cons_head = r->cons.head;
		prod_tail = r->prod.tail;
		entries = (prod_tail - cons_head);

		/* check that we have enough entries in ring */
		if (unlikely(n > entries)) {
			//__RING_STAT_ADD(r, deq_fail, n);
			return -ENOENT;
		}

		cons_next = cons_head + n;
		success = omni_atomic32_cmpset(&r->cons.head, cons_head,
				cons_next);
	} while (unlikely(success == 0));

	/* copy in table */
	rmb();
	for (i = 0; likely(i < n); i++) {
		obj_table[i] = r->ring[(cons_head + i) & mask];
	}

	/*
	 * If there are other dequeues in progress that preceeded us,
	 * we need to wait for them to complete
	 */
	while (unlikely(r->cons.tail != cons_head))
		;

	//__RING_STAT_ADD(r, deq_success, n);
	r->cons.tail = cons_next;
	return 0;
}

static inline int
omni_ring_sc_dequeue_bulk(struct omni_ring *r, void **obj_table, unsigned n)
{
	uint32_t cons_head, prod_tail;
	uint32_t cons_next, entries;
	unsigned int i;
	uint32_t mask = r->prod.mask;

	cons_head = r->cons.head;
	prod_tail = r->prod.tail;
	entries = (prod_tail - cons_head) & mask;

	/* check that we have enough entries in ring */
	if (unlikely(n > entries)) {
		//__RING_STAT_ADD(r, deq_fail, n);
		return -ENOENT;
	}

	cons_next = cons_head + n;
	r->cons.head = cons_next;

	/* copy in table */
	rmb();
	for (i = 0; likely(i < n); i++) {
		obj_table[i] = r->ring[(cons_head + i) & mask];
	}

	//__RING_STAT_ADD(r, deq_success, n);
	r->cons.tail = cons_next;
	return 0;
}

static inline int
__mempool_get_bulk(struct omni_mempool *mp, void **obj_table,
		unsigned n, int is_mc)
{
	int ret;
#if MP_MEMPOOL_CACHE_MAX_SIZE > 0
	struct omni_mempool_cache *cache;
	uint32_t cache_len, cache_len_save = 0;
	void **cache_objs;
	unsigned int lcore_id = smp_processor_id();
	uint32_t cache_size = mp->cache_size;
	uint32_t cache_del_count;

	cache = &mp->local_cache[lcore_id];

	/* cache is not enabled or single consumer */
	if (unlikely(cache_size == 0 || is_mc == 0))
		goto ring_dequeue;

	cache_len = cache->len;
	cache_objs = cache->objs;

	/* cache is empty and we need many objects: dequeue from ring */
	if (unlikely(cache_len == 0 && n >= cache_size))
		goto ring_dequeue;

	/* cache is empty and we dequeue few objects: fill the cache first */
	if (unlikely(cache_len == 0 && n < cache_size)) {
		ret = omni_ring_mc_dequeue_bulk(mp->ring, cache_objs,
				cache_size);
		if (unlikely(ret < 0)) {
			//__MEMPOOL_STAT_ADD(mp, get_fail, n_orig);
			return ret;
		}

		cache_len = cache_size;
	}

	if (likely(n <= cache_len))
		cache_del_count = n;
	else
		cache_del_count = cache_len;

	cache_len_save = cache_len;

	/* add in cache only while there is enough room */
	while (likely(cache_del_count > 0)) {
		cache_len--;
		*obj_table = cache_objs[cache_len];
		obj_table++;
		n--;
		cache_del_count--;
	}

	cache->len = cache_len;

	/* no more object to get, return */
	if (likely(n == 0)) {
		//__MEMPOOL_STAT_ADD(mp, get_success, n_orig);
		return 0;
	}

ring_dequeue:
#endif /* MP_MEMPOOL_CACHE_MAX_SIZE > 0 */

	/* get remaining objects from ring */
	if (is_mc)
		ret = omni_ring_mc_dequeue_bulk(mp->ring, obj_table, n);
	else
		ret = omni_ring_sc_dequeue_bulk(mp->ring, obj_table, n);

#if MP_MEMPOOL_CACHE_MAX_SIZE > 0
	/*
	 * bad luck, the ring is empty but we already dequeued some
	 * entries from cache, we have to restore them
	 */
	if (unlikely(ret < 0 && cache_len_save != 0))
		cache->len = cache_len_save;
#endif
	/*
	   if (ret < 0)
	   __MEMPOOL_STAT_ADD(mp, get_fail, n_orig);
	   else
	   __MEMPOOL_STAT_ADD(mp, get_success, n_orig);
	 */
	return ret;
}

static inline int
omni_mempool_get_bulk(struct omni_mempool *mp, void **obj_table, unsigned int n)
{
	int ret;

	local_bh_disable();
	ret = __mempool_get_bulk(mp, obj_table, n,
			!(mp->flags & MEMPOOL_F_SC_GET));
	local_bh_enable();
	return ret;
}

static inline int
omni_mempool_get(struct omni_mempool *mp, void **obj_p)
{
	return omni_mempool_get_bulk(mp, obj_p, 1);
}

static inline void
__mempool_put_bulk(struct omni_mempool *mp, void * const *obj_table,
		unsigned int n, int is_mp)
{
#if MP_MEMPOOL_CACHE_MAX_SIZE > 0
	struct omni_mempool_cache *cache;
	uint32_t cache_len;
	void **cache_objs;
	unsigned lcore_id = smp_processor_id();
	uint32_t cache_size = mp->cache_size;
	uint32_t cache_add_count;

	/* cache is not enabled or single producer */
	if (unlikely(cache_size == 0 || is_mp == 0))
		goto ring_enqueue;

	cache = &mp->local_cache[lcore_id];
	cache_len = cache->len;
	cache_objs = cache->objs;

	/* cache is full and we add many objects: enqueue in ring */
	if (unlikely(cache_len == cache_size && n >= cache_size))
		goto ring_enqueue;

	/*
	 * cache is full and we add few objects: enqueue the content
	 * of the cache in ring
	 */
	if (unlikely(cache_len == cache_size)) {
		omni_ring_mp_enqueue_bulk(mp->ring, cache->objs,
				cache_size);
		cache_len = 0;
	}

	/* determine how many objects we can add in cache */
	if (likely(n <= cache_size - cache_len))
		cache_add_count = n;
	else
		cache_add_count = cache_size - cache_len;

	/* add in cache while there is enough room */
	while (likely(cache_add_count > 0)) {
		cache_objs[cache_len] = *obj_table;
		obj_table++;
		cache_len++;
		n--;
		cache_add_count--;
	}

	cache->len = cache_len;

	/* no more object to add, return */
	if (likely(n == 0))
		return;

ring_enqueue:
#endif /* RTE_MEMPOOL_CACHE_MAX_SIZE > 0 */

	/* push remaining objects in ring */
	if (is_mp)
		omni_ring_mp_enqueue_bulk(mp->ring, obj_table, n);
	else
		omni_ring_sp_enqueue_bulk(mp->ring, obj_table, n);
}

static inline void
omni_mempool_put_bulk(struct omni_mempool *mp, void * const *obj_table,
		unsigned int n)
{
	__mempool_put_bulk(mp, obj_table, n, !(mp->flags & MEMPOOL_F_SP_PUT));
}

static inline void
omni_mempool_put(struct omni_mempool *mp, void *obj)
{       
	omni_mempool_put_bulk(mp, &obj, 1);
}

static inline struct omni_mbuf *
omni_mbuf_alloc(struct omni_mempool *mp)
{
	void *mb;

	if (omni_mempool_get(mp, &mb) < 0)
		return NULL; 
	return (struct omni_mbuf *)mb;
}

static inline void
omni_mbuf_free(struct omni_mbuf *mb)
{
	omni_mempool_put(mb->pool, mb);
}
#if 0
static inline struct sk_buff *
omni_alloc_skb_head(void)
{
	struct omni_mbuf *m;

	m = omni_mbuf_alloc(pktmbuf_pool);
	if (m)
		return (struct sk_buff *) &m->e.data;
	return NULL;
}

static inline void *
omni_alloc_skb_data(struct sk_buff *skb)
{
	/* FIXME instead of aligned size */
	return (void *)skb + 256;
}

static inline void
omni_free_skb(void *p)
{
	struct omni_mbuf *m = (struct omni_mbuf *) container_of(p, struct omni_mbuf, e.data);
	omni_mbuf_free(m);
}
#endif

static inline struct omni_track *
omni_track_mempool_alloc(void)
{
	struct omni_mbuf *m;

	m = omni_mbuf_alloc(omni_track_pool);
	if (m)
		return (struct omni_track *) &m->e.data;
	return NULL;
}

static inline void
omni_track_mempool_free(void *p)
{
	struct omni_mbuf *m = (struct omni_mbuf *) container_of(p, struct omni_mbuf, e.data);
	omni_mbuf_free(m);
}

static inline uint32_t 
omni_track_rawid_get(void *p)
{
	struct omni_mbuf *m = (struct omni_mbuf *) container_of(p, struct omni_mbuf, e.data);
	return m->uid;
}
