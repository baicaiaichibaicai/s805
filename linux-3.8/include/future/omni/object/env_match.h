#ifndef _ENV_MATCH_H_
#define _ENV_MATCH_H_

#include <future/omni/object/object_set.h>
#include <future/omni/netfilter/omni_x_tables.h>

struct env_ip {
    __u32 ipv4;
    __u32 ipv6[4];
};

extern struct env_ip g_env_ip[MAX_ENV_CNT];

int set_env_ipv4(const char *ifname, __u8 idx);
int match_env_ipv4(const omni_om_entry *entry, const __u32 ipv4);
int match_env_ipv6(const omni_om_entry *entry, const __u32 *ipv6);

#endif

