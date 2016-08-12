/*-
 *   BSD LICENSE
 *
 *   Copyright(c) 2010-2014 Intel Corporation. All rights reserved.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _OMNI_TIMER_H_
#define _OMNI_TIMER_H_

/**
 * @file
 RTE Timer
 *
 * This library provides a timer service to RTE Data Plane execution
 * units that allows the execution of callback functions asynchronously.
 *
 * - Timers can be periodic or single (one-shot).
 * - The timers can be loaded from one core and executed on another. This has
 *   to be specified in the call to omni_timer_reset().
 * - High precision is possible. NOTE: this depends on the call frequency to
 *   omni_timer_manage() that check the timer expiration for the local core.
 * - If not used in an application, for improved performance, it can be
 *   disabled at compilation time by not calling the omni_timer_manage()
 *   to improve performance.
 *
 * The timer library uses the omni_get_hpet_cycles() function that
 * uses the HPET, when available, to provide a reliable time reference. [HPET
 * routines are provided by EAL, which falls back to using the chip TSC (time-
 * stamp counter) as fallback when HPET is not available]
 *
 * This library provides an interface to add, delete and restart a
 * timer. The API is based on the BSD callout(9) API with a few
 * differences.
 *
 * See the RTE architecture documentation for more information about the
 * design of this library.
 */
#if 0
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#endif
//#include <linux/random.h>

#define OMNI_TIMER_STOP    0 /**< State: timer is stopped. */
#define OMNI_TIMER_PENDING 1 /**< State: timer is scheduled. */
#define OMNI_TIMER_RUNNING 2 /**< State: timer function is running. */
#define OMNI_TIMER_CONFIG  3 /**< State: timer is being configured. */
#define OMNI_TIMER_EXPIRED 4 /**< State: timer is expired. */

#define OMNI_TIMER_NO_OWNER -2 /**< Timer has no owner. */
#define LCORE_ID_ANY     UINT_MAX       /**< Any lcore. */

/**
 * Timer type: Periodic or single (one-shot).
 */
enum omni_timer_type {
	SINGLE,
	PERIODICAL
};

/**
 * Timer status: A union of the state (stopped, pending, running,
 * config) and an owner (the id of the lcore that owns the timer).
 */
union omni_timer_status {
	struct {
		uint16_t state;  /**< Stop, pending, running, config. */
		int16_t owner;   /**< The lcore that owns the timer. */
	};
	uint32_t u32;            /**< To atomic-set status + owner. */
};

#ifdef OMNI_LIBOMNI_TIMER_DEBUG
/**
 * A structure that stores the timer statistics (per-lcore).
 */
struct omni_timer_debug_stats {
	uint64_t reset;   /**< Number of success calls to omni_timer_reset(). */
	uint64_t stop;    /**< Number of success calls to omni_timer_stop(). */
	uint64_t manage;  /**< Number of calls to omni_timer_manage(). */
	uint64_t pending; /**< Number of pending/running timers. */
};
#endif

struct omni_timer;

/**
 * Callback function type for timer expiry.
 */
typedef void (*omni_timer_cb_t)(struct omni_timer *, void *);

#define MAX_SKIPLIST_DEPTH 10

/**
 * A structure describing a timer in RTE.
 */
struct omni_timer
{
	uint64_t expire;       /**< Time when timer expire. */
	struct omni_timer *sl_next[MAX_SKIPLIST_DEPTH];
	volatile union omni_timer_status status; /**< Status of timer. */
	uint64_t period;       /**< Period of timer (0 if not periodic). */
	omni_timer_cb_t f;      /**< Callback function. */
	void *arg;             /**< Argument to callback function. */
};


#ifdef __cplusplus
/**
 * A C++ static initializer for a timer structure.
 */
#define OMNI_TIMER_INITIALIZER {             \
	0,                                      \
	{NULL},                                 \
	{{OMNI_TIMER_STOP, OMNI_TIMER_NO_OWNER}}, \
	0,                                      \
	NULL,                                   \
	NULL,                                   \
	}
#else
/**
 * A static initializer for a timer structure.
 */
#define OMNI_TIMER_INITIALIZER {                      \
		.status = {{                         \
			.state = OMNI_TIMER_STOP,     \
			.owner = OMNI_TIMER_NO_OWNER, \
		}},                                  \
	}
#endif

/**
 * Initialize the timer library.
 *
 * Initializes internal variables (list, locks and so on) for the RTE
 * timer library.
 */
extern void omni_timer_subsystem_init(void);

/**
 * Initialize a timer handle.
 *
 * The omni_timer_init() function initializes the timer handle *tim*
 * for use. No operations can be performed on a timer before it is
 * initialized.
 *
 * @param tim
 *   The timer to initialize.
 */
extern void omni_timer_init(struct omni_timer *tim);

/**
 * Reset and start the timer associated with the timer handle.
 *
 * The omni_timer_reset() function resets and starts the timer
 * associated with the timer handle *tim*. When the timer expires after
 * *ticks* HPET cycles, the function specified by *fct* will be called
 * with the argument *arg* on core *tim_lcore*.
 *
 * If the timer associated with the timer handle is already running
 * (in the RUNNING state), the function will fail. The user has to check
 * the return value of the function to see if there is a chance that the
 * timer is in the RUNNING state.
 *
 * If the timer is being configured on another core (the CONFIG state),
 * it will also fail.
 *
 * If the timer is pending or stopped, it will be rescheduled with the
 * new parameters.
 *
 * @param tim
 *   The timer handle.
 * @param ticks
 *   The number of cycles (see omni_get_hpet_hz()) before the callback
 *   function is called.
 * @param type
 *   The type can be either:
 *   - PERIODICAL: The timer is automatically reloaded after execution
 *     (returns to the PENDING state)
 *   - SINGLE: The timer is one-shot, that is, the timer goes to a
 *     STOPPED state after execution.
 * @param tim_lcore
 *   The ID of the lcore where the timer callback function has to be
 *   executed. If tim_lcore is LCORE_ID_ANY, the timer library will
 *   launch it on a different core for each call (round-robin).
 * @param fct
 *   The callback function of the timer.
 * @param arg
 *   The user argument of the callback function.
 * @return
 *   - 0: Success; the timer is scheduled.
 *   - (-1): Timer is in the RUNNING or CONFIG state.
 */
