#ifndef _AUTH_USER_H_
#define _AUTH_USER_H_

#include <linux/types.h>

#define MAX_USER_ID_LEN  32
#define MAX_OBJ_NAME_LEN 32
#define MAX_SRV_NAME_LEN 32

enum auth_user_result {
    AU_EXCEED_LIMIT = -4,
    AU_NO_USER = -3,
    AU_ALREADY_LOGIN = -2,
    AU_FAIL = -1,
    AU_SUCCESS = 0,
};

enum auth_user_status {
    AU_NOT_FOUND = 0,
    AU_FOUND
};

enum auth_user_act {
    AU_LOGIN = 1,
    AU_LOGOUT,
    AU_LOGOUT_ALL,
    AU_LIST
};

struct auth_user {
    char id[MAX_USER_ID_LEN];
    char srv[MAX_SRV_NAME_LEN];
    char obj[MAX_OBJ_NAME_LEN];
    atomic64_t byt_tx;
    atomic64_t byt_rx;
    atomic64_t byt_all;
    atomic64_t sess;
    atomic64_t pkt_tx;
    atomic64_t pkt_rx;
    atomic64_t pkt_all;
    __u32 ip;
    __u32 ltime;
    __u32 etime;
    __u32 to;
    __u8 log_user_type;
    struct list_head list;
    struct timer_list idle_to;
};

struct auth_user_tbl {
    struct auth_user *user;
    int res;
    __u32 cnt;
    __u8 act;
};

int init_auth_user(void);
int au_list_head_free(void* key, void* data);
int auth_user_login(struct auth_user_tbl *tbl);
int auth_user_logout(struct auth_user_tbl *tbl);
int auth_user_logout_all(struct auth_user_tbl *tbl);
int set_auth_user(struct auth_user_tbl *tbl);
int get_auth_user(struct auth_user_tbl *tbl);
void auth_user_timeout(unsigned long ul_userid);

#endif
