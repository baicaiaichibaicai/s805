#ifndef __DNS_MANAGER_H__
#define __DNS_MANAGER_H__

#define MATCH_DOMAIN_TRUE			1
#define MATCH_DOMAIN_FALSE			-1

#define MAX_DOMAIN_SIZE				128

#define DNS_MANAGER_DATA_ALLOC_ERR	-1

enum search_mode {
	SEARCH_DOMAIN,
	SEARCH_ADDR
};

enum delete_mode {
	DELETE_DOMAIN,
	DELETE_ADDR
};

enum insert_err {
	COLLISION = -1,
	DATA_COUNT_FULL = -2,
	DATA_ALLOC_FAIL = -3,
	LIST_ALLOC_FAIL = -4
};

struct dns_manager_data {
	char domain[MAX_DOMAIN_SIZE];
	u_int32_t addr;
	struct timer_list timer;
	struct list_head *head;
	struct list_head list;
#ifdef CONFIG_FERRET_DNS_MANAGER_DEBUG
	time_t date;
#endif
};

int get_dns_manager_flag(void);
void set_dns_manager_flag(int arg);
int get_dns_manager_status(void);

void dns_manager_init(void);
void dns_manager(struct sk_buff* skb);

int dns_manager_search(char *domain, u_int32_t *addr, int mode);
int dns_manager_insert(char *domain, u_int32_t addr, u_int32_t ttl);
int dns_manager_delete(void *key, int mode);
int dns_manager_traversal(void (*func)(void*, struct dns_manager_data *), void *arg);
void dns_manager_empty(void);

int is_match_domain(u_int32_t addr, char* domain);

#endif
