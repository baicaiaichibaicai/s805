#ifndef _AUTH_USER_MATCH_H_
#define _AUTH_USER_MATCH_H_

int auth_user_find_ip(struct sk_buff *skb);
int auth_user_find_id(const char *userid);
int get_auth_userid(struct omni_track *track, char *buf, int len);
int get_auth_user_obj_name(const char *userid, char *buf, int len);
int get_auth_userid_by_ip(__u32 ipv4, char *buf, int len);
void ua_set_userid(struct omni_track *track);
void ua_set_acct(struct omni_track *track, const struct sk_buff *skb);

#endif
