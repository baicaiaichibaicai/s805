#ifndef DDOS_UTIL_H_
#define DDOS_UTIL_H_

#include <linux/proc_fs.h>
#include <future/ddos/flooding_detector.h>

struct ddos_profile_table *get_ddos_profile_data(int index);


struct ddos_user_table *get_ddos_user_data(void);
void *get_ddos_user_tlsfmem(void);
void set_ddos_user_tlsf_size(size_t size);
void add_ddos_user_tlsf_size(size_t size);
size_t get_ddos_user_tlsf_size(void);

void set_ddos_profile_bootmem_size(size_t size );
size_t get_ddos_profile_bootmem_size(void);
void set_ddos_profile_bootmem_start(void *p);
void *get_ddos_profile_bootmem_start(void);
void set_ddos_profile_bootmem_end(void *p);
void *get_ddos_profile_bootmem_end(void);

void set_ddos_user_bootmem_size(size_t size);
size_t get_ddos_user_bootmem_size(void);
void set_ddos_user_bootmem_start(void *p);
void *get_ddos_user_bootmem_start(void);
void set_ddos_user_bootmem_end(void *p);
void *get_ddos_user_bootmem_end(void);

void init_ddos_tables(void);
void ddos_data_table_alloc(void);
void set_ddos_sfxhash_table_memory(void);

#define DDOS_MEM_POOL_INIT init_memory_pool

struct ddos_free_node {
	int data;
	struct list_head list;
};

#if 0
#define FddosFree(x) tlsf_free(x, get_g_ddos_mem_start())

static inline void *FddosAlloc(unsigned long size)
{
	void *tmp;

	tmp = (void *) tlsf_malloc(size, get_g_ddos_mem_start());

	if (tmp == NULL) {
		printk("DDOS - Unable to allocate memory!  (%lu requested)\n", size);
	}

	return tmp;
}

#endif

#endif // DDOS_UTIL_H_ 
