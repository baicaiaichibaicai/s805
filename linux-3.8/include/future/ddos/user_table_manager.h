#ifndef __USER_TABLE_MANAGER_H_
#define __USER_TABLE_MANAGER_H_

void init_user(struct ddos_user_table *ddos_user, u8 status);
void init_ddos_user_table(void);
struct ddos_user_table *get_add_ddos_user_table(struct sk_buff *skb);

#endif // USER_TABLE_MANAGER
