/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_MATCH_URL_DB_H
#define _PROFILE_WEB_MATCH_URL_DB_H


#include <linux/rbtree.h>
#include <future/fw/profiles/profile_web_match.h>
#include <future/command/vty.h>


typedef struct _url_list {
    struct _url_list 	*p_next;
    char 		*url_str;
    unsigned char	category[HTTP_URL_DB_CATEGORY_SIZE];
} url_list;

typedef struct _represent_domain {
    struct rb_node	node;
    unsigned char 	hash[URL_HASH_SIZE];
    url_list 		*p_url;
} represent_domain;


void add_category(int, unsigned char *);

int search_general_domain_tree(char *, int, unsigned char *);
void manual_search_general_domain_tree(struct vty *vty, char *p_url, int url_len);

int load_http_url_db(void);
void unload_http_url_db(void);

int url_db_proc_init(void);
void url_db_proc_exit(void);

#endif/*_PROFILE_WEB_MATCH_URL_DB_H*/

