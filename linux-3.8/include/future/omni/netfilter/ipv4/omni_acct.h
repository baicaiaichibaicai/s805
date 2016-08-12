#ifndef _OMNI_TRACK_IPV4_ACCT
#define _OMNI_TRACK_IPv4_ACCT

#include <future/omni/filter/omni_ip_tables.h>
#include <future/omni/netfilter/omni_x_tables.h>

struct omni_policy_acct {
	struct omni_filter filter;
	struct omni_xt_counters acct[NR_CPUS][OMNI_TRACK_DIR_MAX];

	unsigned long session[NR_CPUS];

	unsigned int verdict;

	om_obj_limit limit;
};

struct omni_acct {
	struct omni_policy_acct *policy_acct;
	void **acct_map;

	/* Number of entries*/
	unsigned int number;
	unsigned int size;
};

extern struct omni_acct *omni_acct;

extern struct omni_policy_acct *find_acct_map(struct omni_filter *filter);
extern int omni_create_acct_map(struct omni_xt_table_info *xt_info, const char *name);
extern void omni_packet_acct(struct omni_policy_acct *omni_at_map, enum omni_track_info trinfo, const struct sk_buff *skb);
extern void omni_session_acct(struct omni_policy_acct *omni_at_map, enum omni_track_info trinfo, const struct sk_buff *skb);
extern void free_omni_acct(struct omni_acct *acct);

extern unsigned int g_check_use_policy_tracker __read_mostly;
#endif
