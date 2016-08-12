#ifndef _X_OMNI_TABLES_H
#define _X_OMNI_TABLES_H


#include <linux/netdevice.h>
#include <future/omni/filter/omni_x_tables.h>
#include <future/omni/object/object_set.h>

/**
 * struct xt_action_param - parameters for matches/targets
 *
 * @match:	the match extension
 * @target:	the target extension
 * @matchinfo:	per-match data
 * @targetinfo:	per-target data
 * @in:		input netdevice
 * @out:	output netdevice
 * @fragoff:	packet is a fragment, this is the data offset
 * @thoff:	position of transport header relative to skb->data
 * @hook:	hook number given packet came from
 * @family:	Actual NFPROTO_* through which the function is invoked
 * 		(helpful when match->family == NFPROTO_UNSPEC)
 *
 * Fields written to by extensions:
 *
 * @hotdrop:	drop packet if we had inspection problems
 * Network namespace obtainable using dev_net(in/out)
 */
struct omni_xt_action_param {
	union {
		const struct omni_xt_match *match;
		const struct omni_xt_target *target;
	};
	union {
		const void *matchinfo, *targinfo;
	};
	const struct net_device *in, *out;
	int fragoff;
	unsigned int thoff;
	unsigned int hooknum;
	u_int8_t family;
	bool hotdrop;

	unsigned int matched_obj[OM_TABLE_ALL];
	unsigned int timeout;
    struct sk_buff *skb;
};

/**
 * struct omni_xt_mtchk_param - parameters for match extensions'
 * checkentry functions
 *
 * @net:	network namespace through which the check was invoked
 * @table:	table the rule is tried to be inserted into
 * @entryinfo:	the family-specific rule data
 * 		(struct ipt_ip, ip6t_ip, arpt_arp or (note) ebt_entry)
 * @match:	struct omni_xt_match through which this function was invoked
 * @matchinfo:	per-match data
 * @hook_mask:	via which hooks the new rule is reachable
 * Other fields as above.
 */
struct omni_xt_mtchk_param {
	struct net *net;
	const char *table;
	const void *entryinfo;
	const struct omni_xt_match *match;
	void *matchinfo;
	unsigned int hook_mask;
	u_int8_t family;
};

/**
 * struct xt_mdtor_param - match destructor parameters
 * Fields as above.
 */
struct omni_xt_mtdtor_param {
	struct net *net;
	const struct omni_xt_match *match;
	void *matchinfo;
	u_int8_t family;
};

/**
 * struct omni_xt_tgchk_param - parameters for target extensions'
 * checkentry functions
 *
 * @entryinfo:	the family-specific rule data
 * 		(struct omni_ipt_entry, omni_ip6t_entry, arpt_entry, ebt_entry)
 *
 * Other fields see above.
 */
struct omni_xt_tgchk_param {
	struct net *net;
	const char *table;
	const void *entryinfo;
	const struct omni_xt_target *target;
	void *targinfo;
	unsigned int hook_mask;
	u_int8_t family;
};

/* Target destructor parameters */
struct omni_xt_tgdtor_param {
	struct net *net;
	const struct omni_xt_target *target;
	void *targinfo;
	u_int8_t family;
};

struct omni_xt_match {
	struct list_head list;

	const char name[XT_EXTENSION_MAXNAMELEN];
	u_int8_t revision;

	/* Return true or false: return FALSE and set *hotdrop = 1 to
           force immediate packet drop. */
	/* Arguments changed since 2.6.9, as this must now handle
	   non-linear skb, using skb_header_pointer and
	   skb_ip_make_writable. */
	bool (*match)(const struct sk_buff *skb,
		      struct omni_xt_action_param *);

	/* Called when user tries to insert an entry of this type. */
	int (*checkentry)(const struct omni_xt_mtchk_param *);

	/* Called when entry of this type deleted. */
	void (*destroy)(const struct omni_xt_mtdtor_param *);
#ifdef CONFIG_COMPAT
	/* Called when userspace align differs from kernel space one */
	void (*compat_from_user)(void *dst, const void *src);
	int (*compat_to_user)(void __user *dst, const void *src);
#endif
	/* Set this to THIS_MODULE if you are a module, otherwise NULL */
	struct module *me;

