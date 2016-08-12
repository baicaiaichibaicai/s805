int white_black_proc_read(char *page, char **start,off_t off, int count, int *eof,void *data);
int white_black_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
int create_white_black_ip_list(void);
int wb_lookup_ip4(struct sk_buff *skb);
int wb_lookup(struct sk_buff *skb);
void wb_set_skip_path(struct omni_track *track);

extern void read_white_ipv4_list_command(struct vty* vty);
extern void read_black_ipv4_list_command(struct vty* vty);

extern rwlock_t wb_ip4_lock;
extern int white_black;

#define WB_WHITE 1
#define WB_BLACK 2

#define WB_NO_MATCH 0
#define WB_WHITE_MATCH 1
#define WB_BLACK_MATCH 2

