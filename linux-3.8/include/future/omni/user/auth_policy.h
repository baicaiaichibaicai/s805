#ifndef _AUTH_POLICY_H_
#define _AUTH_POLICY_H_

#include <linux/types.h>

#define AUTH_POLICY_ANY 1

struct auth_obj {
    __u32 idx;
    __u8 type;
};

struct ap_tbl {
    struct auth_obj *obj;
    __u32 cnt;
    __u8 any;
};

int set_auth_policy(struct ap_tbl *tbl);
int get_auth_policy(struct ap_tbl *tbl);

#endif
