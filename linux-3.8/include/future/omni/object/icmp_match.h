#ifndef _ICMP_MATCH_H_
#define _ICMP_MATCH_H_

#include <future/omni/object/object_set.h>

int match_icmp(const omni_om_entry *entry, const __u8 *search_value, unsigned int *timeout);
int match_icmpv6(const omni_om_entry *entry, const __u8 *search_value, unsigned int *timeout);

#endif

