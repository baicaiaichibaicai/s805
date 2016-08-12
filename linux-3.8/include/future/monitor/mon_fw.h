#ifndef _MON_FW_H_
#define _MON_FW_H_

#include <linux/types.h>

int mon_fw_sess_info_proc_read(char *page, char **start, off_t off, int count, int *eof,void *data);
int mon_fw_sess_stat_proc_read(char *page, char **start, off_t off, int count, int *eof,void *data);

#endif
