#ifndef _OMNI_TRACK_ICMP_H
#define _OMNI_TRACK_ICMP_H

#include <asm/atomic.h>

struct omni_track_icmp {
	/* Optimization: when number in == number out, forget immediately. */
	atomic_t count;
};

#endif
