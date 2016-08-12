/*
 * Future Systems
 */
#ifndef _PROFILE_APP_H
#define _PROFILE_APP_H

#include <future/fw/profiles/profiles.h>
#include <future/fw/profiles/profile_app_action.h>
#include <future/fw/profiles/profile_app_match.h>


#define APP_PROFILE_OFFSET	11
#define APP_PROFILE_SO_SET	(PROFILE_BASE_CTL + APP_PROFILE_OFFSET)
#define APP_PROFILE_SO_GET	(PROFILE_BASE_CTL + APP_PROFILE_OFFSET)

#define APP_PROFILE_CMD_ADD		0x71
#define APP_PROFILE_CMD_DEL		0x72
#define APP_PROFILE_CMD_STATUS	0x75
#define APP_PROFILE_CMD_LIST	0x76


typedef struct {
	app_action action;
	app_match match;
} app_element;

typedef struct {
	int index;

	unsigned short element_cnt;
	app_element **elements;
} app_profile;


app_profile *get_app_profile_list(int id);
app_profile *get_app_profile(unsigned short);
int get_app_profile_max_list(void);
int get_app_element_max_list(void);
int get_app_user_sig_max(void);
int get_app_profile_memory_usage(void);

extern app_profile *omni_policy_get_master_app(void);
extern app_profile *omni_policy_get_backup_app(void);

//int switch_app_profile(void);
void omni_profile_app_prepare(void);
void omni_profile_app_release(void);

#endif/*_PROFILE_APP_H*/


