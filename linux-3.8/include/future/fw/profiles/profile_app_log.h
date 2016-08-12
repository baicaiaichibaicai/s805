/*
 * Future Systems
 */
#ifndef _PROFILE_APP_LOG_H
#define _PROFILE_APP_LOG_H

#include "decode.h"

void app_init_proto_names(void);
void app_fini_proto_names(void);

void app_print_packet(Packet *p);

#endif/*_PROFILE_APP_LOG_H*/

