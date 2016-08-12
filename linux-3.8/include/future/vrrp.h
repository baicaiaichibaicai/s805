#ifndef __VRRP_H__
#define __VRRP_H__

#include <future/general.h>


struct vrrp_priority_table {
    unsigned long	jiffies;
    int 		priority;
};

extern struct vrrp_priority_table	gz_vrrp_rcv_priority[VRRP_TABLE_SIZE];

void reset_vrrp_rcv_priority_info(void);

int check_ha_none_mode_ha_state(int type);
void get_vrrp_rcv_priority_info(void *p_param);
void parse_vrrp_packet(struct sk_buff *skb);


#endif
