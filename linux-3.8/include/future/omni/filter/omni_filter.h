#ifndef _OMNI_FILTER_H
#define _OMNI_FILTER_H

enum omni_inet_hooks {
	OMNI_INET_PRE_ROUTING,
	OMNI_INET_LOCAL_IN,
	OMNI_INET_FORWARD,
	OMNI_INET_LOCAL_OUT,
	OMNI_INET_POST_ROUTING,
	OMNI_INET_NUMHOOKS
};

enum {
	OMNI_PROTO_UNSPEC =  0,
	OMNI_PROTO_IPV4   =  2,
	OMNI_PROTO_IPV6   = 10,
	OMNI_PROTO_NUMPROTO,
};

/* omni-filter header */
struct omni_filter_header {
	u_int32_t timeout;
	u_int32_t log;
	u_int32_t rule_no;
	u_int32_t unique_id;
	u_int16_t vlan;
	u_int8_t bidir:1,
		 dscp:6;
	u_int8_t cos;
};

/* omni-filter resource */
struct omni_filter_resource {
	u_int32_t idx_limit;
	u_int32_t idx_qos;
};

/* omni-filter profile */
struct omni_filter_profile {
	u_int64_t idx_ddos;
	u_int64_t idx_ips;
	u_int32_t idx_app;
	u_int32_t idx_http;
	u_int32_t idx_av;
	u_int32_t idx_as;
	u_int32_t idx_rpc;
	u_int32_t idx_sslInspect;
};

/* omni-filter info */
struct omni_filter {
	struct omni_filter_header header;
	struct omni_filter_resource resource;
	struct omni_filter_profile profile;
};

struct omni_nat_header {
	u_int32_t rule_no;
	u_int32_t unique_id;
};

/* omni-nat info */
struct omni_nat {
	struct omni_nat_header header;
};

enum omni_cache_status {
	OMNI_CACHE_NONE_BIT = 0,
	OMNI_CACHE_NONE = (1 << OMNI_CACHE_NONE_BIT),

	OMNI_CACHE_REEVAL_BIT = 1,
	OMNI_CACHE_REEVAL = (1 << OMNI_CACHE_REEVAL_BIT),

	OMNI_CACHE_CACHED_BIT = 2,
	OMNI_CACHE_CACHED = (1 << OMNI_CACHE_CACHED_BIT),

	OMNI_CACHE_URL_LOG_CACHED_BIT = 3,
	OMNI_CACHE_URL_LOG_CACHED = (1 << OMNI_CACHE_URL_LOG_CACHED_BIT),
};

struct omni_block {
	unsigned int type;
};
#endif
