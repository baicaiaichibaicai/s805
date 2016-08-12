#ifndef _MULTI_KEY_H_
#define _MULTI_KEY_H_

#include <asm/types.h>

typedef struct _multi_key_entry
{
	__u16 dst_start_port;
	__u8 *rule_type_array;

	int rule_count;
	int *rule_index_array;

}multi_key_entry;


typedef struct _multi_key_root
{
	int count;
	multi_key_entry **entry_array;

}multi_key_root;

#endif
