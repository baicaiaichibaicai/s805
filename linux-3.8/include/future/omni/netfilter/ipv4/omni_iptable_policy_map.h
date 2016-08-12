#ifndef _OMNI_IPTABLE_POLICY_MAP_H
#define _OMNI_IPTABLE_POLICY_MAP_H

#include <future/omni/omni_track_compat.h>



struct omni_ipt_address {
	struct omni_ip *ip4;
	struct omni_cc *cc;
	struct omni_domain *domain;
	struct omni_env *env;
	struct omni_user *user;
};

struct omni_ipt_service {
	struct omni_service *service;
};

struct omni_ipt_schedule {
	struct omni_schedule *schedule;
};

struct omni_ipt_resource {
	struct omni_limit *limit;
};

struct omni_policy_map {
	struct omni_ipt_entry *e;
	bool tproxy_socket;

	struct omni_ipt_address addr;
	struct omni_ipt_service serv;

	struct omni_ipt_schedule sche;
	
	struct omni_ipt_resource reso;
};

struct omni_policy_map_info {
	unsigned int size;
	unsigned int nr_entries;

	unsigned int hook_index[OMNI_INET_NUMHOOKS];

	struct omni_policy_map map[1];
};

extern int omni_create_policy_map(struct omni_xt_table_info *newinfo, 
		void *entry0, const struct omni_ipt_replace *repl);


extern void omni_destroy_policy_map(struct omni_policy_map_info *info);
extern void omni_print_policy_map(struct omni_policy_map_info *info);
extern unsigned int omni_policy_map_index(struct omni_policy_map_info *mapinfo, int hook);
extern struct omni_policy_map *omni_policy_map_get(struct omni_policy_map_info *mapinfo, int index);

#endif
