/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_UTILS_H
#define _PROFILE_WEB_UTILS_H


#include <future/fw/profiles/profile_web.h>
#include <linux/skbuff.h>

enum http_method get_http_method(const char *, const int);
int get_http_host(char *, const char *, const int);
void get_http_uri_and_query(char *, int *, char *, int *, const char *, const int);
char *get_http_url(const char *, const char *, const char *);
enum http_content_type get_http_content_type(const char *, const int);

int remove_accept_encoding(const char *, const int);
void adjust_tcp_csum(const struct sk_buff *);

#endif/*_PROFILE_WEB_UTILS_H*/

