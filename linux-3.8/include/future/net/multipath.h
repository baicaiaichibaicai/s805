#ifndef __MULTIPATH_H__
#define __MULTIPATH_H__

#include <future/general.h>
#include <net/ip_fib.h>


#define MULTIPATH_ENTRY_MAX				MAX_NET_INFO_NUM 

#define MULTIPATH_TYPE_NONE				0
#define MULTIPATH_TYPE_PPPOE			1
#define MULTIPATH_TYPE_DHCP				2



enum {
	MULTIPATH_TYPE_DEFAULT = 0,
	MULTIPATH_TYPE_SRC,
	MULTIPATH_TYPE_DST,
	MULTIPATH_TYPE_SRC_DST,
	MULTIPATH_TYPE_EXPLICIT,
	MULTIPATH_TYPE_BANDWIDTH,
	MULTIPATH_TYPE_PACKET,
	MULTIPATH_TYPE_MAX
};

#define LQC_BY_CHECKER				1
#define LQC_BY_BANDWIDTH			2

struct mpr_entry {
	u_short     type;						// MULTIPATH_TYPE_PPPOE, MULTIPATH_TYPE_DHCP
	u_short     priority;
	u_int       gateway;

	u_short		timeout_cnt;
	u_short		temp_cnt;
	int			down_type;					// LQC_BY_CHECKER, LQC_BY_BANDWIDTH
	int			stay_down_seconds;
	int			rvd;
    __u64       bandwidth_measure;
	struct LQC_checker {
		u_short     monitoring_period;
		u_short     down_limit;
		u_short		pre_down_cnt;
		u_short		cur_down_cnt;

		int			down_period;
		int			monitoring_seconds;
	} checker;

	struct LQC_bandwidth {
		int			down_period;
		u_short		exceed_cnt;
		u_short		exceed_seconds;
		__u64		tx_limit;
		__u64		tx_bps;
		__u64		pre_tx_bytes;
	} bandwidth;
};

extern int g_multipath_timeout, g_LQC_checker_flag, g_LQC_bandwidth_flag;
extern long long g_mpr_interface;
extern long long g_mpr_30priority_interface, g_mpr_20priority_interface, g_mpr_10priority_interface, g_mpr_down_interface;

extern struct mpr_entry g_mpr_entry[MULTIPATH_ENTRY_MAX];

void LQC_checker_down(int iface, char *dev_name);
void cmd_print_LQC_checker_status(struct vty* vty);
void cmd_print_LQC_bandwidth_status(struct vty* vty);
int get_mpr_status(int iface);
int LQC_checker_proc_init(void);
int LQC_bandwidth_proc_init(void);
int LQC_bandwidth_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
void set_mpr_30priority_interface(int iface);
void set_mpr_20priority_interface(int iface);
void set_mpr_10priority_interface(int iface);
int get_high_priority_mpr_interface(int iface);

void check_multipath_line(void);
int multipath_proc_init(void);
void ferret_fib_select_multipath_localout(struct fib_result *res, int iface, int debug_flag);
void ferret_fib_select_multipath(struct fib_result *res, struct sk_buff *skb);

extern int bandwidth_measure_proc_read(char *page, char **start, 
    off_t off, int count, int *eof, void *data);
extern int bandwidth_measure_proc_write(struct file *file, const char __user *buffer,
    unsigned long count, void *data);


#endif
