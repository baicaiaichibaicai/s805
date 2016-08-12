/*
 * connection tracking expectations.
 */

#ifndef _OMNI_TRACK_EXPECT_H
#define _OMNI_TRACK_EXPECT_H
#include <future/omni_track.h>

extern struct list_head omni_track_expect_list;
extern struct kmem_cache *omni_track_expect_cachep;
extern struct file_operations exp_file_ops;
extern atomic_t omni_exp_count;

struct omni_track_expect
{
	/* Internal linked list (global expectation list) */
	struct list_head list;

	/* We expect this tuple, with the following mask */
	struct omni_track_tuple tuple, mask;

	/* Function to call after setup and insertion */
	void (*expectfn)(struct omni_track *new,
			 struct omni_track_expect *this);

	/* Helper to assign to new connection */
	struct omni_track_helper *helper;

	/* The conntrack of the master connection */
	struct omni_track *master;

	/* Timer function; deletes the expectation. */
	struct timer_list timeout;

	/* Usage count. */
	atomic_t use;

	/* Unique ID */
	unsigned int id;

	/* Flags */
	unsigned int flags;

	__be32 saved_ip;
	/* This is the original per-proto part, used to map the
	 * expected connection the way the recipient expects. */
	union omni_track_man_proto saved_proto;
	/* Direction relative to the master connection. */
	enum omni_track_dir dir;

	u32	type;
};

#define OMNI_TRACK_EXPECT_PERMANENT 0x1

struct omni_track_expect *
__omni_track_expect_find(const struct omni_track_tuple *tuple);

struct omni_track_expect *
omni_track_expect_find_get(const struct omni_track_tuple *tuple);

struct omni_track_expect *
omni_find_expectation(const struct omni_track_tuple *tuple);

void omni_track_unlink_expect(struct omni_track_expect *exp);
void omni_tr_remove_expectations(struct omni_track *ct);
void omni_track_unexpect_related(struct omni_track_expect *exp);

/* Allocate space for an expectation: this is mandatory before calling
   omni_track_expect_related.  You will have to call put afterwards. */
struct omni_track_expect *omni_track_expect_alloc(struct omni_track *me);
void omni_track_expect_init(struct omni_track_expect *, int,
			      			union omni_track_address *,
			      			union omni_track_address *,
			      			u_int8_t, __be16 *, __be16 *);
void omni_track_expect_put(struct omni_track_expect *exp);
int omni_track_expect_related(struct omni_track_expect *expect);

void init_expect_func_list(void);
void (*get_expect_func(__u16 idx))(struct omni_track *new, struct omni_track_expect *this);

enum EXPECT_FN_LIST {
    EXPECT_FN_NULL,
    EXPECT_FN_FTP,
    EXPECT_FN_TFTP,
    EXPECT_FN_MAX
};

#endif /*_OMNI_TRACK_EXPECT_H*/

