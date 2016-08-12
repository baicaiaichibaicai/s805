#ifndef _OMNI_TRACK_ACCT
#define _OMNI_TRACK_ACCT

struct omni_track_counter {
	atomic64_t packets;
	atomic64_t bytes;
};

#endif
