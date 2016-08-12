#ifndef _OBJ_OPERATION_H_
#define _OBJ_OPERATION_H_

#include <future/omni/object/object_set.h>
#include <future/omni/filter/omni_ip_tables.h>
#include <future/omni/filter/omni_ip6_tables.h>
#include <future/omni/netfilter/omni_x_tables.h>
//#include <asm/types.h>

#ifdef CONFIG_FERRET_ERFC_LOOKUP
#include <future/erfc/erfc.h>
#endif

#define OMNI_OBJ_NOT_FOUND	-1
#define DOMAIN_OBJ_FOUND    1
#define DOMAIN_OBJ_NOT_FOUND    0

int do_add(const omni_om_cmd_add *cmd);
int do_del(const omni_om_cmd_del *cmd);
int do_replace(const omni_om_cmd_replace *cmd);
int do_flush(const omni_om_cmd_flush *cmd);
int do_batch(const omni_om_cmd_batch *cmd);

int do_status(omni_om_cmd_status *cmd);
int do_refer(omni_cmd_refer_status *cmd);
int do_list(omni_om_cmd_list *cmd);
int get_entries_length(omni_om_cmd_entries_length *cmd);
omni_om_entry *_check_correct_entry(const omni_om_entry_table *om_entry_table, const int index);
omni_om_entry_table *omni_find_obj_entry_table(const int index);

int object_match_ipv4(const int table_index, const int index, const __u32 ipv4);
int object_match_ipv6(const int table_index, const int index, const __u32 *ipv6);
int object_match_service(const int table_index, const int index, const __u16 src_port, const __u16 dst_port, const __u8 protocol, unsigned int *timeout);
int object_match_schedule(const int table_index, const int index);
int object_match_domain(const int table_index, const int index, const __u32 ipv4);
int object_match_env_ipv4(const int table_index, const int index, const __u32 ipv4);
int object_match_env_ipv6(const int table_index, const int index, const __u32 *ipv6);
int object_match_icmp(const int table_index, const int index, const __u8 type, const __u8 code, unsigned int *timeout);
int object_match_icmpv6(const int table_index, const int index, const __u8 type, const __u8 code, unsigned int *timeout);
void refresh_match_time(const int table_index, const int index);
int object_match_limit(const int table_index, const int index, const struct omni_track_tuple *t, const int len, struct sk_buff *skb, struct omni_ipt_entry *e);
int object_match_user(const int table_index, const int index, const __u32 ipv4);

int match_v4(const int, const int, const __u32);
int get_cache_qos(const unsigned int index, struct sk_buff *skb);

int object_match_ipv4_group(const int table_index, const int index, const __u32 ipv4, int *mmbr_idx);
int object_match_ipv6_group(const int table_index, const int index, const __u32 *ipv6, int *mmbr_idx);
int object_match_service_group(const int table_index, const int index, const __u16 src_port, const __u16 dst_port, const __u8 protocol, unsigned int *timeout, int *mmbr_idx);
int object_match_icmp_group(const int table_index, const int index, const __u8 type, const __u8 code, unsigned int *timeout, int *mmbr_idx);
int object_match_icmpv6_group(const int table_index, const int index, const __u8 type, const __u8 code, unsigned int *timeout, int *mmbr_idx);
int object_match_user_group(const int table_index, const int index, const __u32 ipv4, int *mmbr_idx);
om_ipv6header *object_match_ipv6header(const int table_index, const int index);
int object_match_cc(const int table_index, const int index, const unsigned char type, const __u16 src_cc, const __u16 dst_cc);
__u16 binary_find_cc(const __u32 ipv4);
int find_domain_obj(const char *domain);

int init_entry_array(void);
void release_entry_array(void);

extern void object_update_lasthit(int table, unsigned int obj_idx);
static inline void object_matched_index_set(int table, unsigned int index,
		struct omni_xt_action_param *acpar)
{
	acpar->matched_obj[table] = index;
}

static inline void object_matched_timeout_set(unsigned int timeout,
		struct omni_xt_action_param *acpar)
{
	acpar->timeout = timeout;
}
#ifdef CONFIG_FERRET_ERFC_LOOKUP
//added by jasonnguyen
int get_size_obj_table(const int table_index);

int object_set_bitmap_proto_service(const int table_index, const int index, BITMAP*bitmap,int cbm_ith, int jt_index);
int object_set_bitmap_proto_service_group(const int table_index, const int index, BITMAP*bitmap, int cbm_ith, int jt_index);

int object_set_bitmap_srcip(const int table_index, const int index, BITMAP**bitmap, int cbm_ith, int jt_index, int cache);
int object_set_bitmap_srcip_group(const int table_index, const int index, BITMAP**bitmap, int cbm_ith, int jt_index, int cache);

int object_set_bitmap_dstip(const int table_index, const int index, BITMAP**bitmap, int cbm_ith, int jt_index, int cache);
int object_set_bitmap_dstip_group(const int table_index, const int index, BITMAP**bitmap, int cbm_ith, int jt_index, int cache);

int object_set_bitmap_srcport(const int table_index, const int index, BITMAP*bitmap, int cbm_ith, int jt_index, int list_index);
int object_set_bitmap_srcport_group(const int table_index, const int index, BITMAP*bitmap, int cbm_ith, int jt_index, int list_index);

int object_set_bitmap_dstport(const int table_index, const int index, BITMAP*bitmap, int cbm_ith, int jt_index, int list_index);
int object_set_bitmap_dstport_group(const int table_index, const int index, BITMAP*bitmap, int cbm_ith, int jt_index, int list_index);
#endif

#ifdef CONFIG_FERRET_HIPAC
#include <future/nf_hipac/hipac.h>

int hipac_set_srcip(const int index, struct hipac_match*match, int dimid, int *count);
int hipac_set_srcip_group(const int index, struct hipac_match*match, int dimid, int *count);

int hipac_set_dstip(const int index, struct hipac_match*match, int dimid, int *count);
int hipac_set_dstip_group(const int index, struct hipac_match*match, int dimid, int *count);

int hipac_set_service(const int index, struct hipac_match*match, int dimid, int *count);
int hipac_set_service_group(const int index, struct hipac_match*match, int dimid, int *count);

int hipac_set_srcport(const int index, struct hipac_match*match, int dimid, int *count);
int hipac_set_srcport_group(const int index, struct hipac_match*match, int dimid, int *count);

int hipac_set_dstport(const int index, struct hipac_match*match, int dimid, int *count);
int hipac_set_dstport_group(const int index, struct hipac_match*match, int dimid, int *count);

#endif


#endif
