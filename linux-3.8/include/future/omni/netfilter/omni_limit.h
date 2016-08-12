#ifndef _OMNI_LIMIT_H
#define _OMNI_LIMIT_H

#include <future/omni/filter/omni_x_tables.h>

struct omni_ipt_limit_entry {
	unsigned char type;
	unsigned int idx;
};

struct omni_limit {
	unsigned int nr_entries;
	struct omni_ipt_limit_entry entry[1];
};

struct omni_limit_match_info {
	struct omni_xt_entry_match m;
	struct omni_limit limit;
};

enum {
	O_LIMIT_TYPE = 0,
};

#endif
