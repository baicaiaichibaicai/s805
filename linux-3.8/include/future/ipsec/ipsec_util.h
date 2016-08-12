#ifndef __IPSEC_UTIL_H_
#define __IPSEC_UTIL_H_

#include <net/xfrm.h>
#include <linux/skbuff.h>
#include <future/route_base.h>
#include <future/ipsec/ipsec.h>
#include <future/command/vty.h>
#include <net/tcp.h>

#define TUNNEL_ROUTING_MAX			ROUTE_IP_REDIRECT_TABLE_MAX
#define NET_SINGLE      			ROUTE_ACCESS_LIST_TYPE_SINGLE
#define NET_NETMASK     			ROUTE_ACCESS_LIST_TYPE_NETMASK
#define NET_RANGE       			ROUTE_ACCESS_LIST_TYPE_RANGE
#define NET_ANY         			ROUTE_ACCESS_LIST_TYPE_ANY

enum XFRM_STATE_t
{
	STATE_EMPTY		= 0,
	STATE_PROGRESS	= 0x1,
	STATE_FINISH	= 0x2,
	STATE_DISABLE	= 0x4,
	STATE_MAX		= 0x8,
};

#define  ACC_EN 0
#define  ACC_DE 1
#define  ACC_TX 0
#define  ACC_RX 1
#define  ACC_MAX 2

#define	MTU_FROM_MSS(mss)	((mss) + sizeof(struct tcphdr) + sizeof(struct iphdr))
#define	MSS_FROM_MTU(mtu)	((mtu) - sizeof(struct tcphdr) + sizeof(struct iphdr))
#define	TCP_DATA_OFFSET(th)	(((struct tcphdr *)th)->doff*4)
#define	TCP_OPT_LEN(th)		(TCP_DATA_OFFSET(th) - sizeof(struct tcphdr))

static inline __be16 *find_mss_ptr(struct tcphdr *th)
{
	int optsize;
	int remains = TCP_OPT_LEN(th);
	const u8 *opt = (const u8 *)(th + 1);
	while (remains > 0) {
		switch (*opt) {
		case TCPOPT_EOL:
			return NULL;
		case TCPOPT_NOP:
			remains--;
			opt++;
		case TCPOPT_MSS:
			return (*++opt == TCPOLEN_MSS && th->syn) ?
				(__be16 *)++opt : NULL;
		default:
			optsize = *(opt + 1);
			if (optsize < 2 || optsize > remains)
				return NULL;
			remains -= optsize;
			opt += optsize;
		}
	}
	return NULL;
}

static inline void adjust_mss(struct tcphdr *th, u16 mtu)
{
	u16 *pmss;
    if (th->syn 
		&& (pmss = find_mss_ptr(th)) != NULL 
		&& MTU_FROM_MSS(be16_to_cpu(*pmss)) > mtu)
       	*pmss = (u16)cpu_to_be16(MSS_FROM_MTU(mtu));
}

extern  bool ipsec_init(int );
extern  void dump_ipsecsa(struct vty *vty);
extern  void dump_ipsecpol(struct vty *vty);
extern  int dump_one_ipsecsa(struct xfrm_state *x, int count, void *_vty);

struct xfrm_state * get_xfrm_state_from_addr(xfrm_address_t *, xfrm_address_t *, int);

extern  int is_future_aria256(void);

extern int ipsec_stat_proc_read(char *page, char **start, off_t off, int count,
		int *eof, void *data);
extern int aria_proc_read(char *page, char **start,off_t off, int count,
    int *eof,void *data);
extern int aria_proc_write(struct file *file, const char __user *buffer,
    unsigned long count, void *data);
extern int tunnel_routing_proc_read(char *page, char **start,off_t off, int count,
    int *eof,void *data);
extern int tunnel_routing_proc_write(struct file *file, const char __user *buffer,
    unsigned long count, void *data);
extern int measure_proc_read(char *page, char **start,off_t off, int count,
    int *eof,void *data);
extern int measure_proc_write(struct file *file, const char __user *buffer,
    unsigned long count, void *data);
extern int ipsec_traffic_proc_read(char *page, char **start,off_t off, int count,
    int *eof,void *data);
extern int ipsec_traffic_proc_write(struct file *file, const char __user *buffer,
    unsigned long count, void *data);
extern int ipsec_acc_proc_read(char *page, char **start, off_t off, int count,
    int *eof,void *data);
extern int plain_acc_proc_read(char *page, char **start, off_t off, int count,
    int *eof,void *data);

//extern int sainfo_proc_read(char *page, char **start,off_t off, int count, int *eof,void *data);
//extern int sainfo_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
//extern int ipsecdebug_proc_read( char *page, char **start, off_t off, int count, int *eof, void *data) ;
//extern int ipsecdebug_proc_write(struct file *file, const char __user *buffer , unsigned long count , void *data);
extern int ipsec_admin_proc_read( char *page, char **start, off_t off, int count, int *eof, void *data);
extern int ipsec_admin_proc_write( struct file *file, const char __user *buffer, unsigned long count, void *data);

extern  unsigned int get_tunnel_routing_ip(unsigned int, unsigned int);

extern struct xfrm_state *ipsecsa_alloc(void);
extern void ipsecsa_free(struct xfrm_state *);
extern void xfrm_selector_ext_free(struct xfrm_state *);
extern int ipsec_encrypt4(struct sk_buff *skb);
extern int ipsec_encrypt6(struct sk_buff *skb);
extern int ipsec_decrypt4(struct sk_buff *skb);
extern int ipsec_decrypt6(struct sk_buff *skb);
extern void ipsec_set_tunnel_count(int num);
extern void ipsec_tunnel_flush_count(void);
extern int check_ipsec_abnormal(struct sk_buff *skb, struct omni_track *track);

extern void ipsec_en_inc(int len);
extern void ipsec_de_inc(int len);
extern void plain_tx_inc(int len);
extern void plain_rx_inc(int len);



extern bool __xfrm_selector_ext_match(struct xfrm_selector_ext *net, const struct flowi *fl,unsigned short family);


#define get_xfrm_state_from_two_addr(x,y)   get_xfrm_state_from_addr(x,y,0)
#define get_xfrm_state_from_src_addr(x)     get_xfrm_state_from_addr(x,0,1)
#define get_xfrm_state_from_dst_addr(x)     get_xfrm_state_from_addr(0,x,2)
#define get_xfrm_state_from_one_addr(x)     get_xfrm_state_from_addr(x,0,3)

#define get_xfrm_state_from_two_addr6(x,y)   get_xfrm_state_from_addr(x,y,0x80000000)
#define get_xfrm_state_from_src_addr6(x)     get_xfrm_state_from_addr(x,0,0x80000001)
#define get_xfrm_state_from_dst_addr6(x)     get_xfrm_state_from_addr(0,x,0x80000002)
#define get_xfrm_state_from_one_addr6(x)     get_xfrm_state_from_addr(x,0,0x80000003)

#define VPNDBG(vty, fmt, args...) \
	do { \
		if ( vty ) vty_out(vty, fmt, ##args); else debug(DL_IPSEC, fmt, ##args); \
	}while(0)


extern __u64 g_ipsec_iface;
extern int fast_ipsec;

#endif /*__IPSEC_UTIL_H__*/
