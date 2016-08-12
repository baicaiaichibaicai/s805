/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_EXTEND_H
#define _PROFILE_WEB_EXTEND_H

#include <future/omni/omni_track_extend.h>
#include <future/fw/profiles/profile_web.h>


#define MAX_HTTP_HOST_STR_LEN		124
#define MAX_HTTP_URI_STR_LEN 		124
#define MAX_HTTP_QUERY_STR_LEN 		252


struct web_match_result {
	//unsigned int 		rule_num;
	//unsigned int 		rule_uid;
	//unsigned short 	profile_num;
	unsigned short 		element_num;
	unsigned short		category;
	unsigned int		event;
	int 				rvd;
	web_action_type 	action;
	web_element 		*element;
};

struct web_payload {
	u_int8_t *data;
	int len;
};

struct web_header_request {
	u_int32_t packet_count;
	enum http_method method;
	enum http_content_type content_type;
	bool encoding_disabled;

	int host_len;
	char host[MAX_HTTP_HOST_STR_LEN];
	int uri_len;
	char uri[MAX_HTTP_URI_STR_LEN];
	int query_len;
	char query[MAX_HTTP_QUERY_STR_LEN];
};

struct web_header_response {
	u_int32_t packet_count;
	enum http_content_type content_type;
};

struct omni_ext_web_profile {
	web_action_type target_action;
	struct web_match_result match_result;
	struct web_payload payload;
	struct web_header_request request;
	struct web_header_response response;
};

unsigned int web_match_result_desc(char* desc, int match_result_event,struct omni_ext_web_profile *web);
void do_unclassified_log(struct sk_buff *skb,struct omni_track *track,struct omni_ext_web_profile *web,__be16 id, int dir);

#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_WEB_EXT)
static inline
struct omni_ext_web_profile *web_profile_omni_ext_find(const struct omni_track *track) {
	return omni_track_ext_find(track, OMNI_TRACK_EXT_WEB);
}

static inline
struct omni_ext_web_profile *web_profile_omni_ext_add(struct omni_track *track, gfp_t gfp) {
	return omni_track_ext_add(track, OMNI_TRACK_EXT_WEB, gfp);
}

#else
static inline
struct omni_ext_web_profile *web_profile_omni_ext_find(const struct omni_track *track) {
	return NULL;
}

static inline
struct omni_ext_web_profile *web_profile_omni_ext_add(struct omni_track *track, gfp_t gfp) {
	return NULL;
}

#endif/*CONFIG_FERRET_FW_PROFILES_WEB_EXT*/

#endif/*_PROFILE_WEB_EXTEND_H*/

