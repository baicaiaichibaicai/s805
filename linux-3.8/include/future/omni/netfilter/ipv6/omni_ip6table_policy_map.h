#ifndef _OMNI_IP6TABLE_POLICY_MAP_H
#define _OMNI_IP6TABLE_POLICY_MAP_H

#include <future/omni/omni_track_compat.h>
#include <future/omni/netfilter/ipv6/omni_ip6_tables.h>

struct omni6_ipt_address {

	struct omni_ip *ipv6;
	struct omni_env *env;
	/* TODO
	   domain
	   user
	   country (cc)
	 */
};

struct omni6_ipt_service {
	struct omni_service *service;
	//struct omni_service_group *service_group; //TODO
};

struct omni6_ipt_schedule {
	struct omni_schedule *schedule;
};

struct omni6_ipt_ipv6header {
	struct omni6_ipv6header *ipv6header;
};


struct omni6_ipt_resource {
	struct omni_limit *limit;
};

struct omni6_policy_map {
	struct omni_ip6t_entry *e;

	struct omni6_ipt_address addr;
	struct omni6_ipt_service serv;

	struct omni6_ipt_schedule sche;
    struct omni6_ipt_ipv6header head;
	struct omni6_ipt_resource reso;
};

struct omni6_policy_map_info {
	unsigned int size;
	unsigned int nr_entries;

	unsigned int hook_index[OMNI_INET_NUMHOOKS];

	struct omni6_policy_map map[1];
};

extern int omni6_create_policy_map(struct omni_xt_table_info *newinfo, 
		void *entry0, const struct omni_ip6t_replace *repl);

extern void omni6_destroy_policy_map(struct omni6_policy_map_info *info);
extern void omni6_print_policy_map(struct omni6_policy_map_info *info);
extern unsigned int omni6_policy_map_index(struct omni6_policy_map_info *mapinfo, int hook);
extern struct omni6_policy_map *omni6_policy_map_get(struct omni6_policy_map_info *mapinfo, int index);

#endif

