extern int white_black_ip6;
int white_black_ip6_proc_read(char *page, char **start,off_t off, int count, int *eof,void *data);
int white_black_ip6_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
int create_white_black_ip6_list(void);
int wb_lookup_ip6(struct sk_buff *skb);

extern void read_white_ipv6_list_command(struct vty* vty);
extern void read_black_ipv6_list_command(struct vty* vty);

extern rwlock_t wb_ip6_lock;

#define WB_WHITE 1
#define WB_BLACK 2

#define WB_NO_MATCH 0
#define WB_WHITE_MATCH 1
#define WB_BLACK_MATCH 2

