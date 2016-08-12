/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_H
#define _PROFILE_WEB_H

#include <linux/time.h>
#include <linux/spinlock_types.h>

#include <future/fw/profiles/profiles.h>
#include <future/fw/profiles/profile_web_action.h>
#include <future/fw/profiles/profile_web_match.h>


#define WEB_PROFILE_OFFSET	10
#define WEB_PROFILE_SO_SET	(PROFILE_BASE_CTL + WEB_PROFILE_OFFSET)
#define WEB_PROFILE_SO_GET	(PROFILE_BASE_CTL + WEB_PROFILE_OFFSET)

#define WEB_PROFILE_CMD_ADD		0x71
#define WEB_PROFILE_CMD_DEL		0x72
#define WEB_PROFILE_CMD_STATUS	0x75
#define WEB_PROFILE_CMD_LIST	0x76
#define WEB_PROFILE_CMD_LIST2	0x77


enum http_content_type {
	HTTP_CT_UNKNOWN,
	HTTP_CT_IMAGE,
	HTTP_CT_APPLICATION,
	HTTP_CT_TEXT_HTML,
	HTTP_CT_TEXT_OTHER,
	HTTP_CT_AUDIO,
	HTTP_CT_VIDEO,
	HTTP_CT_MULTIPART,
	HTTP_CT_MESSAGE,
	HTTP_CT_MODEL,
	HTTP_CT_OTHERS
};

enum http_method {
	HTTP_MTD_UNKNOWN,
	HTTP_MTD_GET,
	HTTP_MTD_POST,
	HTTP_MTD_PUT,
	HTTP_MTD_DELETE,
	HTTP_MTD_HEAD,
	HTTP_MTD_OPTIONS,
	HTTP_MTD_PATCH,
	HTTP_MTD_OTHERS
};


typedef struct {
	web_action act;
	web_match match;
} web_element;

typedef struct {
	int index;

	unsigned short element_cnt;
	web_element **elements;
} web_profile;


extern void omni_policy_web_profile_prepare(void);
extern void omni_web_profile_release(void);
extern web_profile *omni_policy_get_master_web(void);
extern web_profile *omni_policy_get_backup_web(void);


extern web_profile *omni_get_web_profile_list(void);
extern web_profile *omni_get_web_profile(unsigned short);
extern int omni_web_profile_swap(void);
extern web_profile *omni_web_profile_base_get(int id);
extern int omni_web_profile_secondary_free_all(void);
extern void omni_web_profile_debug(web_profile *profile);
extern int omni_web_profile_reset_list(int id);

int omni_get_web_profile_max_list(void);
int omni_get_web_profile_memory_usage(void);

#endif/*_PROFILE_WEB_H*/

