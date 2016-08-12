#ifndef _OMNI_HOOK_UTIL_H
#define _OMNI_HOOK_UTIL_H

#include <future/omni.h>

enum module_bypass {
	FW_APP_BYPASS_BIT = 0,
	FW_APP_BYPASS = (1 << FW_APP_BYPASS_BIT),

	FW_IPS_BYPASS_BIT = 1,
	FW_IPS_BYPASS = (1 << FW_IPS_BYPASS_BIT),

	FW_HTTP_BYPASS_BIT = 2,
	FW_HTTP_BYPASS = (1 << FW_HTTP_BYPASS_BIT)
};

extern int fw_bypass_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
extern int fw_bypass_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);

#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_APP)
extern int (*do_app_match_hook)(struct sk_buff *skb,
	struct net_device *in, struct net_device *out);

int omni_do_app_match(struct sk_buff *, struct net_device *, struct net_device *);

#else
static inline int omni_do_app_match(struct sk_buff *skb, struct net_device *in, struct net_device *out) {
	return OMNI_ACCEPT;
}
#endif/*CONFIG_FERRET_FW_PROFILES_APP*/

#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_WEB)
extern int (*do_web_profile_hook)(struct sk_buff *skb,
	struct net_device *in, struct net_device *out);

int do_web_profile(struct sk_buff *, struct net_device *, struct net_device *);
#else
static inline int do_web_profile(struct sk_buff *skb, struct net_device *in, struct net_device *out) {
	return OMNI_ACCEPT;
}
#endif/*CONFIG_FERRET_FW_PROFILES_WEB*/

//this is temporary code
#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_APP)
extern int (*temp_fips_proc_hook)(struct sk_buff *skb);
int do_fips_proc(struct sk_buff *);

#else
static inline int do_fips_proc(struct sk_buff *skb) {
	return OMNI_ACCEPT;
}
#endif/*CONFIG_FERRET_FW_PROFILES_APP*/

#if IS_ENABLED(CONFIG_FERRET_FW_PROFILES_APP)
void do_app_profile_ext_add(struct omni_track *track);
#endif/*CONFIG_FERRET_FW_PROFILES_APP*/

extern int omni4_profile(struct sk_buff *skb);
extern int omni6_profile(struct sk_buff *skb);

extern int omni_dpi(struct sk_buff *skb);
#endif/*_OMNI_HOOK_UTIL_H*/

