/*
 * Future Systems
 */
#ifndef _OMNI_TRACK_FIREWALL_H
#define _OMNI_TRACK_FIREWALL_H


struct profile_num_info {
#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_WEB)
	unsigned short web;
#endif/*CONFIG_FERRET_FW_PROFILES_WEB*/
#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_APP)
	unsigned short app;
#endif/*CONFIG_FERRET_FW_PROFILES_APP*/
};

struct filter_policy_info {
	unsigned int num; //XTM: conntrack->policy_num (unsigned int)
	unsigned int uid; //XTM: conntrack->unique_id (unsigned int)
	struct profile_num_info profile_num;
};

struct nat_policy_info {
	unsigned short num; //XTM: conntrack->nat_pnum (unsigned short)
	unsigned short uid; //XTM: conntrack->nat_unique_id (unsigned short)
};

struct firewall_info {
	struct filter_policy_info filter;
	struct nat_policy_info nat;
};

#endif/*_OMNI_TRACK_FIREWALL_H*/

