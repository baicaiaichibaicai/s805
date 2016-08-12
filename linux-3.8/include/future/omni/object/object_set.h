#ifndef _OBJ_SET_H_
#define _OBJ_SET_H_

#include <linux/types.h>
#include <future/omni/omni_track_tuple_common.h>

#define OBJ_SET_ID 	5674

#define OM_V4_IP_SINGLE  0x01
#define OM_V4_IP_RANGE   0x02
#define OM_V4_IP_NETMASK 0x03
#define OM_V4_IP_PREFIX  0x04
#define OM_OBJ_GROUP 	 0x05
#define OM_V6_IP_SINGLE  0x11
#define OM_V6_IP_PREFIX  0x12
#define OM_SERVICE		 0x21
#define OM_PROTOCOL		 0x22
#define OM_ICMP			 0x23
#define OM_ICMPV6		 0x24
#define OM_COUNTRY_CODE	 0x25
#define OM_DOMAIN	     0x26
#define OM_ENV	         0x27
#define OM_SCHEDULE      0x31
#define OM_QOS           0x42
#define OM_IPV6HEADER    0x52
#define OM_LIMIT		 0x61
#define OM_USER          0x71

#define OM_MAP_TYPE_NONE	  0x00
#define OM_MAP_TYPE_MULTI_KEY 0x45
#define OM_MAP_TYPE_SORTED 	  0x78

#define MAX_DATE_STARTSTOPS 5
#define MAX_TIME_STARTSTOPS 5

#define MAX_BUF_DOMAIN  256
#define MAX_BUF_ENV     16
#define MAX_ENV_CNT     42
#define MAX_BUF_USER_ID 32

#define ENTRY_TABLE_COUNT 16

enum {
	OM_TABLE_IP         = 0,
	OM_TABLE_IP_G       = 1,
	OM_TABLE_IPV6       = 2,
	OM_TABLE_IPV6_G     = 3,
	OM_TABLE_SVC        = 4,
	OM_TABLE_SVC_G      = 5,
	OM_TABLE_USER       = 6,
	OM_TABLE_USER_G     = 7,
	OM_TABLE_DOMAIN     = 8,
	OM_TABLE_CC         = 9,
	OM_TABLE_ENV        = 10,
	OM_TABLE_SCH        = 11,
	OM_TABLE_RES        = 12,
	OM_TABLE_ARP        = 13,
	OM_TABLE_QOS        = 14,
	OM_TABLE_IPV6HEADER = 15,
	OM_TABLE_ALL        = 16,
};

#pragma pack(push, 8)

typedef struct _om_stat {
	time_t last_hit;
} om_stat;

typedef struct _om_object {
	__u8 type;
	void *obj;
} om_object;

typedef struct _omni_om_entry {
	__u16 count;
	__u16 icmp_count;
	__u16 icmpv6_count;

	//om_object **map;
	__u16 map_type;
	void *map;

	__u32 length;
	om_object *data;

	om_stat   stat;
} omni_om_entry;

typedef struct _omni_om_entry_table {
	omni_om_entry **array;
	int count;
	__u8 memkeep;
} omni_om_entry_table;

typedef struct _om_ip_single {
	__u32 ip;
} om_ip_single;

typedef struct _om_ip_range {
	__u32 start;
	__u32 end;
} om_ip_range;

typedef struct _om_ip_netmask {
	__u32 network;
	__u32 mask;
} om_ip_netmask;

typedef struct _om_ip_prefix {
	__u32 network;
	__u8 prefix;
} om_ip_prefix;

typedef struct _om_obj_group {
	__u32 index;
} om_obj_group;

typedef struct _om_ip_service {
	unsigned int timeout;

	__u16 src_start;
	__u16 src_end;

	__u16 dst_start;
	__u16 dst_end;

	__u8 protocol;
} om_ip_service;

typedef struct _om_ipv6_single {
	__u32 ipv6[4];
} om_ipv6_single;

typedef struct _om_ipv6_prefix {
	__u32 network[4];
	__u8 prefix;
} om_ipv6_prefix;

