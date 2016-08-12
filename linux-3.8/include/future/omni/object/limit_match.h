#ifndef _LIMIT_MATCH_H_
#define _LIMIT_MATCH_H_

#include <future/omni/omni_track_tuple.h>
#include <future/omni/netfilter/ipv4/omni_acct.h>

enum session_limit{
	SESSION_LIMIT_BIT = 1,
	SESSION_LIMIT = (1 << SESSION_LIMIT_BIT),

	USER_LIMIT_BIT = 2,
	USER_LIMIT = (1 << USER_LIMIT_BIT),

	TRAFFIC_LIMIT_BIT = 3,
	TRAFFIC_LIMIT = (1 << TRAFFIC_LIMIT_BIT),
};

//extern int match_limit(const omni_om_entry *entry, const struct omni_track_tuple *t, const int len, const int d_fwd);
extern int match_limit(const omni_om_entry *entry, 
					   const struct omni_track_tuple *t, 
					   const int len, 
					   struct sk_buff *skb,
					   struct omni_ipt_entry *e);
extern void put_limit_entry(struct omni_policy_acct *at_map, struct omni_track *track);
extern int omni_acct_limit_set(struct omni_xt_table_info *newinfo);
extern int omni_traffic_limit(struct sk_buff *skb);

#endif

