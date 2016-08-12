#define MINUTE 60
#define MON_IFACE_UPDATE_TIMEOUT        2
#define MAX_IFACE_MON_HOUR				6
#define MAX_U64_SIZE    ~((u64)0x00) - 1

typedef struct {
	u64 prc64[MAX_IFACE_MON_HOUR];
	u64 prc127[MAX_IFACE_MON_HOUR];
	u64 prc255[MAX_IFACE_MON_HOUR];
	u64 prc511[MAX_IFACE_MON_HOUR];
	u64 prc1023[MAX_IFACE_MON_HOUR];
	u64 prc1522[MAX_IFACE_MON_HOUR];
} net_dev_stats_total_struct;

typedef struct {
    uint64_t tcp_pkt;
    uint64_t tcp_bytes; 
    uint64_t udp_pkt;
    uint64_t udp_bytes;
    uint64_t icmp_pkt;
    uint64_t icmp_bytes;
    uint64_t esp_pkt;
    uint64_t esp_bytes;
    uint64_t other_pkt;
    uint64_t other_bytes;
} protocol_traffic_struct;

int mon_net_protocol_proc_read(char *page, char **start, off_t off, int count, int *eof,void *data);
int mon_conf_protocol_proc_read(char *page, char **start, off_t off, int count, int *eof,void *data);
int mon_conf_protocol_proc_write(struct file *file, const char __user *buffer, 
		unsigned long count, void *data);

void lookup_protocol(struct sk_buff *skb);
bool is_protocol_mon(void);

extern net_dev_stats_total_struct net_dev_stats_total;
extern int real_mon_thread(void *);
extern void protocol_mon_flag_enable(void);
extern void protocol_mon_flag_disable(void);
extern struct timer_list protocol_timeout;
extern int g_protocol_mon_flag;
