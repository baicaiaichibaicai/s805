int ids_mirror_proc_read(char *page, char **start,off_t off, int count, int *eof,void *data);
int ids_mirror_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
void send_rx_mirror(struct sk_buff *skb); 
void send_tx_mirror(struct sk_buff *skb); 

extern rwlock_t mirror_lock;

struct mirror_conf {
    char *action;
    char *org_iface; 
    char *mirror_iface; 
    char *part1;
    char *part2; 
};

struct mirror_count {
    __u64 org_rx;
    __u64 org_tx;
    __u64 mirror_rx;
    __u64 mirror_tx;
};
