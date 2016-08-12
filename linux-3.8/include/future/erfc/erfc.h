#ifndef __ERFC_H__
#define __ERFC_H__

#include <linux/list.h>
#include <linux/spinlock.h>


//#define CONFIG_ERFC_DEBUG


#define CV_HISTORY_CNT	10
#define CURRENT_CHUNKS_CNT 10		//current chunk count is 10 , Don't modify it

#define ERFC_BLOCK_SIZE (sizeof(BITMAP) * 8)	/* "A" in erfc paper */
#define BITMAP uint64_t
#define ERFC_MAX_CHUNKS 13		/* max 13 chunks for any erfc_tree */

/* jump table (JT) */
typedef uint32_t *LV0;			/* no compression */
typedef struct {
	uint8_t *i;
	uint32_t *eq_id;
} LV1;					/* medium compression */
typedef uint32_t LV2;			/* max compression */

typedef struct {
	uint8_t eq_ids;
	union {
		LV0 lv0;
		LV1 lv1;
		LV2 lv2;
	} e;
} ERFC_JT_ENTRY;

typedef struct {
	int64_t entries;
	int64_t max_entries;

	ERFC_JT_ENTRY *entry;		/* vmalloc */
} ERFC_JT;


/* hash table (HT) */
typedef struct {
	uint32_t eq_id;

	struct list_head entry;
} ERFC_HT_ENTRY;

typedef struct {
	int entries;

	struct list_head *entry;	/* vmalloc */
} ERFC_HT;


/* eq-class table (ET) */
typedef struct {
	BITMAP *z;
	BITMAP *y;
	BITMAP *x;
	BITMAP r;
	BITMAP d;

	int first_idx;
} ERFC_ET_ENTRY;	/* == EBV (Extended Bit Vector) */

typedef struct {
	int entries;
	int max_entries;
	int rules;

	ERFC_ET_ENTRY **entry;		/* vmalloc */
} ERFC_ET;


/* ERFC */
typedef struct {
	ERFC_JT *jt;
	ERFC_HT *ht;
	ERFC_ET *et;

	int et_size;
} ERFC_CHUNK;

struct erfc_tables {
    int num_rules;
    int num_chunks;

    ERFC_CHUNK *chunk[ERFC_MAX_CHUNKS];

    int reserved_size_scale;

    unsigned char init;
    u8 memkeep;
};


typedef struct
{
    int chunk[CV_HISTORY_CNT][CURRENT_CHUNKS_CNT];		//chunks count must be 10
    int current_index;
} CV_HISTORY;

extern rwlock_t erfc_lock;
extern struct erfc_tables erfc_working, erfc_preparing;


/* ERFC APIs */
int	erfc_build(struct erfc_tables *p_table, struct omni_xt_table_info*newinfo);
ERFC_ET_ENTRY *		erfc_match(struct erfc_tables *p_table, int *trace);
const char *		erfc_tree(void);
void			erfc_cleanup(struct erfc_tables *p_table);

/* bitmap operations */
void			erfc_set_bit(int index, BITMAP *bitmap);
int			erfc_test_bit(int index, BITMAP *bitmap);
void			erfc_clear_bit(int index, BITMAP *bitmap);
int			erfc_first_bit(BITMAP *bitmap, int rules);

extern unsigned int on_rfc;
extern struct proc_dir_entry*proc_ferret_fw;
extern unsigned int under_list[7];
extern unsigned int under_flag[7];
extern unsigned int upper_list[7];

#endif /* __ERFC_H__ */
