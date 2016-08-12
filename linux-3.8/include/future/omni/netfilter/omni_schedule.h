#ifndef _OMNI_SCHEDULE_H
#define _OMNI_SCHEDULE_H

#include <future/omni/filter/omni_x_tables.h>

struct omni_ipt_schedule_entry {
	unsigned char type;
	unsigned int idx;
};

struct omni_schedule {
	unsigned int nr_entries;
	struct omni_ipt_schedule_entry entry[1];
};

struct omni_schedule_match_info {
	struct omni_xt_entry_match m;
	struct omni_schedule schedule;
};

enum {
	O_SCHEDULE_TYPE = 0,
};

#endif
