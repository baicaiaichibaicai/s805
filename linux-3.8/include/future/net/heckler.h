#ifndef __HECKLER_H__
#define __HECKLER_H__


// MSG Queue Key
#define HECKLER_MSQ_KEY			0x4872

#define MAX_IP_COUNT_OF_ACCEPT_HOST		4
#define MAX_MAC_COUNT_OF_ACCEPT_HOST	4

#define IPM_NONE		0
#define IPM_DETECT		1
#define IPM_DENY		2

#define IPM_SINGLE		1
#define IPM_RANGE		2
#define IPM_NETMASK		3

#define IPM_FREESKB		1

// Heckler type
enum {
    HECKLER_ARP_REQ_SELF_MAC = 1,
    HECKLER_ARP_REP_SELF_MAC,
    HECKLER_ARP_REQ_OTHER_MAC,
    HECKLER_ARP_REP_OTHER_MAC
};


struct heckler_info {
    u_char		type;
    u_char		interval;
    u_short		count;
    u_int		sip;
    u_int		dip;
    char		ifname[IFNAMSIZ];
    u_char		smac[ETH_ALEN];
    u_char		dmac[ETH_ALEN];

}__attribute__((packed));

struct heckler_msgbuf {
    long		mtype;
    struct heckler_info info;
}__attribute__((packed));

struct ipm_network_struct {
    char		ifname[IFNAMSIZ];
	u_int 		ip1;
	u_int 		ip2;
	u_char		type;
	u_char		action;
	char		rvd[38];
}__attribute__((packed)); // 56 bytes

struct ipm_accept_host_struct {
    char		ifname[IFNAMSIZ];
	u_int		ip[MAX_IP_COUNT_OF_ACCEPT_HOST];
	u_char		mac[MAX_MAC_COUNT_OF_ACCEPT_HOST][ETH_ALEN];
	time_t		sdate;		//시작날짜
	time_t		edate;		//만료 날짜
	char		rvd[32];	
}__attribute__((packed)); 

extern int heckler_msqid;

// Write bh_lock
#define HECKLER_WRITE_LOCK_BH		write_lock_bh(&heckler_event_lock)
#define HECKLER_WRITE_UNLOCK_BH		write_unlock_bh(&heckler_event_lock)

// wakeup event
#define HECKLER_WAKEUP_EVENT		do {	\
						if (heckler_event_pid)\
						wake_up_process(heckler_event_pid);\
					} while(0) 
// event_pool max
#define HECKLER_EVENT_POOL_MAX		1000 

extern rwlock_t heckler_event_lock;
extern struct heckler_msgbuf *g_heckler_msg_pool;
extern struct task_struct *heckler_event_pid;
extern int heckler_event_pool_thread(void *arg);

extern int heckler_msgqueue_init(void);
void heckler_msgsnd(u_char type, u_char interval, u_char count,
	u_int sip, u_int dip, u_char *smac, u_char *dmac, char *ifname);

#endif