	const char *table;
	unsigned int matchsize;
#ifdef CONFIG_COMPAT
	unsigned int compatsize;
#endif
	unsigned int hooks;
	unsigned short proto;

	unsigned short family;
};

/* Registration hooks for targets. */
struct omni_xt_target {
	struct list_head list;

	const char name[XT_EXTENSION_MAXNAMELEN];
	u_int8_t revision;

	/* Returns verdict. Argument order changed since 2.6.9, as this
	   must now handle non-linear skbs, using skb_copy_bits and
	   skb_ip_make_writable. */
	unsigned int (*target)(struct sk_buff *skb,
			       const struct omni_xt_action_param *);

	/* Called when user tries to insert an entry of this type:
           hook_mask is a bitmask of hooks from which it can be
           called. */
	/* Should return 0 on success or an error code otherwise (-Exxxx). */
	int (*checkentry)(const struct omni_xt_tgchk_param *);

	/* Called when entry of this type deleted. */
	void (*destroy)(const struct omni_xt_tgdtor_param *);
#ifdef CONFIG_COMPAT
	/* Called when userspace align differs from kernel space one */
	void (*compat_from_user)(void *dst, const void *src);
	int (*compat_to_user)(void __user *dst, const void *src);
#endif
	/* Set this to THIS_MODULE if you are a module, otherwise NULL */
	struct module *me;

	const char *table;
	unsigned int targetsize;
#ifdef CONFIG_COMPAT
	unsigned int compatsize;
#endif
	unsigned int hooks;
	unsigned short proto;

	unsigned short family;
};

/* Furniture shopping... */
struct omni_xt_table {
	struct list_head list;

	/* What hooks you will enter on */
	unsigned int valid_hooks;

	/* Man behind the curtain... */
	struct omni_xt_table_info *private;

	/* Set this to THIS_MODULE if you are a module, otherwise NULL */
	struct module *me;

	u_int8_t af;		/* address/protocol family */
	int priority;		/* hook order */

	/* A unique name... */
	const char name[XT_TABLE_MAXNAMELEN];

	/* efficient identifier */
	u_int8_t type;
};

#include <linux/netfilter_ipv4.h>

/* The table itself */
struct omni_xt_table_info {
	/* Size per table */
	unsigned int size;
	/* Number of entries: FIXME. --RR */
	unsigned int number;
	/* Initial number of entries. Needed for module usage count */
	unsigned int initial_entries;

	/* Entry points and underflows */
	unsigned int hook_entry[NF_INET_NUMHOOKS];
	unsigned int underflow[NF_INET_NUMHOOKS];

	/* Policy map --sekim */
	struct omni_policy_map_info *mapinfo;
	struct omni6_policy_map_info *mapinfo6;

	/* account map --dhkim */
	struct omni_acct *acct;

	__u8 memkeep;
	/*
	 * Number of user chains. Since tables cannot have loops, at most
	 * @stacksize jumps (number of user chains) can possibly be made.
	 */
	unsigned int stacksize;
	unsigned int __percpu *stackptr;
	void ***jumpstack;
	/* ipt_entry tables: one per CPU */
	/* Note : this field MUST be the last one, see XT_TABLE_INFO_SZ */
	void *entries[1];
};

#define OMNI_XT_TABLE_INFO_SZ (offsetof(struct omni_xt_table_info, entries) \
			  + nr_cpu_ids * sizeof(char *))

extern int omni_xt_jumpstack_alloc(struct omni_xt_table_info *i);
extern int omni_xt_register_target(struct omni_xt_target *target);
extern void omni_xt_unregister_target(struct omni_xt_target *target);
extern int omni_xt_register_targets(struct omni_xt_target *target, unsigned int n);
extern void omni_xt_unregister_targets(struct omni_xt_target *target, unsigned int n);

extern int omni_xt_register_match(struct omni_xt_match *target);
extern void omni_xt_unregister_match(struct omni_xt_match *target);
extern int omni_xt_register_matches(struct omni_xt_match *match, unsigned int n);
extern void omni_xt_unregister_matches(struct omni_xt_match *match, unsigned int n);

