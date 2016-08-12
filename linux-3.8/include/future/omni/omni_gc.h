#ifndef _OMNI_GC_H
#define _OMNI_GC_H

#include <linux/llist.h>
#include <future/omni/omni_track_tuple.h>

//#define ADV_GC

extern int omni_gc_add(struct omni_track_tuple_hash *tuplehash);

struct omni_gc_entry {
	struct llist_node list;
	struct omni_track_tuple_hash *tuplehash;
#ifdef ADV_GC
	struct omni_track_tuple_hash *prev[2];
#endif
	struct omni_mbuf *m;
};

struct omni_gc_stat {
	unsigned long start;
	unsigned long end;
	unsigned long avg;
	unsigned long samples;
};

DECLARE_PER_CPU(struct omni_gc_stat, gc_stat);

#endif
