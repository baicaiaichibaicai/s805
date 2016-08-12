#ifndef _OMNI_IP_H
#define _OMNI_IP_H

#include <future/omni/filter/omni_x_tables.h>

struct omni_ipt_ip_entry {
	unsigned char type;
	unsigned int idx;
};

struct omni_ip {
	unsigned int nr_entries;
	struct omni_ipt_ip_entry entry[1];
};

struct omni_ip_match_info {
	struct omni_xt_entry_match m;
	struct omni_ip ip;
};

enum {
	O_SRC_TYPE = 0,
	O_DST_TYPE, 
	O_SRC_GROUP_TYPE,
	O_DST_GROUP_TYPE
};

#endif
