#ifndef __SYSFS_FERRET_H__
#define __SYSFS_FERRET_H__

extern struct kset ferret_kset;
extern struct kobj_type ferret_ktype;
extern struct kobject *ddos_kobj;

void sysfs_ferret_init(void);

#endif
