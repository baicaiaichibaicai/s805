#ifndef _OMNI_DOMAIN_H
#define _OMNI_DOMAIN_H

#include <future/omni/filter/omni_x_tables.h>

struct omni_ipt_domain_entry {
	unsigned char type;
	unsigned int idx;
};

struct omni_domain {
	unsigned int nr_entries;
	struct omni_ipt_domain_entry entry[1];
};

struct omni_domain_match_info {
	struct omni_xt_entry_match m;
	struct omni_domain domain;
};
#endif
