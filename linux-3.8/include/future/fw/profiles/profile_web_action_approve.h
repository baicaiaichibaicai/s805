/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_ACTION_APPROVE_H
#define _PROFILE_WEB_ACTION_APPROVE_H

#include <net/ip.h>


#define APPROVE_UNKNOWN_STATUS -1
#define APPROVE_REQUESTED_STATUS 1
#define APPROVE_FINISH_STATUS 0

#define APPROVE_MARK_HEADER "/http+"
#define APPROVE_MARK_HEADER_LEN 6
#define APPROVE_MARK_TOTAL_LEN 22 // 6(/http+) 8(jiffie) * 2

#define GRAY_URL_LIST_CNT 0x10000


struct gray_url {
    struct gray_url *p_next;
    char *host_name;
    time_t hit;
};

struct _int2short {
    unsigned short high;
    unsigned short low;
};


int get_approve_status(struct sk_buff *, char *, char *, char *);
int add_approved_host_list(struct iphdr *, char *);

int init_approve_host_list(void);
void exit_approve_host_list(void);

#endif/*_PROFILE_WEB_ACTION_APPROVE_H*/

