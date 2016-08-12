#ifndef _OMNI_DYNAMICS_H
#define _OMNI_DYNAMICS_H
#include <future/omni/netfilter/omni_x_tables.h>
#include <future/omni/netfilter/omni_schedule.h>

#define OMNI_DYNAMIC_BIAS	5
#define OMNI_DYNAMICS_SKIP 	0
#define OMNI_DYNAMICS_EVAL 	1

extern int omni_dynamic_add(struct omni_schedule *sche,
		unsigned int rule_idx,
		struct omni_xt_action_param *acpar);

extern void omni_dynamic_growup(void);
extern bool omni_dynamic_enabled(struct sk_buff *skb);
extern void omni_dynamic_free(struct omni_dynamic *dynamic);
extern void omni_dynamic_do_match(struct sk_buff *skb);

#endif


