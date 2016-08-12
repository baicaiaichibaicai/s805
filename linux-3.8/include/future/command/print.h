/* /include/future/command/print.h
 *
 * (C) 2013 by suyoon <songung@future.co.kr>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
 */

#ifndef _COMMAND_PRINT_H
#define _COMMAND_PRINT_H

#include <future/command/vty.h>

// debug level
#define DL_CON			0x00000000	// console-only
#define DL_SLOG			0x00000001	// system loga					's'
#define DL_PF			0x00000002	// packet filtering				'p'
#define DL_IPSEC		0x00000004	// IPSec detail					'i'
#define DL_MA			0x00000008	// message agent				'm'
#define DL_CF			0x00000010	// content filtering			'c'
#define DL_CT			0x00000020	// conntrack debug				't'
//#define DL_DEV		0x00000040	// Device Driver
#define DL_NET			0x00000080	// Network						'n'
#define DL_QOS			0x00000100	// Traffic Control				'q'
#define DL_IKE_LOG		0x00000200	// IKE logfile					'k'
#define DL_ROUTE		0x00000400
#define DL_RAUSER		0x00000800	// RAVPN						'u'
#define DL_PKI			0x00001000	// PKI							'k'
#define DL_IPMAC		0x00002000	// IP&MAC mngt debug			'd'
#define DL_USER			0x00004000	// User Awareness				'r'
#define DL_DOS			0x00008000	// DoS Attack					'o'
#define DL_SYSLOG		0x00010000	// Syslog						'l'
//#define DL_CRDEV		0x00020000	// Crypto Device
#define DL_HA			0x00040000	// HA							'b'
#define DL_GRP			0x00080000	// object group (ip, service)	'g'
#define DL_ETC			0x00100000	// etc.. debug					'e' 
#define DL_IPS			0x00200000	// IPS							'h'
#define DL_APP			0x00400000	// Application Awareness		'y'
#define DL_IPSEC2		0x00800000	// IPSec						'a'
#define DL_OMNI			0x01000000	// OMNI-Path					'z'
#define DL_NAT64		0x02000000  // NAT64						'j'
#define DL_OM_ERR		0x04000000  // SYSTEM ERROR					'w'

// others
#define DL_SYNC_TRACK	0x04000000	// debug sync session
#define DL_CAPTURE		0x10000000	// capture ipv4 command (internal use)
#define DL_MESSAGE		0x20000000	// pause/resume debugging commmand (internal use)
#define DL_CAPTURE6		0x40000000	// capture ipv6 command (internal use)

#define DEFAULT_DL		DL_SLOG
#define MAX_DL			(8 * 4) + 1	// + 1 is for DL_CON

// nodify level
#define NL_DEBUG		0x00000001
#define NL_INFO			0x00000002
#define NL_WARN			0x00000004
#define NL_ERROR		0x00000008

// color string
#define C_CENTER_STR "\033[43;30m"
#define C_DROP_STR   "\033[43;30m"
#define C_ESP_STR    "\033[43;30m"
#define C_ICMP_STR   "\033[43;30m"
#define C_VRRP_STR   "\033[43;30m"
#define C_SPD_STR    "\033[43;30m"
#define C_NAT_STR    "\033[43;30m"
#define C_IP_STR     "\033[43;30m"
#define C_IP6_STR    "\033[43;30m"
#define C_PORT_STR   "\033[43;30m"
#define C_IFACE_STR  "\033[43;30m"
#define C_END        "\033[m"

// color string length
#define C_CENTER_STR_LEN 64
#define C_DROP_STR_LEN   32
#define C_ESP_STR_LEN    32
#define C_ICMP_STR_LEN   32
#define C_VRRP_STR_LEN   32
#define C_SPD_STR_LEN    32
#define C_NAT_STR_LEN    32
#define C_IP_STR_LEN     64
#define C_IP6_STR_LEN    64
#define C_PORT_STR_LEN   32
#define C_IFACE_STR_LEN  64

int _debug(unsigned int lev, const char *fmt, ...);
int __debug(struct vty *vty, const char *fmt, ...);

void reset_common_dl(void);
void reset_dl(struct vty *);


#define capture(lev, fmt, a...) \
		do { extern unsigned int common_dl; \
			if ((common_dl & DL_CAPTURE) && (common_dl & lev)) \
				_debug(lev, fmt, ##a); } while(0)

#undef debug
#define debug(lev, fmt, a...) \
		do { extern unsigned int common_dl; \
			if (common_dl & lev) _debug(lev, fmt, ##a); } while(0)

#define test_dl(vty, lev) (vty->debug_level & lev)

#define set_dl(vty, lev) \
		do { extern unsigned int common_dl; \
			vty->debug_level |= lev; \
			common_dl |= lev; } while(0)

#define clear_dl(vty, lev) \
		do { vty->debug_level &= ~lev; \
			reset_common_dl(); } while(0)

#define toggle_dl(vty, lev) \
		do { if (test_dl(vty, lev)) clear_dl(vty, lev); \
			else set_dl(vty, lev); } while(0)

void __set_ipstr_color(struct vty *vty, __be32 ip, char *ipstr);
void __set_portstr_color(struct vty *vty, __be16 port, char *portstr);
void __set_drop_color(struct vty *vty, char *str, unsigned int block_type);
void __set_ifname_color(struct vty *vty, struct net_device *dev, char *ifstr);
int check_d_fwd(int d_fwd, int d_fwd2, struct capture_info *ci);

int notify(unsigned int lev, const char *fmt, ...);
void debug_hex_dump(unsigned int, char *, size_t);

#endif /*_COMMAND_PRINT_H*/

