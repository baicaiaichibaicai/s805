/*
 * Future Systems
 */
#ifndef _PROFILE_APP_EXTEND_H
#define _PROFILE_APP_EXTEND_H

#include <future/omni/omni_track_extend.h>
#include <future/fw/profiles/profile_app_action.h>

#define MAX_APP_MATCH_LIST 12

struct app_match_result {
	unsigned short profile_num;
	unsigned short element_num;
	unsigned int sid_num;
	app_action action;
};

struct omni_ext_app_profile {
	unsigned char match_cnt;
	struct app_match_result match_list[MAX_APP_MATCH_LIST];
	unsigned int detect_cnt_list[MAX_APP_MATCH_LIST];
};

#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_APP_EXT)
static inline
struct omni_ext_app_profile *app_profile_omni_ext_find(const struct omni_track *track) {
	return omni_track_ext_find(track, OMNI_TRACK_EXT_APP);
}

static inline
struct omni_ext_app_profile *app_profile_omni_ext_add(struct omni_track *track, gfp_t gfp) {
	return omni_track_ext_add(track, OMNI_TRACK_EXT_APP, gfp);
}

#else
static inline
struct omni_ext_app_profile *app_profile_omni_ext_find(const struct omni_track *track) {
	return NULL;
}

static inline
struct omni_ext_app_profile *app_profile_omni_ext_add(struct omni_track *track, gfp_t gfp) {
	return NULL;
}

#endif/*CONFIG_FERRET_FW_PROFILES_APP_EXT*/

#endif/*_PROFILE_APP_EXTEND_H*/

