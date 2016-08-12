/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_MATCH_H
#define _PROFILE_WEB_MATCH_H

#include <future/command/vty.h>
#include <linux/skbuff.h>

#define HTTP_URL_DB_CATEGORY_SIZE 32
#define URL_HASH_SIZE 16


#define MAX_URL_PATTERN_LEN 64
#define MAX_URL_PATTERN_EXP_LEN 128
#define MAX_PICS_RATE 6

#define HTTP_PROFILE_MATCH			0
#define HTTP_PROFILE_NOT_MATCH     -1

typedef enum {
	INVALID_WEB_MATCH = 0,
	WEB_URL_PATTERN_MATCHING = 'U',			// 85
	WEB_RESPONSE_PACKET_FILTERING = 'P',	// 80
	WEB_KISCOM_DB = 'K',					// 75
	WEB_SELF_RATING = 'E',					// 69
	WEB_YOUTH_HARMFUL = 'Y',				// 89
	WEB_URL_FILTERING = 'I'					// 73
} web_match_type;

typedef enum {
	INVALID_UPM_TYPE = 0,
	UPM_ALL,
	UPM_REQUEST,
	UPM_RESPONSE,
	MAX_UPM_TYPE
} url_pattern_matching_type;

typedef enum {
	INVALID_RPF_TYPE = 0,
	RPF_TEXT = 0x01,
	RPF_FILE = 0x02,
	RPF_TEXTAREA = 0x04,
	RPF_MULTIPART = 0x08,
} reponse_packet_filtering_type;


typedef struct {
	url_pattern_matching_type type;
	int pattern_cnt;
	char (*pattern)[MAX_URL_PATTERN_LEN];
	void *m_wmb;
//	void **prog;

} url_pattern_matching;
typedef struct {
	unsigned int type;
} response_packet_filtering;

typedef struct {
	unsigned char rate[MAX_PICS_RATE];
} pics_rating;

typedef struct {
	char is_use;
} youth_harmful_site;

typedef struct {
	int index_cnt;
	unsigned short *index;
} url_filtering;


typedef struct {
	web_match_type type;
	union {
		url_pattern_matching 		u;
		response_packet_filtering 	r;
		pics_rating 				p;
		youth_harmful_site 			y;
		url_filtering 				f;
	} opt;
} web_match;


int do_request_web_match(struct sk_buff *skb,web_match *match, int action, void *nf_ext_web);
int do_response_web_match(struct sk_buff *skb,web_match *match, int action, void *nf_ext_web);
void manual_search_url_pattern(struct vty *vty, char *p_url, int url_len);
#endif/*_PROFILE_WEB_MATCH_H*/

