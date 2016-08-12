#ifndef __SYSFS_INSTANCE_H__
#define __SYSFS_INSTANCE_H__

extern char *strtok_r(char *s, const char *delim, char **save_ptr);
void ddos_profile_control_attr_init(void);
ssize_t ferret_attr_show(struct kobject *kobj, struct attribute *attr, char *buf);
ssize_t ferret_attr_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len);

struct system_sysfs
{
	struct kobject kobj;
} __attribute__((packed));

struct traffic_sysfs
{
	struct kobject kobj;
} __attribute__((packed));

#endif
