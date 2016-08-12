#ifndef _AUTH_POLICY_MATCH_H_
#define _AUTH_POLICY_MATCH_H_

#include <linux/skbuff.h>

enum auth_policy_match {
    AP_NOT_MATCH = 0,
    AP_MATCH
};

struct rst_tbl {
    struct dst_entry *dst;
    __be32 sip;
    __be32 dip;
    __be32 seq;
    __be32 ack_seq;
    __be32 ack;
    __be16 sport;
    __be16 dport;
};

#define TCP_FLAG_FIN	0x01
#define TCP_FLAG_SYN    0x02
#define TCP_FLAG_RST	0x04
#define TCP_FLAG_PSH	0x08
#define TCP_FLAG_ACK    0x10
#define TCP_FLAG_SYNACK 0x12

void syn_proxy(struct sk_buff *skb);
void auth_policy_tcp_synack(struct sk_buff *oldskb, int hook);
int auth_policy_do_match(struct sk_buff *skb);
char *find_field(char *str, char *name, int len, char *end);
int get_org_host(char *dst_str, const char *data, const int len);
int get_org_uri(char *dst_uri_query, const char *data, const int len);
int send_redirection_page(struct sk_buff *skb, struct net_device *in);
int auth_policy_redirect(struct sk_buff *skb, struct net_device *in);
void auth_policy_tcp_reset(struct sk_buff *oldskb, int hook);
void auth_policy_tcp_fin_data(struct sk_buff *oldskb, int hook, char *data, u_int16_t data_len);
void init_rst_tbl(void);
void set_rst_tbl(struct sk_buff *oldskb);
void run_rst_tbl(const struct iphdr *iph, const struct tcphdr *th);
int check_redir_tuple(struct rst_tbl *rst, const struct iphdr *iph, const struct tcphdr *th);
void force_tcp_reset(struct rst_tbl *rst);

#endif
