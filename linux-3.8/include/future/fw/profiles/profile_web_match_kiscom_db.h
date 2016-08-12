/*
 * Future Systems
 */
#ifndef _PROFILE_WEB_MATCH_KOCSC_DB_H
#define _PROFILE_WEB_MATCH_KOCSC_DB_H

#include <future/command/vty.h>


int run_kiscom_db_search(unsigned char *, unsigned char *);
void manual_search_kiscom_domain_tree(struct vty *vty, char *p_url, int url_len);

int load_kiscom_db(void);
void unloading_kiscom_db(void);

int kiscom_db_proc_init(void);
void kiscom_db_proc_exit(void);

#endif/*_PROFILE_WEB_MATCH_KOCSC_DB_H*/

