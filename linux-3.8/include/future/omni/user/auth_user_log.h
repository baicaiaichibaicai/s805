#ifndef _AUTH_USER_LOG_H_
#define _AUTH_USER_LOG_H_

enum auth_user_type {
    AU_TYPE_IN_SELF = 0,
    AU_TYPE_IN_ADMIN,
    AU_TYPE_IN_SSO,
    AU_TYPE_OUT_SELF,
    AU_TYPE_OUT_ADMIN,
    AU_TYPE_OUT_SSO,
    AU_TYPE_OUT_IDLE,
    AU_TYPE_OUT_SAME_IP,
    AU_TYPE_OUT_SAME_ID
};

enum auth_user_log_res {
    AU_LOG_RES_IN_S_SELF = 0,
    AU_LOG_RES_IN_S_ADMIN,
    AU_LOG_RES_IN_S_SSO,
    AU_LOG_RES_IN_F_ALREADY_LOGIN,
    AU_LOG_RES_IN_F_IDPW,
    AU_LOG_RES_IN_F_SHARED,
    AU_LOG_RES_IN_F_RESTRICT,
    AU_LOG_RES_IN_F_EXCEED,
    AU_LOG_RES_IN_F_SRV_ERROR,
    AU_LOG_RES_IN_F_NO_SRV,
    AU_LOG_RES_IN_F_ERROR,
    AU_LOG_RES_OUT_S_SELF,
    AU_LOG_RES_OUT_S_ADMIN,
    AU_LOG_RES_OUT_S_SSO,
    AU_LOG_RES_OUT_S_TIMEOUT,
    AU_LOG_RES_OUT_S_SAME_IP,
    AU_LOG_RES_OUT_S_SAME_ID,
    AU_LOG_RES_OUT_F_NO_USER,
    AU_LOG_RES_OUT_F_ERROR
};

struct au_log_res {
    u_int8_t rescode;
    const char *msg;
    u_int32_t event;
};

struct au_log_utype {
    u_int8_t type;
    u_int32_t event;
};

#endif
