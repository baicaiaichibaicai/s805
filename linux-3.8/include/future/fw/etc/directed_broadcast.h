#ifndef __FUTURE_DIRECTED_BROADCAST_H_
#define __FUTURE_DIRECTED_BROADCAST_H_

int dr_broadcast_proc_init(void);

void omni_dr_broadcast_log(struct sk_buff *skb);
int omni_dr_broadcast_check(struct sk_buff *skb);

#endif /* __FUTURE_DIRECTED_BROADCAST_H_ */

