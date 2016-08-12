/*
 * connection tracking filter expectations.
 */

#ifndef _OMNI_TRACK_FILTER_EXPECT_H
#define _OMNI_TRACK_FILTER_EXPECT_H
#include <future/omni_track.h>

#define EXP_TYPE_NONE	0x00
#define EXP_TYPE_RTP	0x01

extern struct list_head omni_filter_expect_list;
extern struct kmem_cache *omni_filter_expect_cachep;

struct omni_filter_expect
{
	struct list_head list;
	struct omni_track_tuple tuple, mask;

	struct omni_track *master;
	struct timer_list timeout;

	atomic_t use;
	unsigned int id;
	unsigned int type;
};

struct omni_filter_expect *__omni_filter_expect_find(const struct omni_track_tuple *tuple);
struct omni_filter_expect *omni_filter_expect_find_get(const struct omni_track_tuple *tuple);
struct omni_filter_expect *find_filter_exp(const struct omni_track_tuple *tuple);

void omni_filter_unlink_expect(struct omni_filter_expect *exp);
void omni_remove_filter_exps(struct omni_track *track);
void omni_filter_unexpect_related(struct omni_filter_expect *exp);

struct omni_filter_expect *omni_filter_expect_alloc(struct omni_track *me);
void omni_filter_expect_init(struct omni_filter_expect *, int,
			      union omni_track_address *,
			      union omni_track_address *,
			      u_int8_t, __be16 *, __be16 *);
void omni_filter_expect_put(struct omni_filter_expect *exp);
int omni_filter_expect_related(struct omni_filter_expect *expect);

#endif /*_OMNI_TRACK_FILTER_EXPECT_H*/

