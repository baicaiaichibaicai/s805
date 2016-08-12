#ifndef _OMNI_MARK_H
#define _OMNI_MARK_H

#include <linux/types.h>

struct omni_mark_tginfo2 {
	__u32 mark, mask;
};

struct omni_mark_mtinfo1 {
	__u32 mark, mask;
	__u8 invert;
};

#endif
