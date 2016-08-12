#ifndef _OMNI_USER_H
#define _OMNI_USER_H

#include <future/omni/filter/omni_x_tables.h>

struct omni_ipt_user_entry {
	unsigned char type;
	unsigned int idx;
};

struct omni_user {
	unsigned int nr_entries;
	struct omni_ipt_user_entry entry[1];
};

struct omni_user_match_info {
	struct omni_xt_entry_match m;
	struct omni_user user;
};
#endif
