
#ifndef FIPS_UTIL_H_
#define FIPS_UTIL_H_

#include <linux/kernel.h>
#include <future/lib/tlsf_malloc.h>

/////////////////////////////////////////
// 전역변수

extern unsigned ips_mem;
extern int search_method;
extern int g_main_memory;
extern void *g_ips_mem_start;
extern void *g_ips_mem_end;
extern size_t g_ips_mem_size;
extern void *g_ips_mem_start1;

static inline int       get_g_main_memory(void)			{ return g_main_memory;}
static inline unsigned  get_ips_mem(void)				{ return ips_mem;}
static inline void     *get_ips_mem_start(void)			{ return g_ips_mem_start;}
static inline void     *set_ips_mem_start(void *p_addr)	{ return g_ips_mem_start = p_addr;}
static inline void     *get_ips_mem_end(void)			{ return g_ips_mem_end;}
static inline void     *set_ips_mem_end(void *p_addr)	{ return g_ips_mem_end = p_addr;}
static inline int       get_ips_mem_size(void)			{ return g_ips_mem_size;}
static inline int       set_ips_mem_size(int size)		{ return g_ips_mem_size = size;}
static inline int 		 get_search_method(void)		{ return search_method;}
static inline void     *get_ips_mem_node(int node)      { if(node ==0) return g_ips_mem_start; \
                                                          else return g_ips_mem_start1; }
                                                          //else return *g_ips_mem_start1; }



//void ips_malloc_init(void *, void *);
void ips_malloc_init(void *, void *, void *);
void ips_bootmem_init(void);

int fips_read_file(const char *file_name, char **buf);

#define WHICH_MEM_POOL_INIT  init_memory_pool
#define FipsFree(x) tlsf_free(x, get_ips_mem_start())
//#define FipsCalloc(e,s) tlsf_calloc(e, s, get_ips_mem_start()) 
//#define FipsStrdup(s) rtl_strdup(s, get_ips_mem_start())
#define FipsFree_node(node, x) tlsf_free(x, get_ips_mem_node(node))

//#if 0
static inline void *FipsAlloc(unsigned long size)
{
    void *tmp;

    tmp = (void *) tlsf_malloc(size, get_ips_mem_start());

    if (tmp == NULL) {
        printk("IPS - Unable to allocate memory!  (%lu requested) %lu MB in %s %d\n", 
				size, size >> 20, __FUNCTION__, __LINE__);
		return NULL;
    }

    return tmp;
}

extern int g_tlsf_debug;
static inline void *FipsCalloc(unsigned long count, unsigned long size)
{
    void *tmp;
    unsigned long calloc_size = count*size;

    tmp = (void *) tlsf_malloc(calloc_size, get_ips_mem_start());

    if (tmp == NULL) {
        printk("IPS - Unable to allocate memory(FipsCalloc)!  (%lu requested) %lu MB in %s %d\n", 
				calloc_size, calloc_size >> 20, __FUNCTION__, __LINE__);
        g_tlsf_debug = 1;
        tmp = tlsf_malloc(calloc_size, get_ips_mem_start());
        printk("IPS Memory Error(Calloc) memory:%p, Total: %dkB, Used: %dkB, Free: %dkB caller(%ps) size:%lu\n", 
                tmp, (int)get_ips_mem_size() >> 10, (int)get_used_size(get_ips_mem_start()) >> 10, 
                (int)(get_ips_mem_size() - get_used_size(get_ips_mem_start())) >> 10, __builtin_return_address(0), calloc_size); 
        g_tlsf_debug = 0;
		return NULL;
    }
	memset(tmp, 0, calloc_size);

    return tmp;
}

static inline void *FipsStrdup(const char *p_str)
{
	void *tmp;
	int size = 0;

	size = strlen(p_str);
	if(!size)
	{
		return NULL;
	}

	tmp = FipsAlloc(size+1);
	if(tmp)
	{
    	strcpy(tmp, p_str);
    	return tmp;
	}

    g_tlsf_debug = 1;
    tmp = FipsAlloc(size+1);
    printk("IPS Memory Error(Strdup) memory:%p, Total: %dkB, Used: %dkB, Free: %dkB caller(%ps) size:%d\n", 
            tmp, (int)get_ips_mem_size() >> 10, (int)get_used_size(get_ips_mem_start()) >> 10, 
            (int)(get_ips_mem_size() - get_used_size(get_ips_mem_start())) >> 10, __builtin_return_address(0), size+1); 
    g_tlsf_debug = 0;
    
	return NULL;
}
//#endif

static inline void *FipsAlloc_node(int node, unsigned long size)
{
    void *tmp;

    tmp = (void *) tlsf_malloc(size, get_ips_mem_node(node));
	if (tmp == NULL) {
        printk("IPS - Unable to allocate memory for node(%d)!  (%lu requested) %lu MB in %s %d\n", 
                node, size, size >> 20, __FUNCTION__, __LINE__);
        return NULL;
    }

    return tmp;
}

static inline void *FipsCalloc_node(int node, unsigned long count, unsigned long size)
{
    void *tmp;
    unsigned long calloc_size = count*size;

    tmp = (void *) tlsf_malloc(calloc_size, get_ips_mem_node(node));
    if (tmp == NULL) {
        printk("IPS FipsCalloc_node - Unable to allocate memory for node(%d)!  (%lu requested) %lu MB in %s %d\n",
                node, size, size >> 20, __FUNCTION__, __LINE__);
        return NULL;
    }
    memset(tmp, 0, calloc_size);

    return tmp;
}

#endif // FIPS_UTIL_H_
