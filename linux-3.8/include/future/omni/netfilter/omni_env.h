#ifndef _OMNI_ENV_H
#define _OMNI_ENV_H

#include <future/omni/filter/omni_x_tables.h>

struct omni_ipt_env_entry {
	unsigned char type;
	unsigned int idx;
};

struct omni_env {
	unsigned int nr_entries;
	struct omni_ipt_env_entry entry[1];
};

struct omni_env_match_info {
	struct omni_xt_entry_match m;
	struct omni_env env;
};
#endif
