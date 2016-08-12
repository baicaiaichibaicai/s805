#ifndef _OMNI_TRACK_TUPLE_COMMON_H
#define _OMNI_TRACK_TUPLE_COMMON_H

enum omni_track_dir
{
	OMNI_TRACK_DIR_ORIGINAL,
	OMNI_TRACK_DIR_REPLY,
	OMNI_TRACK_DIR_MAX
};

struct omni_track_stat
{
	unsigned int searched;
	unsigned int found;
	unsigned int new;
	unsigned int invalid;
	unsigned int ignore;
	unsigned int delete;
	unsigned int delete_list;
	unsigned int insert;
	unsigned int insert_failed;
	unsigned int drop;
	unsigned int early_drop;
	unsigned int error;
	unsigned int expect_new;
	unsigned int expect_create;
	unsigned int expect_delete;

	unsigned int gc_searched;
	unsigned int gc_delete;
	unsigned int gc_retry;
};

#define TRINFO2DIR(trinfo) ((trinfo) >= OMNI_TRACK_IS_REPLY ? OMNI_TRACK_DIR_REPLY : OMNI_TRACK_DIR_ORIGINAL)

#endif
