#ifndef _FUTURE_OMNI_H
#define _FUTURE_OMNI_H

#include <linux/skbuff.h>

/* Response from hook functions. */
#define OMNI_DROP			0
#define OMNI_ACCEPT			1
#define OMNI_STOLEN			2
#define OMNI_QUEUE			3
#define OMNI_REPEAT			4
#define OMNI_STOP			5
#define OMNI_ASYNC			6

extern unsigned char omni_track_enable;

extern void omnipath_init(void);
extern void omnipath_init_last(void);
extern int omni_track_init(void);
extern int omni_track_l3proto_ipv4_init(void);
extern int omni_track_l3proto_ipv6_init(void);
extern int omni_gc_init(void);
extern int omni_track_block_init(void);

extern unsigned int g_check_use_filter __read_mostly;
extern unsigned int g_check_use_mangle __read_mostly;
extern unsigned int g_check_use_nat __read_mostly;
extern unsigned int g_check_use_log __read_mostly;
extern unsigned int g_check_use_profile __read_mostly;
extern unsigned int g_check_use_limit __read_mostly;
extern unsigned int g_check_use_vpn __read_mostly;

enum check_use_profile{
	OMNI_USE_WEB_BIT = 1,
	OMNI_USE_WEB = (1 << OMNI_USE_WEB_BIT),

	OMNI_USE_APP_BIT = 2,
	OMNI_USE_APP = (1 << OMNI_USE_APP_BIT),

	OMNI_USE_IPS_BIT = 3,
	OMNI_USE_IPS = (1 << OMNI_USE_IPS_BIT),

	OMNI_USE_AVS_BIT = 4,
	OMNI_USE_AVS = (1 << OMNI_USE_AVS_BIT),

	OMNI_USE_SSLINSPECT_BIT = 5,
	OMNI_USE_SSLINSPECT = (1 << OMNI_USE_SSLINSPECT_BIT)
};

#define PROFILE_COMPARISON (OMNI_USE_WEB | OMNI_USE_APP | OMNI_USE_IPS | OMNI_USE_AVS | OMNI_USE_SSLINSPECT)

#endif
