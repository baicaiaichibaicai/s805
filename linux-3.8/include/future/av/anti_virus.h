#ifndef _ANTI_VIRUS_Hn
#define _ANTI_VIRUS_H

#define SMTP_SERVICE	1
#define POP3_SERVICE	2
#define IMAP_SERVICE	3
#define HTTP_SERVICE	4
#define FTP_SERVICE		5

#include <future/fw/profiles/profile_web_action_common.h>
#include <future/fw/profiles/profile_web_action_approve.h>
#include <future/fw/profiles/profile_web_extend.h>

#define AVS_MAX_HTTP_HOST_STRLEN 	124
#define AVS_MAX_HTTP_URI_STRLEN		124
#define AVS_MAX_HTTP_QUERY_STRLEN	252

#define MAX_AVS_PROFILE 			64       

#define MAX_VIRUS_INFO				110000
#define MAX_VIRUS_NAME_LEN			64

#define AV_COMMON_HTTP_HEADER_FMT \
"HTTP/1.1 %d %s\r\n" \
"Content-Type: text/html; charset=UTF-8\r\n" \
"\r\n"

/*** for approve page ***/
#define AV_APPROVE_REQUEST_BODY_COMMON_HEAD_FMT \
"<html>" \
	"<head>" \
		"<script type=text/JavaScript>\n" \
			"<!--\n" \
				"var l=\"%s%s%s\"\n" \
				"function a(){window.location=l+\"/http+%08x%08x\"}\n" \
				"function b(){if(top!=self) window.location=l+\"/http+%08x%08x\"}\n" \
			"//-->\n" \
		"</script>" \
	"</head>" \
	"<body onload=\"b();return true\">" \
		"<center><br><br><b>\"%s\"</b><br><br>" \
			"<table>" \
				"This site contains dangerous content(infected virus).<br>" \
				"Click the \"Access\" button to use the site for business.<br>" \
				"Access information will be reported to the admin." \
			"</table>" \
			"<br>" \
			"<button onclick=\"a();\">Access</button>" \
			"<button onclick=\"window.open('about:blank','_self').close();\">Close</button>" \
		"</center>" \
	"</body>" \
"</html>"

/*** for approved page ***/
#define WEB_META_REFRESH_BODY_COMMON_AV_HEAD_FMT \
"<html>" \
	"<head></head>" \
	"<meta http-equiv=\"Refresh\" content=\"3; URL=%s%s%s\">" \
	"<body>" \
		"<center><br><br><b>\"%s\"</b><br><br>" \
			"<table>"

/* Format for meta refresh html (option_)
 * no argument
 */
#define WEB_META_REFRESH_BODY_OPTION_AV_TXT_FMT \
				"Please wait...<br>" \
				"After reporting, access information, go to the site.<br><br>" \

/* Format for meta refresh html (option_img)
 * argument list
 * 1.string: image url
 */
#define WEB_META_REFRESH_BODY_OPTION_AV_IMG_FMT \
				"<img src=\"%s\">"

/* Format for meta refresh html (common_tail)
 * no argument
 */
#define WEB_META_REFRESH_BODY_COMMON_AV_TAIL_FMT \
			"</table>" \
		"</center>" \
	"</body>" \
"</html>"

#define WEB_META_REFRESH_AV_TXT_FMT \
	WEB_META_REFRESH_BODY_COMMON_AV_HEAD_FMT \
	WEB_META_REFRESH_BODY_OPTION_AV_TXT_FMT \
	WEB_META_REFRESH_BODY_COMMON_AV_TAIL_FMT

#define WEB_META_REFRESH_AV_IMG_FMT \
	WEB_META_REFRESH_BODY_COMMON_AV_HEAD_FMT \
	WEB_META_REFRESH_BODY_OPTION_AV_IMG_FMT \
	WEB_META_REFRESH_BODY_COMMON_AV_TAIL_FMT

struct virus_info{
	u32 virus_count;
	char virus_name[MAX_VIRUS_INFO][MAX_VIRUS_NAME_LEN];
};

struct avs_config{
	u32 profile_id;
	u32 http_action;
	u32 ftp_action;
	u32 mail_action;
};

struct ftp_av_profile{
	struct omni_track_tuple *up_tuple;
	struct omni_track_tuple *down_tuple;
	u_short down_request;
	u_short up_request;
	u32	syn_sequence;
	u32 syn_ack_sequence;
};

struct mail_av_profile{
	struct omni_track_tuple *tuple;
	int mail_service;
};

#define ENABLE_PROFILE 0
#if ENABLE_PROFILE
struct http_av_profile{
	
};
#endif

/*** for approve page ***/
struct avs_http_request{
	int host_len;
	char host[AVS_MAX_HTTP_HOST_STRLEN];
	int uri_len;
	char uri[AVS_MAX_HTTP_URI_STRLEN];
	int query_len;
	char query[AVS_MAX_HTTP_QUERY_STRLEN];
};

struct avs_http_page{
	struct avs_http_request request;	
};

/*** flag ***/
extern struct avs_config *av_config;
extern int set_config_flag;

/***********************/
/*** function declaration ***/
int do_anti_virus(struct sk_buff *skb);
int av_load(char *file_name);
int init_stream_av(void);
void exit_stream_av(void);
void send_av_ftp_message(struct sk_buff *oldskb, int hook, char *data, u_int16_t data_len);

#endif
