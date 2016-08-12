#ifndef __INET_DEV_H__
#define __INET_DEV_H__


struct ifa4_base {
	__be32				ip;
	__be32				mask;
	unsigned char		flags;
	char				rvd[7];
};

struct ifa6_base {
	struct in6_addr		addr;
	__u32				prefix;
	unsigned char		flags;
	char				rvd[7];
};

struct ifa_base_head {
	char            	name[IFNAMSIZ];
	int					cnt_4;
	int					cnt_6;
	int					base_idx;
	int 				ifindex;
};


int ifindex_get_by_iface(int iface);

// flag : 0 = search only interface ip, 1 = search all ip
bool match_ifa4_base_info(int iface, __be32 ip, int flag);
bool match_ifa6_base_info(int iface, struct in6_addr *p_addr, int flag);
struct ifa4_base *get_ifa4_base_info_by_iface(int iface);
struct ifa6_base *get_ifa6_base_info_by_iface(int iface);

void print_ifa4_base_info(struct vty *vty);
void print_ifa6_base_info(struct vty *vty);


#endif

