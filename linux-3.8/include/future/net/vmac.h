#ifndef __VMAC_H__
#define __VMAC_H__


struct vmac_dev_info {
	struct in_addr      ip;
	struct in_addr      vip;
	struct in6_addr     ip6;
	struct in6_addr     vip6;
	unsigned char       vmac[8];        // for 8byte align..
};

#define MAX_PROXY_MAC_COUNT			4

struct mac_info {
	unsigned char		value[ETH_ALEN];
	unsigned short		type;
};

struct proxy_mac_dev_info {
	int					cnt;
	int					ifindex;
	struct mac_info		mac[MAX_PROXY_MAC_COUNT];
};



//extern long long g_vmac_interface_flag, g_proxy_mac_interface_flag;
//extern struct vmac_dev_info gz_vmac_dev_info[MAX_INTERFACE_COUNT][2];
extern struct vmac_dev_info gz_vmac_dev_info[64][2];
extern struct proxy_mac_dev_info gz_proxy_mac_dev_info[64];


long long get_vmac_interface_flag(void);
long long match_vmac_interface_info(int iface);

long long get_proxy_mac_interface_flag(void);
long long match_proxy_mac_interface_info(int iface);

int set_interface_ip_to_vmac_dev_info(int iface, struct in_addr *p_ip, struct in6_addr *p_ip6);
int add_vmac_to_vmac_dev_info(int iface, struct in_addr *p_vip, struct in6_addr *p_vip6, unsigned char group_num, int primary);
int del_vmac_from_vmac_dev_info(int iface, unsigned char group_num, int force);
unsigned char *vmac_get_by_saddr4(__be32 saddr, int iface);
unsigned char *vmac_get_by_skb(struct sk_buff *skb, int iface);


void reset_proxy_mac_dev_info(void);
int check_mac_from_proxy_mac_dev_info(int iface, unsigned char *p_mac);
int add_proxy_mac_to_proxy_mac_dev_info(int iface, unsigned char *p_mac, unsigned short mac_type);
int del_proxy_mac_from_proxy_mac_dev_info(int iface, unsigned char *p_mac);


#endif