int omni_timer_reset(struct omni_timer *tim, uint64_t ticks,
		    enum omni_timer_type type, unsigned tim_lcore,
		    omni_timer_cb_t fct, void *arg);


/**
 * Loop until omni_timer_reset() succeeds.
 *
 * Reset and start the timer associated with the timer handle. Always
 * succeed. See omni_timer_reset() for details.
 *
 * @param tim
 *   The timer handle.
 * @param ticks
 *   The number of cycles (see omni_get_hpet_hz()) before the callback
 *   function is called.
 * @param type
 *   The type can be either:
 *   - PERIODICAL: The timer is automatically reloaded after execution
 *     (returns to the PENDING state)
 *   - SINGLE: The timer is one-shot, that is, the timer goes to a
 *     STOPPED state after execution.
 * @param tim_lcore
 *   The ID of the lcore where the timer callback function has to be
 *   executed. If tim_lcore is LCORE_ID_ANY, the timer library will
 *   launch it on a different core for each call (round-robin).
 * @param fct
 *   The callback function of the timer.
 * @param arg
 *   The user argument of the callback function.
 */
void
omni_timer_reset_sync(struct omni_timer *tim, uint64_t ticks,
		     enum omni_timer_type type, unsigned tim_lcore,
		     omni_timer_cb_t fct, void *arg);

/**
 * Stop a timer.
 *
 * The omni_timer_stop() function stops the timer associated with the
 * timer handle *tim*. It may fail if the timer is currently running or
 * being configured.
 *
 * If the timer is pending or stopped (for instance, already expired),
 * the function will succeed. The timer handle tim must have been
 * initialized using omni_timer_init(), otherwise, undefined behavior
 * will occur.
 *
 * This function can be called safely from a timer callback. If it
 * succeeds, the timer is not referenced anymore by the timer library
 * and the timer structure can be freed (even in the callback
 * function).
 *
 * @param tim
 *   The timer handle.
 * @return
 *   - 0: Success; the timer is stopped.
 *   - (-1): The timer is in the RUNNING or CONFIG state.
 */
int omni_timer_stop(struct omni_timer *tim);


/**
 * Loop until omni_timer_stop() succeeds.
 *
 * After a call to this function, the timer identified by *tim* is
 * stopped. See omni_timer_stop() for details.
 *
 * @param tim
 *   The timer handle.
 */
void omni_timer_stop_sync(struct omni_timer *tim);

/**
 * Test if a timer is pending.
 *
 * The omni_timer_pending() function tests the PENDING status
 * of the timer handle *tim*. A PENDING timer is one that has been
 * scheduled and whose function has not yet been called.
 *
 * @param tim
 *   The timer handle.
 * @return
 *   - 0: The timer is not pending.
 *   - 1: The timer is pending.
 */
int omni_timer_pending(struct omni_timer *tim);

/**
 * Manage the timer list and execute callback functions.
 *
 * This function must be called periodically from EAL lcores
 * main_loop(). It browses the list of pending timers and runs all
 * timers that are expired.
 *
 * The precision of the timer depends on the call frequency of this
 * function. However, the more often the function is called, the more
 * CPU resources it will use.
 */
void omni_timer_manage(void);

/**
 * Dump statistics about timers.
 *
 * @param f
 *   A pointer to a file for output
 */
void omni_timer_dump_stats(void);

/**
 * Get a pseudo-random value.
 * 
 * This function generates pseudo-random numbers using the linear
 * congruential algorithm and 48-bit integer arithmetic, called twice
 * to generate a 64-bit value.
 *
 * @return
 *   A pseudo-random value between 0 and (1<<64)-1.
 */
static inline uint64_t omni_rand(void)
{
/* original code
	uint64_t val;
	val = lrand48();
	val <<= 32;
	val += lrand48();
	return val;
*/
	extern int rand(void);
	uint64_t val;
	val = rand();
	val <<= 32;
	val += rand();
	return val;

/* This get_random_bytes mechanism is poor performance that accessed 
 * character device frequently.
 */
/*	uint64_t val;
	uint32_t tmp;

	get_random_bytes(&tmp, sizeof(tmp));
	tmp >>= 1;
	val = tmp;
	val <<= 32;
	get_random_bytes(&tmp, sizeof(tmp));
	tmp >>= 1;
	val += tmp;
	return val;
*/
}

/**
 * Searches the input parameter for the least significant set bit
 * (starting from zero).
 * If a least significant 1 bit is found, its bit index is returned.
 * If the content of the input parameter is zero, then the content of the return
 * value is undefined.
 * @param v
 *     input parameter, should not be zero.
 * @return
 *     least significant set bit in the input parameter.
 */
static inline uint32_t omni_bsf32(uint32_t v)
{
	return (__builtin_ctz(v));
}

/**
 * Omni-timer event handler when timer expired.
 */
extern void omni_timer_thread_init(void);

/**
  omni-timer must know expire handler of omni-track.
 */
extern void (*omni_timer_expire_handle)(struct omni_timer *tim, void *ul_omni_track);



#endif /* _OMNI_TIMER_H_ */
