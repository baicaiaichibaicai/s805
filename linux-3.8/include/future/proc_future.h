#ifndef _PROC_FUTURE_H
#define _PROC_FUTURE_H

#include <linux/proc_fs.h>
	
extern struct proc_dir_entry *proc_omni_ferret;

extern void __init proc_future_init(void);

extern int omni_dump_cond_read(char *page, char **start, off_t off, 
		int count, int *eof,void *data);
extern int omni_dump_cond_write(struct file *file, const char __user *buffer, 
		unsigned long count, void *data);

extern int show_iface_info_init(void);
extern int iface_op_mode_proc_write(struct file *file, const char __user *buffer,
        unsigned long count, void *data);
#ifdef CONFIG_FERRET_IPS
int monips_top10_procfs_init(struct proc_dir_entry *p_proc);
int fips_proc_init(struct proc_dir_entry *p_proc);
#endif //CONFIG_FERRET_IPS

#endif



