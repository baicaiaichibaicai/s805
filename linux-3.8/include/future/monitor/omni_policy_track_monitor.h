#ifndef _OMNI_MONITOR_POLICY_TRACK
#define _OMNI_MONITOR_POLICY_TRACK

#include <future/omni/filter/omni_filter.h>
#include <future/omni/omni_track_tuple_common.h>
#include <future/omni/netfilter/omni_x_tables.h>

#define OM_MON_PT_SET_ID 5791
#define OM_MON_PT_GET_ID 5796

#define OM_PT_CMD_ENTRIES	0x01
#define OM_PT_CMD_LIST		0x02

struct omni_mon_pt_info{
	struct omni_filter filter;
	struct omni_xt_counters total_acct[OMNI_TRACK_DIR_MAX];

	unsigned long total_session;

	unsigned long bps[OMNI_TRACK_DIR_MAX];
	unsigned long pps[OMNI_TRACK_DIR_MAX];
	unsigned long cps;

	unsigned int verdict;
};

struct omni_mon_pt_sock {
	struct omni_mon_pt_info *omni_pt_info;

	unsigned int number;
	unsigned int size;

	__u8 cmd_type;
};

#endif
