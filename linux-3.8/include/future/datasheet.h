#ifndef __DATASHEET_H__
#define __DATASHEET_H__


int get_root_manager_cnt(void);
int get_sub_manager_cnt(void);
int get_bridge_iface_cnt(void);
int get_bonding_iface_cnt(void);
int get_ipv4_vip_cnt(void);
int get_ipv6_vip_cnt(void);
int get_vlan_cnt(void);
int get_vlan_policy(void);
int get_dhcp_server_cnt(void);
int get_ipv6_tunneling_cnt(void);
int get_static_routing_cnt(void);
int get_policy_routing_cnt(void);
int get_vrrp_cnt(void);   
int get_checker_cnt(void); 
int get_ip_obj_cnt(void); 
int get_ip_obj_member_cnt(void); 
int get_ip_grp_obj_cnt(void);   
int get_ip_grp_obj_member_cnt(void);
int get_ipv6_obj_cnt(void); 
int get_ipv6_obj_member_cnt(void); 
int get_ipv6_grp_obj_cnt(void);   
int get_ipv6_grp_obj_member_cnt(void);
int get_ipv6_header_cnt(void);     
int get_service_obj_cnt(void);    
int get_service_obj_member_cnt(void);
int get_service_grp_obj_cnt(void); 
int get_service_grp_obj_member_cnt(void);
int get_time_obj_cnt(void);             
int get_qos_obj_cnt(void);             
int get_session_limit_obj_cnt(void);  
int get_user_obj_cnt(void);
int get_user_import_cnt(void);
int get_user_obj_grp_cnt(void);
int get_user_obj_grp_member_cnt(void);
int get_ua_policy_cnt(void);
int get_ua_policy_src_cnt(void);
int get_ua_server_cnt(void);
int get_ua_user_mmgr_cnt(void);
int get_white_black_ip_cnt(void);  
int get_spd_rule_line_cnt(void);  
int get_et_entries_square_cnt(void); 
int get_cv(void);                 
int get_nat_rule_line_cnt(void); 
int get_session_cnt(void);      
int get_vpn_tunnel_cnt(void);  
int get_sechost_cnt(void);    
int get_ipsecsa_proposal_cnt(void); 
int get_isakmpsa_proposal_cnt(void);
int get_ipsec_gatelist_cnt(void);  
int get_access_server_cnt(void);   
int get_access_server_grp_cnt(void);
int get_ssl_user_cnt(void);
int get_log_per_sec_cnt(void);     
int get_log_per_logtable_cnt(void);
int get_log_numbering_cnt(void);   
int get_max_kernel_logfile_cnt(void); 
int get_etc_webadmin_size(void);     
int get_rpm_pool_size(void);        
int get_ips_group_max(void);	
int get_ips_signature_max(void);
int get_ddos_usr_cnt(void);
int get_app_prof_elem_cnt(void);
int get_anomaly_dos_table_size(void);
int get_anomaly_ddos_table_size(void);
int get_anomaly_normal_user_table_size(void);
int get_app_user_def_cnt(void);
int get_ipv4_country_cnt(void);
int get_ipv4_country_member_cnt(void);
int get_domain_obj_cnt(void);
int get_domain_obj_member_cnt(void);
int get_arp_cnt(void);
int get_multipath_iface_cnt(void);
int get_nat64_rule_line_cnt(void);
int get_pat_ip_cnt(void);

int get_app_profile_cnt(void);
int get_http_profile_cnt(void);       
int get_rpc_profile_cnt(void);         
int get_ips_profile_cnt(void);	
int get_ddos_profile_cnt(void);	

void set_ips_profile_cnt(int value);			// 프로파일 최대 개수 set
void set_ddos_profile_cnt(int value);			// ddos profile 개수 set

void set_session_cnt(int value);
void set_spd_rule_line_cnt(int value);
void set_nat_rule_line_cnt(int value);
void set_vpn_tunnel_cnt(int value);

void set_ips_group_max(int value);           	// 그룹 최대 개수 set
void set_ips_signature_max(int value);       	// 시그너처 최대 개수 set
void set_ddos_usr_cnt(int value);         		// 생성 가능한 user_table 개수 set


void datasheet_init(void);

void set_64GB_datasheet(void);
void set_48GB_datasheet(void);
void set_32GB_datasheet(void);
void set_24GB_datasheet(void);
void set_16GB_datasheet(void);
void set_12GB_datasheet(void);
void set_8GB_datasheet(void);
void set_4GB_datasheet(void);
void set_2GB_datasheet(void);
void set_VPN_4GB_datasheet(void);
void set_VPN_2GB_datasheet(void);
void set_VPN_1GB_datasheet(void);
void set_VPN_512MB_datasheet(void);
//void set_default_datasheet(void);




#endif

