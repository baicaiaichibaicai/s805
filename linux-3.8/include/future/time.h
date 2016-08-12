#ifndef	_FUTURE_TIME_H
#define _FUTURE_TIME_H

#include <linux/time.h>
#include <linux/rtc.h>

/* Return the current time and put it in *TIMER if TIMER is not NULL.  */
extern time_t time (time_t *__timer);

/* Return the `struct tm' representation
   of *TIMER in the local timezone.  */
extern struct tm *localtime (__const time_t *__timer);
extern struct tm *localtime_r(__const time_t * tim_p, struct tm *res);

/* Return a string of the form "Day Mon dd hh:mm:ss yyyy\n"
   that is the representation of TP in this format.  */
extern char *asctime (__const struct tm *__tp);

/* Equivalent to `asctime (localtime (timer))'.  */
extern char *ctime (__const time_t *__timer);

extern struct tm * gmtime(__const time_t *timer);

extern time_t x_mktime(struct tm *tm);

extern int gettimeofday(struct timeval *tv, struct timezone *tz);

void to_tm (int tim, struct rtc_time *tm);

/* Defined in localtime.c.  */
extern char *__tzname[2];	/* Current timezone names.  */
extern int __daylight;		/* If daylight-saving time is ever in use.  */
extern long int __timezone;	/* Seconds west of UTC.  */

/* Nonzero if YEAR is a leap year (every 4 years,
   except every 100th isn't, and every 400th is).  */
# define __isleap(year)	\
  ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))


#endif /*_FUTURE_TIME_H*/