extern int omni_xt_check_match(struct omni_xt_mtchk_param *,
			  unsigned int size, u_int8_t proto, bool inv_proto);
extern int omni_xt_check_target(struct omni_xt_tgchk_param *,
			   unsigned int size, u_int8_t proto, bool inv_proto);

extern struct omni_xt_table *omni_xt_register_table(const struct omni_xt_table *table,
					  struct omni_xt_table_info *bootstrap,
					  struct omni_xt_table_info *newinfo);
extern void *omni_xt_unregister_table(struct omni_xt_table *table);

extern struct omni_xt_table_info *omni_xt_replace_table(struct omni_xt_table *table,
					      unsigned int num_counters,
					      struct omni_xt_table_info *newinfo,
					      int *error);

extern struct omni_xt_match *omni_xt_find_match(u8 af, const char *name, u8 revision);
extern struct omni_xt_target *omni_xt_find_target(u8 af, const char *name, u8 revision);
extern struct omni_xt_match *omni_xt_request_find_match(u8 af, const char *name,
					      u8 revision);
extern struct omni_xt_target *omni_xt_request_find_target(u8 af, const char *name,
						u8 revision);
extern int omni_xt_find_revision(u8 af, const char *name, u8 revision,
			    int target, int *err);

extern struct omni_xt_table *omni_xt_find_table_lock(u_int8_t af, const char *name);
extern void omni_xt_table_unlock(struct omni_xt_table *t);

extern int omni_xt_proto_init(u_int8_t af);
extern void omni_xt_proto_fini(u_int8_t af);

extern struct omni_xt_table_info *omni_xt_alloc_table_info(unsigned int size);
extern void omni_xt_free_table_info(struct omni_xt_table_info *info);

extern struct omni_xt_table_info *omni_xt_table_base_get(int idx);
extern struct omni_xt_table_info *omni6_xt_table_base_get(int idx);
extern void omni_policy_free_v4_table(void);
extern void omni_policy_free_v6_table(void);

/**
 * omni_xt_recseq - recursive seqcount for netfilter use
 * 
 * Packet processing changes the seqcount only if no recursion happened
 * get_counters() can use read_seqcount_begin()/read_seqcount_retry(),
 * because we use the normal seqcount convention :
 * Low order bit set to 1 if a writer is active.
 */
DECLARE_PER_CPU(seqcount_t, omni_xt_recseq);

/**
 * omni_xt_write_recseq_begin - start of a write section
 *
 * Begin packet processing : all readers must wait the end
 * 1) Must be called with preemption disabled
 * 2) softirqs must be disabled too (or we should use this_cpu_add())
 * Returns :
 *  1 if no recursion on this cpu
 *  0 if recursion detected
 */
static inline unsigned int omni_xt_write_recseq_begin(void)
{
	unsigned int addend;

	/*
	 * Low order bit of sequence is set if we already
	 * called omni_xt_write_recseq_begin().
	 */
	addend = (__this_cpu_read(omni_xt_recseq.sequence) + 1) & 1;

	/*
	 * This is kind of a write_seqcount_begin(), but addend is 0 or 1
	 * We dont check addend value to avoid a test and conditional jump,
	 * since addend is most likely 1
	 */
	__this_cpu_add(omni_xt_recseq.sequence, addend);
	smp_wmb();

	return addend;
}

/**
 * omni_xt_write_recseq_end - end of a write section
 * @addend: return value from previous omni_xt_write_recseq_begin()
 *
 * End packet processing : all readers can proceed
 * 1) Must be called with preemption disabled
 * 2) softirqs must be disabled too (or we should use this_cpu_add())
 */
static inline void omni_xt_write_recseq_end(unsigned int addend)
{
	/* this is kind of a write_seqcount_end(), but addend is 0 or 1 */
	smp_wmb();
	__this_cpu_add(omni_xt_recseq.sequence, addend);
}

/*
 * This helper is performance critical and must be inlined
 */
