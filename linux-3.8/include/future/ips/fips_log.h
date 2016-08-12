#ifndef __FIPS_LOG_H__
#define __FIPS_LOG_H__
#include <future/log/logv2.h>

/* IPS log*/
int put_ips_log(log_param_struct *log_param, void *en, void *packet);
int put_audit_log(log_param_struct *log_param, void *en,void *packet);
void init_audit_log(void);
void get_audit_log(void);

#endif
