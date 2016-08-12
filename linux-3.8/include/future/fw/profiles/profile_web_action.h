/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_ACTION_H
#define _PROFILE_WEB_ACTION_H

#include <linux/skbuff.h>

#define MAX_WEB_APPROVE_IMG_URL_LEN	128
#define MAX_WEB_REDIRECT_LEN		256


typedef enum {
	INVALID_WEB_ACTION = 0,
	WEB_ACCEPT = 0x01,
	WEB_BLOCK,
	WEB_REDIRECTION,
	WEB_APPROVE,
	MAX_WEB_ACTION
} web_action_type;


typedef struct {
	char url[MAX_WEB_REDIRECT_LEN];
	char img_url[MAX_WEB_REDIRECT_LEN];
} web_redirect_opt;

typedef struct {
	char req_img_url[MAX_WEB_APPROVE_IMG_URL_LEN];
	char res_img_url[MAX_WEB_APPROVE_IMG_URL_LEN];
	char bypass_log;
} web_approve_opt;

typedef struct {
	web_action_type type;
	union {
		web_redirect_opt r;
		web_approve_opt a;
	} opt;
} web_action;


unsigned int do_web_action(struct sk_buff *skb, void *nf_ext_web, struct omni_track *track, __be16 id, int dir);

#endif/*_PROFILE_WEB_ACTION_H*/

