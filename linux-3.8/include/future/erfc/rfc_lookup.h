/*
 * 	Multiple packet classification technique
 *
 *	@created on 2007. 3. 27
 *	
 *	@author : babystep :)
 */
#ifndef __RFC_LOOKUP__
#define __RFC_LOOKUP__

#ifdef CONFIG_FERRET_ERFC_LOOKUP
# include <future/erfc/erfc.h>
#endif


#define IPT_MATCH_IPGROUP_SRC   0x00000001
#define IPT_MATCH_IPGROUP_DST   0x00000002

#define IP_GROUP_MAXNAMELEN 128

enum rfc_status {
	RFC_OFF = 0,
	RFC_READY,
	RFC_ON,
};
//cache//

struct erfc_cache_v2 {
	__u16 bound;
	__u16 value;
};


//#define SUPPORT_ERFC_CACHE

#define IP_CACHE_SIZE 800
#define IPG_CACHE_SIZE 200
#define B_CHUNK_SIZE 8192

struct erfc_cache_entry {
	//struct list_head list;
	__u32 used;
	unsigned char hi[B_CHUNK_SIZE];
	unsigned char low[B_CHUNK_SIZE];
};

struct erfc_cache {
	int num;
	int count; 
	int entry_size;
	struct erfc_cache_entry entry[0];
};


struct erfc_ip_map_entry{
	int index;
	int cached;
};

struct erfc_ip_map{
	int num;
	struct erfc_ip_map_entry entry[0];
};



/* Using RFC lookup */
struct rfc_addr {
    union {
	__u32 	addr;
	struct {
	    __u16 hi, low;
	} p;
    } u_addr;
};

/* 
 * RFC algorithm CBM table entry
 */
struct rfc_cbm_entry {
    struct list_head list;

    __u32 eqid;
    char *cbm;
};



struct erfc_params{
	struct erfc_ip_map* ip_map;
    struct erfc_ip_map* ipg_map;
	struct erfc_ip_cache* ip_cache;
	struct erfc_ip_cache* ipg_cache;
	int enable_cache;
	int ip_map_index;
	int ipg_map_index;
	int ip_cache_index;
	int ipg_cache_index;
};

#ifdef SUPPORT_ERFC_CACHE

int erfc_set_bit_cache_entry(unsigned char *chunk, int index);
int erfc_set_bit_cache_entry_ip(int index, int type);
int erfc_set_bit_cache_entry_ipg(int index, int type);

int erfc_set_range_cache_entry(unsigned char *chunk, int left, int right);
int erfc_set_range_cache_entry_ip(int start, int end, int type);
int erfc_set_range_cache_entry_ipg(int start, int end, int type);

int erfc_mix_cache_entry(unsigned char* chunk1, unsigned char*chunk2);
int erfc_mix_cache_entry_v2( int idx1, int idx2, int type);

int erfc_check_cache(struct erfc_ip_map *map, int index);
int erfc_check_cache_ip(int index);
int erfc_check_cache_ipg(int index);

int erfc_insert_cache_entry(struct erfc_cache *cache, struct erfc_ip_map*map, int idx);
int erfc_insert_cache_entry_ip(int idx);
int erfc_insert_cache_entry_ipg(int idx);

int erfc_ip_cache_operation(struct erfc_cache*cache, struct erfc_ip_map*map,
		                                     int index,BITMAP**bitmap, int cbm_ith, int jt_index );
int erfc_ip_cache_operation_ip(int index,BITMAP**bitmap, int cbm_ith, int jt_index );
int erfc_ip_cache_operation_ipg(int index,BITMAP**bitmap, int cbm_ith, int jt_index );

#endif


extern int  rfc_preprocess_main(struct omni_xt_table_info *newinfo, void *entry0, const struct omni_ipt_replace *repl);

extern struct erfc_tables erfc_working;
extern struct erfc_tables erfc_preparing;


extern ERFC_ET_ENTRY *rfc_get_cbm_entry(unsigned int hook, struct omni_track_tuple *tuple);
extern struct omni_ipt_entry *rfc_lookup(unsigned int hook, ERFC_ET_ENTRY *ete, struct omni_ipt_entry *entry, int *index, bool *hotdrop, struct omni_policy_map_info *rfc_policy_map);

extern void rfc_switching(void);
extern void rfc_cleanup_reinit(unsigned int hook);

//extern unsigned int on_rfc;
//DEBUG
extern int i_count[10];
#endif
