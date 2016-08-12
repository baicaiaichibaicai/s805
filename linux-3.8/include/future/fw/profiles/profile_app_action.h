/*
 * Future Systems
 */
#ifndef _PROFILE_APP_ACTION_H
#define _PROFILE_APP_ACTION_H

#include <linux/skbuff.h>


typedef enum {
	INVALID_APP_ACTION = 0,
	APP_DETECT = 0x1,
	APP_BLOCK,
	APP_QOS,
	MAX_APP_ACTION
} app_action_type;

typedef struct {
	app_action_type type;
	unsigned short qos_obj_idx;
} app_action;


#endif/*_PROFILE_APP_ACTION_H*/

