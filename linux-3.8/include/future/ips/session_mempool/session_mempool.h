
#ifndef _SESSION_MEMPOOL_H
#define _SESSION_MEMPOOL_H

#include "lp_list_types.h"

#define SESSION_MEMPOOL_SIZE	16384 // 16K

struct block_session_info {
	int block_count;
	long long int block_bytes;
};

struct session_block_info {
	unsigned long block_key;
	unsigned long session_id;
	atomic_t block_count;
	atomic64_t block_bytes;
	//struct block_session_info rx;
	//struct block_session_info tx;
};

struct ips_block_session_info {
	struct list_head list;

	uint32_t start_time;
	uint32_t end_time;
	uint32_t spd_id;
	uint32_t fsid;
	int block_type;
	uint16_t ip_type;
	uint16_t protocol;
	uint16_t src_port;
	uint16_t dest_port;
	__be32 saddr;
	__be32 daddr;
	__be32 saddr_6[4];
	__be32 daddr_6[4];

	struct session_block_info *packet_count; 
};

typedef struct _SessionMemPool
{
	void **datapool; 

	LPListItem *listpool; 

	int total;

	LPList free_list;
	LPList used_list;

	size_t obj_size;
} SessionMemPool;

int session_mempool_init(SessionMemPool *mempool, int num_objects, size_t obj_size);
int session_mempool_destroy(SessionMemPool *mempool);
int session_mempool_alloc(SessionMemPool *mempool, int *index);
int session_mempool_free(SessionMemPool *mempool, int *index);
int session_mempool_check(SessionMemPool *mempool, int index);
void *session_get_alloc_memory(SessionMemPool *mempool, int index);

#endif /* _SESSION_MEMPOOL_H */



