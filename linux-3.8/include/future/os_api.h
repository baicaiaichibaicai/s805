#ifndef __OS_API_H__
#define __OS_API_H__

#include <linux/netdevice.h>



void get_memory_status(void);
int get_cpu_usage(void);

int get_link_status(struct net_device *dev);
int get_link_speed(struct net_device *dev);
int get_link_duplex(struct net_device *dev);

char *dev_name_get_by_iface(int iface);
struct net_device *dev_get_by_ifindex(int ifindex);
struct net_device *dev_get_by_iface(int iface);
struct net_device *dev_get_by_dev_name(char *devname);
int iface_get_by_dev_name(char *p_dev_name);
int iface_get_by_dev(struct net_device *dev);
int thread_background(void *arg);
int udp_recvfrom(struct socket *sock, void *buffer, int len, struct sockaddr_in *sa);
int udp_recvfrom6(struct socket *sock, void *buffer, int len, struct sockaddr_in6 *sa);


#endif

