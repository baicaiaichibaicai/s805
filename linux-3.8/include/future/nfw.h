#ifndef __NFW_H__
#define __NFW_H__


enum {
	FIRMWARE_IDX,
	RAMDISK_IDX,
	IMAGE_IDX,
	DO_IDX,
	IPS_IDX,
	APP_IDX,
	AV1_IDX,
	AV2_IDX,
	AS_IDX,
	KISCOM_IDX,
	HTTP_IDX,
	MALWARE_IDX,
	VERSION_INFO_MAX
};


struct version_attributes{
	int len;
	int index;
	char *name;
};


extern int end_of_boot, g_initialize_flag;
extern int g_main_memory, g_memory_usage_percent, g_memory_usage, g_cpu_usage;
extern long long g_ppp_flag, g_dhcp_flag;
extern char g_cpuname[64];
extern long long g_ppp_flag;

extern char *version_list_info[VERSION_INFO_MAX];
extern char *trap_version_list_info[VERSION_INFO_MAX];


// proc_dir_entry
extern struct proc_dir_entry *proc_ferret_network;
extern struct proc_dir_entry *proc_ferret_network_routing;
extern struct proc_dir_entry *proc_ferret_system_license;
extern struct proc_dir_entry *proc_ferret_datasheet;			// datasheet
extern struct proc_dir_entry *proc_ferret_fw;
extern struct proc_dir_entry *proc_ferret_monitor_fw;
extern struct proc_dir_entry *proc_ferret_monitor_network;

extern int MAX_ETH_DEV;						   // 물리 인터페이스 개수
extern char *g_adv_routing;
extern void proc_ferret_init(void);
#ifdef CONFIG_X86
extern int lanner_bios_wdt_port;
extern unsigned char read_sio_reg(int port, int LDN, int reg);
#endif
extern int g_mgt_port;
extern int g_secure_exit;
extern int g_service_on;
extern int g_ethernet_88d9;


#endif
