/* /include/future/admin/qsort.h
 *
 * (C) 2013 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */
					
#ifndef _COMMAND_QSORT_H
#define _COMMAND_QSORT_H

void qsort(void *base, size_t nel, size_t width,
	int (*compar)(const void *, const void *));

#endif /*_COMMAND_QSORT_H*/

