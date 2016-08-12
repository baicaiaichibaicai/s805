#ifndef __TRAFFIC_H__
#define __TRAFFIC_H__

#include <linux/types.h>
#include <linux/timer.h>
#include <future/command/vty.h>


struct traffic_monitor_iface {
	struct net_device				*dev;
	uint64_t						sum_rx_bytes;
	uint64_t						sum_tx_bytes;
	uint64_t						sum_rx_packets;
	uint64_t						sum_tx_packets;
	uint64_t						rx_bytes[3];		// [0][1]: stores ifconfig value, [2] stores the difference 
	uint64_t						tx_bytes[3];
	uint64_t						rx_packets[3];
	uint64_t						tx_packets[3];
//	int								idx;
//	int								rvd;
};


#define MAX_MONITOR_IFACE_COUNT			8
#define TRAFFIC_MONITOR_INTERVAL		2
#define MAX_MONITOR_LOOP_COUNT			60

struct system_info {
	uint32_t						cpu_usage;
	uint32_t						mem_usage;
	uint32_t						sess_usage;
	uint32_t						cps_usage[2];
	uint32_t						log_usage;
};

struct traffic_monitor_info {
	unsigned long 					start_jiffies;
	u64								interface_flag;	
	unsigned char					iface_count;			// iface count;
	char							output_flag;			// 0:kernel, 1:websmc
	char							idx;					// [0][1]: stores ifconfig value, [2] stores the difference 
	unsigned char					interval;				// traffic monitor interval
	unsigned char					max_loop_count;			// MAX_MONITOR_LOOP_COUNT
	unsigned short					loop_count;
	char							view_flag;				// 0:off, 1:on
	struct system_info				s_info;
	struct traffic_monitor_iface	info[MAX_MONITOR_IFACE_COUNT];
	struct timer_list				m_timer;
	struct vty						*vty;
};

extern unsigned char show_traffic_enable;
extern void show_traffic_flag_enable(void);
extern void show_traffic_flag_disable(void);

char *convert_to_unit_string(uint64_t num, char *p_buf);
void show_traffic_function(struct traffic_monitor_info *p_tm);
int make_traffic_monitor_info(u64 interface_flag, struct vty *vty, struct traffic_monitor_info *p_tm,
		unsigned char max_loop_count);

void calc_traffic_monitor_info(struct traffic_monitor_info *p_tm);
int show_traffic_monitor_info(struct traffic_monitor_info *p_tm, char *page);
void traffic_dev_put(struct traffic_monitor_info *p_tm);
void traffic_dev_hold(u64 interface_flag, struct traffic_monitor_info *p_tm);
void init_traffic_calc_info(struct traffic_monitor_info *p_tm);
#endif

