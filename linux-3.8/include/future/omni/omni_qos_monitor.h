extern int g_qos_monitor;

extern int mon_fw_qos_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
extern int mon_fw_qos_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data);
extern void qdisc_monitor_update(struct Qdisc *sch, const struct sk_buff *skb);
