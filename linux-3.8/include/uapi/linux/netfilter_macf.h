#ifndef __LINUX_MACFILTER_H
#define __LINUX_MACFILTER_H

/* ARP-specific defines for netfilter.
 * (C)2002 Rusty Russell IBM -- This code is GPL.
 */

#include <linux/netfilter.h>

/* There is no PF_ARP. */
#define OMNI_MACF			0

/* MACFilter Hooks */
#define OMNI_MACF_IN		0
#define OMNI_MACF_OUT		1
#define OMNI_MACF_FORWARD	2
#define OMNI_MACF_NUMHOOKS	3

#endif /* __LINUX_MACFILTER_H */
