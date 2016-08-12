#ifndef __IP_SPOOFING_H__
#define __IP_SPOOFING_H__

enum
{
	IP_SPOOFING_PASS,
	IP_SPOOFING_DROP
};

int ip_spoofing_proc_init(struct proc_dir_entry *p_proc_entry);
int check_ip_spoofing(struct sk_buff *skb);
int ipv4_spoofing_core(struct sk_buff *skb);
int ipv6_spoofing_core(struct sk_buff *skb);
int put_ips_spoofing_log(log_param_struct *log_param, struct sk_buff *skb);

#endif
