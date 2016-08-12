#define MAX_EXCEPT_IP 16

struct ipv4_info {
	unsigned int ip1;
	unsigned int ip2;
	char format;
};

struct pan_conf {
	int start_time;
	int end_time;
	int hold_time;
	char use;
	char protocol;
	struct ipv4_info detect_except[MAX_EXCEPT_IP];
	struct ipv4_info  protect_except[MAX_EXCEPT_IP];
};

enum pan_conf_type {
	PAN_USE,
	PAN_START_TIME,
	PAN_END_TIME,
	PAN_DETECT_EXCEPT,
	PAN_PROTECT_EXCEPT,
	PAN_PROTOCOL,
	PAN_HOLD_TIME,
	PAN_CONF_MAX
};

enum pan_except_type {
	PAN_DETECT,
	PAN_PROTECT
};

enum pan_time_type {
	PAN_START,
	PAN_END
};

enum pan_ip_format {
	PAN_SINGLE_IP = 1,
	PAN_RANGE_IP,
	PAN_PREFIX_IP
};

enum pan_protocol {
	PAN_PROTOCOL_TCP_BIT = 0,
	PAN_PROTOCOL_TCP = (1 << PAN_PROTOCOL_TCP_BIT),

	PAN_PROTOCOL_UDP_BIT = 1,
	PAN_PROTOCOL_UDP = (1 << PAN_PROTOCOL_UDP_BIT),

	PAN_PROTOCOL_ICMP_BIT = 2,
	PAN_PROTOCOL_ICMP = (1 << PAN_PROTOCOL_ICMP_BIT)
};

extern int fw_pan_proc_read(char *page, char **start, off_t off, int count,
		int *eof, void *data);
extern int fw_pan_proc_write(struct file *file, const char __user *buffer,
		unsigned long count, void *data);
extern void pan_use_disable(void);
extern void pan_use_ensable(void);
extern bool pan_block_lookup(struct sk_buff *skb, struct net_device *in, struct net_device *out);
extern void pan_block_regist(struct sk_buff *skb, struct net_device *in, struct net_device *out);

extern rwlock_t pan_ip4_lock;