static inline unsigned long omni_ifname_compare_aligned(const char *_a,
						   const char *_b,
						   const char *_mask)
{
	const unsigned long *a = (const unsigned long *)_a;
	const unsigned long *b = (const unsigned long *)_b;
	const unsigned long *mask = (const unsigned long *)_mask;
	unsigned long ret;

	ret = (a[0] ^ b[0]) & mask[0];
	if (IFNAMSIZ > sizeof(unsigned long))
		ret |= (a[1] ^ b[1]) & mask[1];
	if (IFNAMSIZ > 2 * sizeof(unsigned long))
		ret |= (a[2] ^ b[2]) & mask[2];
	if (IFNAMSIZ > 3 * sizeof(unsigned long))
		ret |= (a[3] ^ b[3]) & mask[3];
	BUILD_BUG_ON(IFNAMSIZ > 4 * sizeof(unsigned long));
	return ret;
}

extern struct nf_hook_ops *omni_xt_hook_link(const struct omni_xt_table *, nf_hookfn *);
extern void omni_xt_hook_unlink(const struct omni_xt_table *, struct nf_hook_ops *);

struct check_use_modules {
	unsigned int filter;
	unsigned int mangle;
	unsigned int vpn;
	unsigned int nat;
	unsigned int log;
	unsigned int profile;
	unsigned int limit;
};

#ifdef CONFIG_COMPAT
#include <net/compat.h>

struct omni_compat_xt_entry_match {
	union {
		struct {
			u_int16_t match_size;
			char name[XT_FUNCTION_MAXNAMELEN - 1];
			u_int8_t revision;
		} user;
		struct {
			u_int16_t match_size;
			compat_uptr_t match;
		} kernel;
		u_int16_t match_size;
	} u;
	unsigned char data[0];
};

struct omni_compat_xt_entry_target {
	union {
		struct {
			u_int16_t target_size;
			char name[XT_FUNCTION_MAXNAMELEN - 1];
			u_int8_t revision;
		} user;
		struct {
			u_int16_t target_size;
			compat_uptr_t target;
		} kernel;
		u_int16_t target_size;
	} u;
	unsigned char data[0];
};

/* FIXME: this works only on 32 bit tasks
 * need to change whole approach in order to calculate align as function of
 * current task alignment */

struct omni_compat_xt_counters {
	compat_u64 pcnt, bcnt;			/* Packet and byte counters */
};

struct omni_compat_xt_counters_info {
	char name[XT_TABLE_MAXNAMELEN];
	compat_uint_t num_counters;
	struct omni_compat_xt_counters counters[0];
};

struct _omni_compat_xt_align {
	__u8 u8;
	__u16 u16;
	__u32 u32;
	compat_u64 u64;
};

#define OMNI_COMPAT_XT_ALIGN(s) __ALIGN_KERNEL((s), __alignof__(struct _omni_compat_xt_align))

extern void omni_xt_compat_lock(u_int8_t af);
extern void omni_xt_compat_unlock(u_int8_t af);

extern int omni_xt_compat_add_offset(u_int8_t af, unsigned int offset, int delta);
extern void omni_xt_compat_flush_offsets(u_int8_t af);
extern void omni_xt_compat_init_offsets(u_int8_t af, unsigned int number);
extern int omni_xt_compat_calc_jump(u_int8_t af, unsigned int offset);

extern int omni_xt_compat_match_offset(const struct omni_xt_match *match);
extern int omni_xt_compat_match_from_user(struct omni_xt_entry_match *m,
				     void **dstptr, unsigned int *size);
extern int omni_xt_compat_match_to_user(const struct omni_xt_entry_match *m,
				   void __user **dstptr, unsigned int *size);

extern int omni_xt_compat_target_offset(const struct omni_xt_target *target);
extern void omni_xt_compat_target_from_user(struct omni_xt_entry_target *t,
				       void **dstptr, unsigned int *size);
extern int omni_xt_compat_target_to_user(const struct omni_xt_entry_target *t,
				    void __user **dstptr, unsigned int *size);


#endif /*CONFIG_COMPAT*/
#endif /*_X_OMNI_TABLES_H*/
