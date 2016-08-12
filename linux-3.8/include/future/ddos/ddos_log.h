#ifndef __DDOS_LOG_H__
#define __DDOS_LOG_H__

#include <future/log/logv2.h>

/* ddos attack define code */
// DoS DETECT
#define DETECT_DOS_SYN                     0x70103001  // DoS SYN flooding
#define DETECT_DOS_SYNACK                  0x70103002  // DoS SYN_ACK flooding
#define DETECT_DOS_ACK                     0x70103003  // DoS ACK flooding
#define DETECT_DOS_FIN                     0x70103004  // DoS FIN flooding
#define DETECT_DOS_RST                     0x70103005  // DoS RST flooding
#define DETECT_DOS_PUSH                    0x70103006  // DoS PUSH flooding
#define DETECT_DOS_URG                     0x70103007  // DoS URG flooding
#define DETECT_DOS_UDP                     0x70103008  // DoS UDP flooding
#define DETECT_DOS_ICMP                    0x70103009  // DoS ICMP flooding
#define DETECT_DOS_PING_SIZE_OVER          0x7010300a  // DoS Ping Size Over
#define DETECT_DOS_GET                     0x7010300b  // DoS GET flooding
#define DETECT_DOS_REQUEST                 0x7010300c  // DoS REQUEST flooding
#define DETECT_DOS_DNS_QUERY               0x7010300d  // DoS DNS Query flooding
#define DETECT_DOS_SQL_QUERY               0x7010300e  // DoS SQL Query flooding

// DDoS DETECT
#define DETECT_DDOS_SYN                    0x70203001  // DDoS SYN flooding
#define DETECT_DDOS_SYNACK                 0x70203002  // DDoS SYN_ACK flooding
#define DETECT_DDOS_ACK                    0x70203003  // DDoS ACK flooding 
#define DETECT_DDOS_FIN                    0x70203004  // DDoS FIN flooding
#define DETECT_DDOS_RST                    0x70203005  // DDoS RST flooding
#define DETECT_DDOS_PUSH                   0x70203006  // DDoS PUSH flooding
#define DETECT_DDOS_URG                    0x70203007  // DDoS URG flooding
#define DETECT_DDOS_UDP                    0x70203008  // DDoS UDP flooding
#define DETECT_DDOS_ICMP                   0x70203009  // DDoS ICMP flooding
#define DETECT_DDOS_PING_SIZE_OVER         0x7020300a  // DDoS Ping Size Over 
#define DETECT_DDOS_GET                    0x7020300b  // DDoS GET flooding
#define DETECT_DDOS_REQUEST                0x7020300c  // DDoS REQUEST flooding
#define DETECT_DDOS_DNS_QUERY              0x7020300d  // DDoS DNS Query flooding
#define DETECT_DDOS_SQL_QUERY              0x7020300e  // DDoS SQL Query flooding

// HTTP DETECT
#define DETECT_HTTP_CC                     0x70303001  // HTTP CC
#define DETECT_HTTP_AB_SMALL_PACKET        0x70303002  // HTTP Abnormal small packet
#define DETECT_HTTP_AB_LARGE_CONTENT       0x70303003  // HTTP Abnormal large content
#define DETECT_HTTP_AB_WINDOW              0x70303004  // HTTP Abnormal window
#define DETECT_HTTP_PARAMETER              0x70303005  // HTTP Abnormal parameter
#define DETECT_HTTP_AB_SESSION             0x70303006  // HTTP Incompleted

// DoS BLOCK
#define BLOCK_DOS_START_SYN                0x70103101  // DoS SYN flooding
#define BLOCK_DOS_START_SYNACK   	       0x70103102  // DoS SYN_ACK flooding
#define BLOCK_DOS_START_ACK                0x70103103  // DoS ACK flooding
#define BLOCK_DOS_START_FIN                0x70103104  // DoS FIN flooding             
#define BLOCK_DOS_START_RST                0x70103105  // DoS RST flooding
#define BLOCK_DOS_START_PUSH               0x70103106  // DoS PUSH flooding
#define BLOCK_DOS_START_URG                0x70103107  // DoS URG flooding
#define BLOCK_DOS_START_UDP                0x70103108  // DoS UDP flooding
#define BLOCK_DOS_START_ICMP               0x70103109  // DoS ICMP flooding
#define BLOCK_DOS_START_PING_SIZE_OVER     0x7010310a  // DoS Ping Size Over
#define BLOCK_DOS_START_GET                0x7010310b  // DoS GET flooding
#define BLOCK_DOS_START_REQUEST            0x7010310c  // DoS REQUEST flooding
#define BLOCK_DOS_START_DNS_QUERY          0x7010310d  // DoS DNS Query flooding
#define BLOCK_DOS_START_SQL_QUERY          0x7010310e  // DoS SQL Query flooding

// DDoS BlOCK START
#define BLOCK_DDOS_START_SYN               0x70203101  // DDoS SYN flooding 
#define BLOCK_DDOS_START_SYNACK            0x70203102  // DDoS SYN_ACK flooding
#define BLOCK_DDOS_START_ACK               0x70203103  // DDoS ACK flooding
#define BLOCK_DDOS_START_FIN               0x70203104  // DDoS FIN flooding
#define BLOCK_DDOS_START_RST               0x70203105  // DDoS RST flooding
#define BLOCK_DDOS_START_PUSH              0x70203106  // DDoS PUSH flooding
#define BLOCK_DDOS_START_URG               0x70203107  // DDoS URG flooding
#define BLOCK_DDOS_START_UDP               0x70203108  // DDoS UDP flooding
#define BLOCK_DDOS_START_ICMP              0x70203109  // DDoS ICMP flooding
#define BLOCK_DDOS_START_PING_SIZE_OVER    0x7020310a  // DDoS PING SIZE OVER 
#define BLOCK_DDOS_START_GET               0x7020310b  // DDoS GET flooding
#define BLOCK_DDOS_START_REQUEST           0x7020310c  // DDoS REQUEST flooding
#define BLOCK_DDOS_START_DNS_QUERY         0x7020310d  // DDoS DNS Query flooding
#define BLOCK_DDOS_START_SQL_QUERY         0x7020310e  // DDoS SQL Query flooding

// HTTP BLOCK START
#define BLOCK_HTTP_START_CC                0x70303101  // HTTP CC
#define BLOCK_HTTP_START_AB_SMALL_PACKET   0x70303102  // HTTP Abnormal small packet
#define BLOCK_HTTP_START_AB_LARGE_CONTENT  0x70303103  // HTTP Abnormal large content
#define BLOCK_HTTP_START_AB_WINDOW         0x70303104  // HTTP Abnormal window
#define BLOCK_HTTP_START_PARAMETER         0x70303105  // HTTP Abnormal parameter
#define BLOCK_HTTP_START_AB_SESSION        0x70303106  // HTTP Incomplete

int put_ddos_log(log_param_struct *log_param, void *packet, char *desc, u_int attack_type, int action, u_int block_time, u_int sec_t);
char *get_dos_attack_type_str(int type, u_int *dos_event);
char *get_ddos_attack_type_str(int type, u_int *ddos_event);
char *get_http_attack_type_str(int type, u_int *http_event);
int get_ddos_event_code(int action, int type);

#endif // DDOS_LOG_H
