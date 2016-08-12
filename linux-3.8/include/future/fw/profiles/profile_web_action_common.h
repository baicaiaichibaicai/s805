/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_ACTION_COMMON_H
#define _PROFILE_WEB_ACTION_COMMON_H

#include <linux/skbuff.h>


#define MAX_PAGE_PALOAD (1500 - LL_MAX_HEADER - sizeof(struct iphdr) - sizeof(struct tcphdr))


/* Format for common http header
 * 1.unsigned int: response code
 * 2.string: code name
 * ex) 200, "OK"
 */
#define WEB_COMMON_HTTP_HEADER_FMT \
"HTTP/1.1 %d %s\r\n" \
"Content-Type: text/html; charset=UTF-8\r\n" \
"\r\n"


/* Format for approve request html (common_head)
 * argument list
 * 1.string: target host
 * 2.string: target uri
 * 3.string: target query
 * 4.unsigned int: saddr (odd approve makrt)
 * 5.unsigned int: daddr (odd approve makrt)
 * 6.unsigned int: saddr (even approve makrt)
 * 7.unsigned int: daddr (even approve makrt)
 * 8.string: target host
 */
#define WEB_APPROVE_REQUEST_BODY_COMMON_HEAD_FMT \
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
			"<table>"

/* Format for approve request html (option_txt)
 * no argument
 */
#define WEB_APPROVE_REQUEST_BODY_OPTION_TXT_FMT \
				"This site is not approved.<br>" \
				"Click the \"Access\" button to use the site for business.<br>" \
				"Access information will be reported to the admin." \

/* Format for approve request html (option_img)
 * argument list
 * 1.string: image url
 */
#define WEB_APPROVE_REQUEST_BODY_OPTION_IMG_FMT \
				"<img src=\"%s\">"

/* Format for approve request html (common_tail)
 * no argument
 */
#define WEB_APPROVE_REQUEST_BODY_COMMON_TAIL_FMT \
			"</table>" \
			"<br>" \
			"<button onclick=\"a();\">Access</button>" \
			"<button onclick=\"window.open('about:blank','_self').close();\">Close</button>" \
		"</center>" \
	"</body>" \
"</html>"

#define WEB_APPROVE_REQUEST_TEXT_FMT \
	WEB_APPROVE_REQUEST_BODY_COMMON_HEAD_FMT \
	WEB_APPROVE_REQUEST_BODY_OPTION_TXT_FMT \
	WEB_APPROVE_REQUEST_BODY_COMMON_TAIL_FMT

#define WEB_APPROVE_REQUEST_IMG_FMT \
	WEB_APPROVE_REQUEST_BODY_COMMON_HEAD_FMT \
	WEB_APPROVE_REQUEST_BODY_OPTION_IMG_FMT \
	WEB_APPROVE_REQUEST_BODY_COMMON_TAIL_FMT


/* Format for meta refresh html (common_head)
 * argument list
 * 1.string: target host
 * 2.string: target uri
 * 3.string: target query
 * 4.string: target host
 */
#define WEB_META_REFRESH_BODY_COMMON_HEAD_FMT \
"<html>" \
	"<head></head>" \
	"<meta http-equiv=\"Refresh\" content=\"3; URL=%s%s%s\">" \
	"<body>" \
		"<center><br><br><b>\"%s\"</b><br><br>" \
			"<table>"

/* Format for meta refresh html (option_)
 * no argument
 */
#define WEB_META_REFRESH_BODY_OPTION_TXT_FMT \
				"Please wait...<br>" \
				"After reporting, access information, go to the site.<br><br>" \

/* Format for meta refresh html (option_img)
 * argument list
 * 1.string: image url
 */
#define WEB_META_REFRESH_BODY_OPTION_IMG_FMT \
				"<img src=\"%s\">"

/* Format for meta refresh html (common_tail)
 * no argument
 */
#define WEB_META_REFRESH_BODY_COMMON_TAIL_FMT \
			"</table>" \
		"</center>" \
	"</body>" \
"</html>"

#define WEB_META_REFRESH_TXT_FMT \
	WEB_META_REFRESH_BODY_COMMON_HEAD_FMT \
	WEB_META_REFRESH_BODY_OPTION_TXT_FMT \
	WEB_META_REFRESH_BODY_COMMON_TAIL_FMT

#define WEB_META_REFRESH_IMG_FMT \
	WEB_META_REFRESH_BODY_COMMON_HEAD_FMT \
	WEB_META_REFRESH_BODY_OPTION_IMG_FMT \
	WEB_META_REFRESH_BODY_COMMON_TAIL_FMT


/* Format for redirection html page (header + body)
 * argument list
 * 1.string: target url
 * 2.string: target url
 */
#define WEB_REDIRECTION_PAGE_FMT \
"HTTP/1.0 302 Moved temporarily\r\n" \
"Location: %s\r\n" \
"\r\n" \
"<html>" \
	"<body>" \
		"<a href=\"%s\"></a>" \
	"</body>" \
"</html>"

/* Format for redirection image html page
 * argument list
 * 1.string: image url
 */
#define WEB_REDIRECTION_BODY_FMT \
"<html>" \
	"<head></head>" \
	"<body>"\
		"<center><br><br>" \
			"<table>" \
				"<img src=\"%s\">" \
			"</table>" \
		"</center>" \
	"</body>" \
"</html>"


/* Format for redirection default html page
 * argument list
 * 1.string: url
 */
#define WEB_REDIRECTION_DEFAULT_HTML_FMT \
"<html>" \
"<head><title>WeGuardia ZEN Web Filter</title></head>\n" \
	"<body>" \
		"<center>" \
		"<br><br><b><h1>URL blocking!</h1><hr></b><br><br>" \
		"\"%s\"<br>" \
		"You are requested URL has been blocked." \
		"</center>" \
	"</body>" \
"</html>"


void send_tcp_reset(struct sk_buff *, int);
void send_tcp_fin_data(struct sk_buff *, int, char *, u_int16_t);

#endif/*_PROFILE_WEB_ACTION_COMMON_H*/

