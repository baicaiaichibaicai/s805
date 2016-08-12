#ifndef _OMNI_SERVICE_H
#define _OMNI_SERVICE_H

#include <future/omni/filter/omni_x_tables.h>

struct omni_ipt_service_entry {
	unsigned char type;
	unsigned int idx;
};

struct omni_service {
	unsigned int nr_entries;
	struct omni_ipt_service_entry entry[1];
};

struct omni_service_match_info {
	struct omni_xt_entry_match m;
	struct omni_service serv;
};

enum {
	O_SERVICE_TYPE = 0,
	O_GROUP_TYPE
};


#endif
