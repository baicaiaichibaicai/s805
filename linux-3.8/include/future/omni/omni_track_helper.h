/*
 * connection tracking helpers.
 *
 * 16 Dec 2003: Yasuyuki Kozakai @USAGI <yasuyuki.kozakai@toshiba.co.jp>
 *	- generalize L3 protocol dependent part.
 *
 * Derived from include/linux/netfiter_ipv4/ip_conntrack_helper.h
 */

#ifndef _OMNI_TRACK_HELPER_H
#define _OMNI_TRACK_HELPER_H
#include <future/omni_track.h>
#include <future/omni/omni_track_extend.h>
#include <future/omni/nat/omni_nat.h>

#ifdef __KERNEL__
#include <linux/bitops.h>
#include <linux/compiler.h>
#include <asm/atomic.h>

#define OMNI_F_BASIC	0
#define OMNI_F_HELP		1	
#define OMNI_F_NAT		2
#define OMNI_F_NUM		4

extern unsigned int check_help_assign;

struct module;

extern struct omni_track_tuple *test_tuple;

extern struct list_head tr_helpers;

struct omni_track_helper
{	
	struct list_head list; 	

	const char *name;			
	struct module *me;		
	unsigned int max_expected;	
	unsigned int timeout;		

	/* Mask of things we will help (compared against server response) */
	struct omni_track_tuple tuple;
	struct omni_track_tuple mask;
	
	/* Function to call when data passes; return verdict, or -1 to
           invalidate. */
	int (*helpfn)(struct sk_buff **pskb,
			    unsigned int protoff,
			    struct omni_track *ct,
			    enum omni_track_info track_info);

	void (*destroy)(struct omni_track *ct);

	int (*to_nfattr)(struct sk_buff *skb, const struct omni_track *ct);
};

extern struct omni_track_helper *
__omni_track_helper_find(const struct omni_track_tuple *tuple);

extern struct omni_track_helper *
omni_track_helper_find_get( const struct omni_track_tuple *tuple);

extern struct omni_track_helper *
__omni_track_helper_find_byname(const char *name);

extern void omni_track_helper_put(struct omni_track_helper *helper);
extern int omni_track_helper_register(struct omni_track_helper *);

unsigned int ipv4_omni_track_helper(struct sk_buff *pskb);

static inline struct omni_tr_help *omnitr_help(struct omni_track *tr)
{
	struct omni_tr_help *omnitrack_help = &(tr->help);

	return omnitrack_help;
}

/* expect function */
struct omni_track_helper_expectfn {
	struct list_head head;
	const char *name;
	void (*expectfn)(struct omni_track *tr, struct omni_track_expect *exp);
};

void omni_track_helper_expectfn_register(struct omni_track_helper_expectfn *n);

void omni_track_helper_expectfn_unregister(struct omni_track_helper_expectfn *n);

extern int omni_track_helper_init(struct net *net);
extern void omni_track_helper_fini(struct net *net);

extern void omni_track_helper_unregister(struct omni_track_helper *);

#endif /* __KERNEL__ */
#endif /*_OMNI_TRACK_HELPER_H*/
