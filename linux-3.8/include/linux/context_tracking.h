#ifndef _LINUX_CONTEXT_TRACKING_H
#define _LINUX_CONTEXT_TRACKING_H

#ifdef CONFIG_OCTEON_FUTURE_BOARD
#include <linux/percpu.h>
#include <linux/vtime.h>
#include <asm/ptrace.h>
#include <linux/sched.h>

struct context_tracking {
	/*
	 * When active is false, probes are unset in order
	 * to minimize overhead: TIF flags are cleared
	 * and calls to user_enter/exit are ignored. This
	 * may be further optimized using static keys.
	 */
	bool active;
	enum ctx_state {
		IN_KERNEL = 0,
		IN_USER,
	} state;
};

static inline void __guest_enter(void)
{
	/*
	 * This is running in ioctl context so we can avoid
	 * the call to vtime_account() with its unnecessary idle check.
	 */
	vtime_account_system(current);
	current->flags |= PF_VCPU;
}

static inline void __guest_exit(void)
{
	/*
	 * This is running in ioctl context so we can avoid
	 * the call to vtime_account() with its unnecessary idle check.
	 */
	vtime_account_system(current);
	current->flags &= ~PF_VCPU;
}
#endif

#ifdef CONFIG_CONTEXT_TRACKING
#include <linux/sched.h>

extern void user_enter(void);
extern void user_exit(void);
extern void context_tracking_task_switch(struct task_struct *prev,
					 struct task_struct *next);
#else
static inline void user_enter(void) { }
static inline void user_exit(void) { }
#ifdef CONFIG_OCTEON_FUTURE_BOARD
static inline void guest_enter(void)
{
	__guest_enter();
}

static inline void guest_exit(void)
{
	__guest_exit();
}

static inline enum ctx_state exception_enter(void) { return 0; }
static inline void exception_exit(enum ctx_state prev_ctx) { }
#endif
static inline void context_tracking_task_switch(struct task_struct *prev,
						struct task_struct *next) { }
#endif /* !CONFIG_CONTEXT_TRACKING */

#endif
