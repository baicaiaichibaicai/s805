#ifndef _BINARY_SEARCH_H_
#define _BINARY_SEARCH_H_

#include <future/omni/object/object_set.h>

int compare_ipv4(const om_object *om_object, const char *search_value);
int compare_ipv6(const om_object *om_object, const char *search_value);
int compare_service(const om_object *om_object, const char *search_value);
int compare_group(const om_object *om_object, const char *search_value);
int compare_cc_obj(const om_object *om_object, const char *search_value);
int binary_search(int (*compare)(const om_object *, const char *), const omni_om_entry *entry, const char *search_value);
__u16 cc_binary_search(const __u32 ipv4);


#endif

