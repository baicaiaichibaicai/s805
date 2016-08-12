#ifndef __OMNI_TRACK_SYNC_PRINT_H__
#define __OMNI_TRACK_SYNC_PRINT_H__

#include <linux/types.h>

struct sync_track {
	__be32 ip;
	__be16 port;
	u_int16_t proto;
};

int omni_track_sync_debug(void *data, char *action);

#endif