typedef struct _om_obj_schedule {
    __u8 wdays;                             /* 1 bit per a day of the week. -SMTWTFS */
	__u32 mdays;                            /* 1 bit per day. */
	__u16 months;                           /* 1 bit per month. */
	__u16 time_start[MAX_TIME_STARTSTOPS];  /* 0 < time_start < 23*60+59 = 1439 */
	__u16 time_stop[MAX_TIME_STARTSTOPS];   /* 0:0 < time_stat < 23:59 */
	__u16 n_time;                           /* Number of time setting */
	__u8 kerneltime;                        /* ignore skb time (and use kerneltime) or not. */
    __u32 date_start[MAX_TIME_STARTSTOPS];
    __u32 date_stop[MAX_TIME_STARTSTOPS];
	__u16 n_date;                           /* Number of date setting */
} om_obj_schedule;

typedef struct _om_qos {
 	unsigned long iface;
 	__u32 id;
} om_qos;

typedef struct _om_ipv6header {
	__u8 matchflags;
	__u8 invflags;
	__u8 modeflag;
} om_ipv6header;

typedef struct _om_ip_icmp {
	unsigned int timeout;
	__u8 type;
	__u8 code;
}om_ip_icmp;

typedef struct _om_cc {
	__u16 cc;
}om_cc;

typedef struct _om_obj_domain {
    char domain[MAX_BUF_DOMAIN];
} om_obj_domain;

typedef struct _om_obj_env {
    char env[MAX_BUF_ENV];
} om_obj_env;

typedef struct _om_cc_data {
	__u32 begin;
	__u32 end;
	__u16 cc;
}__attribute__ ((packed)) om_cc_data;

typedef struct _om_obj_limit {
	__u32 traffic_limit[OMNI_TRACK_DIR_MAX];
	__u32 session_limit;
	__u32 user_limit;

	__u8 type;
} om_obj_limit;

typedef struct _om_obj_user {
    char userid[MAX_BUF_USER_ID];
} om_obj_user;

#pragma pack(pop)

#define OM_CMD_ADD		 		0x71
#define OM_CMD_DEL		 		0x72
#define OM_CMD_REPLACE   		0x73
#define OM_CMD_BATCH	 		0x74
#define OM_CMD_STATUS	 		0x75
#define OM_CMD_LIST		 		0x76
#define OM_CMD_FLUSH	 		0x78
#define OM_CMD_ENTRIES_LENGTH	0x79
#define OM_CMD_REFER	 		0x80

/* command */
typedef struct _omni_om_cmd_root {
	__u8  cmd_type;
	void *cmd;
} omni_om_cmd_root;

typedef struct _omni_om_cmd_add {
	__u8  table_index;

	__u32 index;
	omni_om_entry *entry;
} omni_om_cmd_add;

typedef struct _omni_om_cmd_del {
	__u8  table_index;

	__u32 index;
} omni_om_cmd_del;

typedef struct _omni_om_cmd_replace {
	__u8  table_index;

	__u32 index;
	omni_om_entry *entry;
} omni_om_cmd_replace;

typedef struct _omni_om_cmd_batch {
	__u8 table_index;

	__u32 count[ENTRY_TABLE_COUNT];
	__u32 *indexes[ENTRY_TABLE_COUNT];
	omni_om_entry **entries[ENTRY_TABLE_COUNT];
} omni_om_cmd_batch;

typedef struct _omni_om_cmd_status {
	__u8  table_index;

	__u8 *valid[ENTRY_TABLE_COUNT];
	om_stat *stat[ENTRY_TABLE_COUNT];
} omni_om_cmd_status;

/*20160305@reference command */
typedef struct _omni_cmd_refer_status {
	__u8  table_index;
	unsigned int size_by_valid;
	unsigned int offset_by_valid[ENTRY_TABLE_COUNT];
	unsigned int offset_by_time[ENTRY_TABLE_COUNT];
	unsigned char stat[0];
} omni_cmd_refer_status;


typedef struct _omni_om_cmd_list {
	__u8  table_index;

	__u32 *length[ENTRY_TABLE_COUNT];
	om_object **data[ENTRY_TABLE_COUNT];

} omni_om_cmd_list;

typedef struct _omni_om_cmd_entries_length {
	__u8  table_index;

	__u32 *length[ENTRY_TABLE_COUNT];
} omni_om_cmd_entries_length;

typedef struct _omni_om_cmd_flush {
	__u8  table_index;
} omni_om_cmd_flush;

typedef struct _five_tuple {
	__u32 src_ip;
	__u32 dst_ip;
	__u16 src_port;
	__u16 dst_port;
	__u8  protocol;
} five_tuple;

#endif

