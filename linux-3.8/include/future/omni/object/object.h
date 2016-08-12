#ifndef _OBJ_H_
#define _OBJ_H_

#include <future/omni/object/object_set.h>
#include <future/omni/object/multi_key.h>
#include <future/command/vty.h>

extern int omni_object_init(void);
extern int omni_policy_init(void);
extern void omni_policy_switch(void);

extern omni_om_entry_table *get_om_entry_table(const int table_index);
extern omni_om_entry_table *omni_obj_base_get(int id);
extern void free_om_entry(omni_om_entry *entry);
extern void release_multi_key_root(multi_key_root *multi_key);

enum table_index {
	TABLE_FILTER,
	TABLE_NAT,
	TABLE_MANGLE,
	TABLE_NAT64 = TABLE_MANGLE,
	TABLE_MAX
};

extern struct check_use_modules *omni_policy_get_backup_modules(void);
extern struct check_use_modules *omni_policy_get_master_modules(void);

extern struct omni_xt_table_info *omni_policy_get_backup_v4_table(int index);
extern struct omni_xt_table_info *omni_policy_get_master_v4_table(int index);
extern struct omni_xt_table_info *omni_policy_get_backup_v6_table(int index);
extern struct omni_xt_table_info *omni_policy_get_master_v6_table(int index);

extern void omni_policy_set_backup_v4_table(int index, struct omni_xt_table_info *info);
extern void omni_policy_set_backup_v6_table(int index, struct omni_xt_table_info *info);

extern omni_om_entry_table *omni_policy_get_master_obj(int id);
extern omni_om_entry_table *omni_policy_get_backup_obj(int id);

extern struct erfc_tables *omni_policy_get_master_erfc(void);
extern struct erfc_tables *omni_policy_get_backup_erfc(void);
extern int omni_erfc_free_array_all(void);

extern int omni_obj_alloc_array(int obj_id);
extern int omni_obj_free_array_all(void);
extern void omni_obj_prepare(void);

extern void omni_policy_dump_all(struct vty *vty);
#endif  /* _OBJ_H_ */
