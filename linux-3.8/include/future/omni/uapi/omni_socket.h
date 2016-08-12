#ifndef _OMNI_SOCKET_H
#define _OMNI_SOCKET_H

#include <linux/types.h>

enum {
	OMNI_SOCKET_TRANSPARENT = 1 << 0,
};

struct omni_socket_mtinfo1 {
	__u8 flags;
};

extern bool 
socket_match(const struct sk_buff *skb, struct omni_xt_action_param *par,
	     const struct omni_socket_mtinfo1 *info);

#endif
