#ifndef _OMNI_CC_H
#define _OMNI_CC_H

#include <future/omni/filter/omni_x_tables.h>

struct omni_ipt_cc_entry {
	unsigned char type;
	unsigned int idx;
};

struct omni_cc {
	unsigned int nr_entries;
	struct omni_ipt_cc_entry entry[1];
};

struct omni_cc_match_info {
	struct omni_xt_entry_match m;
	struct omni_cc cc;
};
#endif
