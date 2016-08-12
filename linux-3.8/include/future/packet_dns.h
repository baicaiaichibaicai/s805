#ifndef __PACKET_DNS_H__
#define __PACKET_DNS_H__

#define CONFIG_FERRET_DNS_MANAGER

struct dnshdr {
	u_int16_t id;			/* query identification number */
	u_int16_t flags;		/* flags */
	u_int16_t qdcount;		/* number of question entries */
	u_int16_t ancount;		/* number of answer entries */
	u_int16_t nscount;		/* number of authority entries */
	u_int16_t arcount;		/* number of resource entries */
};

struct query_record {
	u_int8_t* name;
	u_int16_t* type;
	u_int16_t* class;
};

struct answer_record {
	u_int8_t* name;
	u_int16_t* type;
	u_int16_t* class;
	u_int32_t* time_to_live;
	u_int16_t* data_len;
	void* data;
};

struct dns_records {
	struct query_record* qd;
	struct answer_record* an;
	struct answer_record* ns;
	struct answer_record* ar;
};

struct dnshdr* dns_hdr(const struct sk_buff* skb);
#ifdef CONFIG_FERRET_DNS_MANAGER
void init_dns_records(struct dns_records* dr, struct query_record* qd, struct answer_record* an);
#else
void init_dns_records(struct dnshdr* dh, struct dns_records* dr);
void free_dns_records(struct dns_records* dr);
#endif
int get_dns_name(struct dnshdr *dh, u_int8_t* cursor, char* name, int size);
int dissect_dns_packet(struct dnshdr* dh, struct dns_records* dr);

#endif
