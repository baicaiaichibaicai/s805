/*******************************************************************************

  
  Copyright(c) 1999 - 2006 Intel Corporation. All rights reserved.
  
  This program is free software; you can redistribute it and/or modify it 
  under the terms of the GNU General Public License as published by the Free 
  Software Foundation; either version 2 of the License, or (at your option) 
  any later version.
  
  This program is distributed in the hope that it will be useful, but WITHOUT 
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
  more details.
  
  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59 
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  
  The full GNU General Public License is included in this distribution in the
  file called LICENSE.
  
  Contact Information:
  Linux NICS <linux.nics@intel.com>
  bp_addressing-devel Mailing List <bp_addressing-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

*******************************************************************************/

/* bp_addressing_hw.h
 * Structures, enums, and macros for the MAC
 */

#ifndef _BP_ADDRESSING_HW_H_
#define _BP_ADDRESSING_HW_H_

#include "addressing_osdep.h"


/* Forward declarations of structures used by the shared code */
struct bp_addressing_hw;
struct bp_addressing_hw_stats;

/* Enumerated types specific to the bp_addressing hardware */
/* Media Access Controlers */
typedef enum {
    bp_addressing_undefined = 0,
    bp_addressing_82542_rev2_1,
    bp_addressing_82543,
    bp_addressing_82544,
    bp_addressing_82540,
    bp_addressing_82545,
    bp_addressing_82545_rev_3,
    bp_addressing_82546,
    bp_addressing_82546_rev_3,
    bp_addressing_82541,
    bp_addressing_82541_rev_2,
    bp_addressing_82547,
    bp_addressing_82547_rev_2,
    bp_addressing_82571,
    bp_addressing_82572,
    bp_addressing_82573,
    bp_addressing_80003es2lan,
    bp_addressing_ich8lan,
    bp_addressing_num_macs
} bp_addressing_mac_type;

typedef enum {
    bp_addressing_eeprom_uninitialized = 0,
    bp_addressing_eeprom_spi,
    bp_addressing_eeprom_microwire,
    bp_addressing_eeprom_flash,
    bp_addressing_eeprom_ich8,
    bp_addressing_eeprom_none, /* No NVM support */
    bp_addressing_num_eeprom_types
} bp_addressing_eeprom_type;

/* Media Types */
typedef enum {
    bp_addressing_media_type_copper = 0,
    bp_addressing_media_type_fiber = 1,
    bp_addressing_media_type_internal_serdes = 2,
    bp_addressing_num_media_types
} bp_addressing_media_type;

typedef enum {
    bp_addressing_10_half = 0,
    bp_addressing_10_full = 1,
    bp_addressing_100_half = 2,
    bp_addressing_100_full = 3
} bp_addressing_speed_duplex_type;

/* Flow Control Settings */
typedef enum {
    bp_addressing_fc_none = 0,
    bp_addressing_fc_rx_pause = 1,
    bp_addressing_fc_tx_pause = 2,
    bp_addressing_fc_full = 3,
    bp_addressing_fc_default = 0xFF
} bp_addressing_fc_type;

struct bp_addressing_shadow_ram {
    uint16_t    eeprom_word;
    boolean_t   modified;
};

/* PCI bus types */
typedef enum {
    bp_addressing_bus_type_unknown = 0,
    bp_addressing_bus_type_pci,
    bp_addressing_bus_type_pcix,
    bp_addressing_bus_type_pci_express,
    bp_addressing_bus_type_reserved
} bp_addressing_bus_type;

/* PCI bus speeds */
typedef enum {
    bp_addressing_bus_speed_unknown = 0,
    bp_addressing_bus_speed_33,
    bp_addressing_bus_speed_66,
    bp_addressing_bus_speed_100,
    bp_addressing_bus_speed_120,
    bp_addressing_bus_speed_133,
    bp_addressing_bus_speed_2500,
    bp_addressing_bus_speed_reserved
} bp_addressing_bus_speed;

/* PCI bus widths */
typedef enum {
    bp_addressing_bus_width_unknown = 0,
    bp_addressing_bus_width_32,
    bp_addressing_bus_width_64,
    bp_addressing_bus_width_pciex_1,
    bp_addressing_bus_width_pciex_2,
    bp_addressing_bus_width_pciex_4,
    bp_addressing_bus_width_reserved
} bp_addressing_bus_width;

/* PHY status info structure and supporting enums */
typedef enum {
    bp_addressing_cable_length_50 = 0,
    bp_addressing_cable_length_50_80,
    bp_addressing_cable_length_80_110,
    bp_addressing_cable_length_110_140,
    bp_addressing_cable_length_140,
    bp_addressing_cable_length_undefined = 0xFF
} bp_addressing_cable_length;

typedef enum {
    bp_addressing_gg_cable_length_60 = 0,
    bp_addressing_gg_cable_length_60_115 = 1,
    bp_addressing_gg_cable_length_115_150 = 2,
    bp_addressing_gg_cable_length_150 = 4
} bp_addressing_gg_cable_length;

typedef enum {
    bp_addressing_igp_cable_length_10  = 10,
    bp_addressing_igp_cable_length_20  = 20,
    bp_addressing_igp_cable_length_30  = 30,
    bp_addressing_igp_cable_length_40  = 40,
    bp_addressing_igp_cable_length_50  = 50,
    bp_addressing_igp_cable_length_60  = 60,
    bp_addressing_igp_cable_length_70  = 70,
    bp_addressing_igp_cable_length_80  = 80,
    bp_addressing_igp_cable_length_90  = 90,
    bp_addressing_igp_cable_length_100 = 100,
    bp_addressing_igp_cable_length_110 = 110,
    bp_addressing_igp_cable_length_115 = 115,
    bp_addressing_igp_cable_length_120 = 120,
    bp_addressing_igp_cable_length_130 = 130,
    bp_addressing_igp_cable_length_140 = 140,
    bp_addressing_igp_cable_length_150 = 150,
    bp_addressing_igp_cable_length_160 = 160,
    bp_addressing_igp_cable_length_170 = 170,
    bp_addressing_igp_cable_length_180 = 180
} bp_addressing_igp_cable_length;

typedef enum {
    bp_addressing_10bt_ext_dist_enable_normal = 0,
    bp_addressing_10bt_ext_dist_enable_lower,
    bp_addressing_10bt_ext_dist_enable_undefined = 0xFF
} bp_addressing_10bt_ext_dist_enable;

typedef enum {
    bp_addressing_rev_polarity_normal = 0,
    bp_addressing_rev_polarity_reversed,
    bp_addressing_rev_polarity_undefined = 0xFF
} bp_addressing_rev_polarity;

typedef enum {
    bp_addressing_downshift_normal = 0,
    bp_addressing_downshift_activated,
    bp_addressing_downshift_undefined = 0xFF
} bp_addressing_downshift;

typedef enum {
    bp_addressing_smart_speed_default = 0,
    bp_addressing_smart_speed_on,
    bp_addressing_smart_speed_off
} bp_addressing_smart_speed;

typedef enum {
    bp_addressing_polarity_reversal_enabled = 0,
    bp_addressing_polarity_reversal_disabled,
    bp_addressing_polarity_reversal_undefined = 0xFF
} bp_addressing_polarity_reversal;

typedef enum {
    bp_addressing_auto_x_mode_manual_mdi = 0,
    bp_addressing_auto_x_mode_manual_mdix,
    bp_addressing_auto_x_mode_auto1,
    bp_addressing_auto_x_mode_auto2,
    bp_addressing_auto_x_mode_undefined = 0xFF
} bp_addressing_auto_x_mode;

typedef enum {
    bp_addressing_1000t_rx_status_not_ok = 0,
    bp_addressing_1000t_rx_status_ok,
    bp_addressing_1000t_rx_status_undefined = 0xFF
} bp_addressing_1000t_rx_status;

typedef enum {
    bp_addressing_phy_m88 = 0,
    bp_addressing_phy_igp,
    bp_addressing_phy_igp_2,
    bp_addressing_phy_gg82563,
    bp_addressing_phy_igp_3,
    bp_addressing_phy_ife,
    bp_addressing_phy_undefined = 0xFF
} bp_addressing_phy_type;

typedef enum {
    bp_addressing_ms_hw_default = 0,
    bp_addressing_ms_force_master,
    bp_addressing_ms_force_slave,
    bp_addressing_ms_auto
} bp_addressing_ms_type;

typedef enum {
    bp_addressing_ffe_config_enabled = 0,
    bp_addressing_ffe_config_active,
    bp_addressing_ffe_config_blocked
} bp_addressing_ffe_config;

typedef enum {
    bp_addressing_dsp_config_disabled = 0,
    bp_addressing_dsp_config_enabled,
    bp_addressing_dsp_config_activated,
    bp_addressing_dsp_config_undefined = 0xFF
} bp_addressing_dsp_config;

struct bp_addressing_phy_info {
    bp_addressing_cable_length cable_length;
    bp_addressing_10bt_ext_dist_enable extended_10bt_distance;
    bp_addressing_rev_polarity cable_polarity;
    bp_addressing_downshift downshift;
    bp_addressing_polarity_reversal polarity_correction;
    bp_addressing_auto_x_mode mdix_mode;
    bp_addressing_1000t_rx_status local_rx;
    bp_addressing_1000t_rx_status remote_rx;
};

struct bp_addressing_phy_stats {
    uint32_t idle_errors;
    uint32_t receive_errors;
};

struct bp_addressing_eeprom_info {
    bp_addressing_eeprom_type type;
    uint16_t word_size;
    uint16_t opcode_bits;
    uint16_t address_bits;
    uint16_t delay_usec;
    uint16_t page_size;
    boolean_t use_eerd;
    boolean_t use_eewr;
};

/* Flex ASF Information */
#define BP_ADDRESSING_HOST_IF_MAX_SIZE  2048

typedef enum {
    bp_addressing_byte_align = 0,
    bp_addressing_word_align = 1,
    bp_addressing_dword_align = 2
} bp_addressing_align_type;



/* Error Codes */
#define BP_ADDRESSING_SUCCESS      0
#define BP_ADDRESSING_ERR_EEPROM   1
#define BP_ADDRESSING_ERR_PHY      2
#define BP_ADDRESSING_ERR_CONFIG   3
#define BP_ADDRESSING_ERR_PARAM    4
#define BP_ADDRESSING_ERR_MAC_TYPE 5
#define BP_ADDRESSING_ERR_PHY_TYPE 6
#define BP_ADDRESSING_ERR_RESET   9
#define BP_ADDRESSING_ERR_MASTER_REQUESTS_PENDING 10
#define BP_ADDRESSING_ERR_HOST_INTERFACE_COMMAND 11
#define BP_ADDRESSING_BLK_PHY_RESET   12
#define BP_ADDRESSING_ERR_SWFW_SYNC 13

/* Function prototypes */
/* Initialization */
int32_t bp_addressing_set_mac_type(struct bp_addressing_hw *hw);
void bp_addressing_set_media_type(struct bp_addressing_hw *hw);

/* EEPROM Functions */
int32_t bp_addressing_init_eeprom_params(struct bp_addressing_hw *hw);

/* MNG HOST IF functions */
uint32_t bp_addressing_enable_mng_pass_thru(struct bp_addressing_hw *hw);

#define BP_ADDRESSING_MNG_DHCP_TX_PAYLOAD_CMD   64
#define BP_ADDRESSING_HI_MAX_MNG_DATA_LENGTH    0x6F8   /* Host Interface data length */

#define BP_ADDRESSING_MNG_DHCP_COMMAND_TIMEOUT  10      /* Time in ms to process MNG command */
#define BP_ADDRESSING_MNG_DHCP_COOKIE_OFFSET	0x6F0   /* Cookie offset */
#define BP_ADDRESSING_MNG_DHCP_COOKIE_LENGTH	0x10    /* Cookie length */
#define BP_ADDRESSING_MNG_IAMT_MODE		0x3
#define BP_ADDRESSING_MNG_ICH_IAMT_MODE         0x2
#define BP_ADDRESSING_IAMT_SIGNATURE            0x544D4149 /* Intel(R) Active Management Technology signature */

#define BP_ADDRESSING_MNG_DHCP_COOKIE_STATUS_PARSING_SUPPORT 0x1 /* DHCP parsing enabled */
#define BP_ADDRESSING_MNG_DHCP_COOKIE_STATUS_VLAN_SUPPORT    0x2 /* DHCP parsing enabled */
#define BP_ADDRESSING_VFTA_ENTRY_SHIFT                       0x5
#define BP_ADDRESSING_VFTA_ENTRY_MASK                        0x7F
#define BP_ADDRESSING_VFTA_ENTRY_BIT_SHIFT_MASK              0x1F

struct bp_addressing_host_mng_command_header {
    uint8_t command_id;
    uint8_t checksum;
    uint16_t reserved1;
    uint16_t reserved2;
    uint16_t command_length;
};

struct bp_addressing_host_mng_command_info {
    struct bp_addressing_host_mng_command_header command_header;  /* Command Head/Command Result Head has 4 bytes */
    uint8_t command_data[BP_ADDRESSING_HI_MAX_MNG_DATA_LENGTH];   /* Command data can length 0..0x658*/
};
#ifdef __BIG_ENDIAN
struct bp_addressing_host_mng_dhcp_cookie{
    uint32_t signature;
    uint16_t vlan_id;
    uint8_t reserved0;
    uint8_t status;
    uint32_t reserved1;
    uint8_t checksum;
    uint8_t reserved3;
    uint16_t reserved2;
};
#else
struct bp_addressing_host_mng_dhcp_cookie{
    uint32_t signature;
    uint8_t status;
    uint8_t reserved0;
    uint16_t vlan_id;
    uint32_t reserved1;
    uint16_t reserved2;
    uint8_t reserved3;
    uint8_t checksum;
};
#endif

int32_t bp_addressing_read_eeprom(struct bp_addressing_hw *hw, uint16_t reg, uint16_t words, uint16_t *data);
int32_t bp_addressing_validate_eeprom_checksum(struct bp_addressing_hw *hw);
int32_t bp_addressing_update_eeprom_checksum(struct bp_addressing_hw *hw);
int32_t bp_addressing_write_eeprom(struct bp_addressing_hw *hw, uint16_t reg, uint16_t words, uint16_t *data);
int32_t bp_addressing_read_part_num(struct bp_addressing_hw *hw, uint32_t * part_num);
int32_t bp_addressing_read_mac_addr(struct bp_addressing_hw * hw);

#define BP_ADDRESSING_READ_REG_IO(a, reg) \
    bp_addressing_read_reg_io((a), BP_ADDRESSING_##reg)
#define BP_ADDRESSING_WRITE_REG_IO(a, reg, val) \
    bp_addressing_write_reg_io((a), BP_ADDRESSING_##reg, val)

/* PCI Device IDs */
#define BP_ADDRESSING_DEV_ID_82542               0x1000
#define BP_ADDRESSING_DEV_ID_82543GC_FIBER       0x1001
#define BP_ADDRESSING_DEV_ID_82543GC_COPPER      0x1004
#define BP_ADDRESSING_DEV_ID_82544EI_COPPER      0x1008
#define BP_ADDRESSING_DEV_ID_82544EI_FIBER       0x1009
#define BP_ADDRESSING_DEV_ID_82544GC_COPPER      0x100C
#define BP_ADDRESSING_DEV_ID_82544GC_LOM         0x100D
#define BP_ADDRESSING_DEV_ID_82540EM             0x100E
#define BP_ADDRESSING_DEV_ID_82540EM_LOM         0x1015
#define BP_ADDRESSING_DEV_ID_82540EP_LOM         0x1016
#define BP_ADDRESSING_DEV_ID_82540EP             0x1017
#define BP_ADDRESSING_DEV_ID_82540EP_LP          0x101E
#define BP_ADDRESSING_DEV_ID_82545EM_COPPER      0x100F
#define BP_ADDRESSING_DEV_ID_82545EM_FIBER       0x1011
#define BP_ADDRESSING_DEV_ID_82545GM_COPPER      0x1026
#define BP_ADDRESSING_DEV_ID_82545GM_FIBER       0x1027
#define BP_ADDRESSING_DEV_ID_82545GM_SERDES      0x1028
#define BP_ADDRESSING_DEV_ID_82546EB_COPPER      0x1010
#define BP_ADDRESSING_DEV_ID_82546EB_FIBER       0x1012
#define BP_ADDRESSING_DEV_ID_82546EB_QUAD_COPPER 0x101D
#define BP_ADDRESSING_DEV_ID_82541EI             0x1013
#define BP_ADDRESSING_DEV_ID_82541EI_MOBILE      0x1018
#define BP_ADDRESSING_DEV_ID_82541ER_LOM         0x1014
#define BP_ADDRESSING_DEV_ID_82541ER             0x1078
#define BP_ADDRESSING_DEV_ID_82547GI             0x1075
#define BP_ADDRESSING_DEV_ID_82541GI             0x1076
#define BP_ADDRESSING_DEV_ID_82541GI_MOBILE      0x1077
#define BP_ADDRESSING_DEV_ID_82541GI_LF          0x107C
#define BP_ADDRESSING_DEV_ID_82546GB_COPPER      0x1079
#define BP_ADDRESSING_DEV_ID_82546GB_FIBER       0x107A
#define BP_ADDRESSING_DEV_ID_82546GB_SERDES      0x107B
#define BP_ADDRESSING_DEV_ID_82546GB_PCIE        0x108A
#define BP_ADDRESSING_DEV_ID_82546GB_QUAD_COPPER 0x1099
#define BP_ADDRESSING_DEV_ID_82547EI             0x1019
#define BP_ADDRESSING_DEV_ID_82547EI_MOBILE      0x101A
#define BP_ADDRESSING_DEV_ID_82571EB_COPPER      0x105E
#define BP_ADDRESSING_DEV_ID_82571EB_FIBER       0x105F
#define BP_ADDRESSING_DEV_ID_82571EB_SERDES      0x1060
#define BP_ADDRESSING_DEV_ID_82572EI_COPPER      0x107D
#define BP_ADDRESSING_DEV_ID_82572EI_FIBER       0x107E
#define BP_ADDRESSING_DEV_ID_82572EI_SERDES      0x107F
#define BP_ADDRESSING_DEV_ID_82572EI             0x10B9
#define BP_ADDRESSING_DEV_ID_82573E              0x108B
#define BP_ADDRESSING_DEV_ID_82573E_IAMT         0x108C
#define BP_ADDRESSING_DEV_ID_82573L              0x109A
#define BP_ADDRESSING_DEV_ID_82546GB_QUAD_COPPER_KSP3 0x10B5
#define BP_ADDRESSING_DEV_ID_80003ES2LAN_COPPER_DPT     0x1096
#define BP_ADDRESSING_DEV_ID_80003ES2LAN_SERDES_DPT     0x1098
#define BP_ADDRESSING_DEV_ID_80003ES2LAN_COPPER_SPT     0x10BA
#define BP_ADDRESSING_DEV_ID_80003ES2LAN_SERDES_SPT     0x10BB

#define BP_ADDRESSING_DEV_ID_ICH8_IGP_M_AMT      0x1049
#define BP_ADDRESSING_DEV_ID_ICH8_IGP_AMT        0x104A
#define BP_ADDRESSING_DEV_ID_ICH8_IGP_C          0x104B
#define BP_ADDRESSING_DEV_ID_ICH8_IFE            0x104C
#define BP_ADDRESSING_DEV_ID_ICH8_IGP_M          0x104D


#define NODE_ADDRESS_SIZE 6
#define ETH_LENGTH_OF_ADDRESS 6

/* MAC decode size is 128K - This is the size of BAR0 */
#define MAC_DECODE_SIZE (128 * 1024)

#define BP_ADDRESSING_82542_2_0_REV_ID 2
#define BP_ADDRESSING_82542_2_1_REV_ID 3
#define BP_ADDRESSING_REVISION_0       0
#define BP_ADDRESSING_REVISION_1       1
#define BP_ADDRESSING_REVISION_2       2
#define BP_ADDRESSING_REVISION_3       3

#define SPEED_10    10
#define SPEED_100   100
#define SPEED_1000  1000
#define HALF_DUPLEX 1
#define FULL_DUPLEX 2

/* The sizes (in bytes) of a ethernet packet */
#define ENET_HEADER_SIZE             14
#define MAXIMUM_ETHERNET_FRAME_SIZE  1518 /* With FCS */
#define MINIMUM_ETHERNET_FRAME_SIZE  64   /* With FCS */
#define ETHERNET_FCS_SIZE            4
#define MAXIMUM_ETHERNET_PACKET_SIZE \
    (MAXIMUM_ETHERNET_FRAME_SIZE - ETHERNET_FCS_SIZE)
#define MINIMUM_ETHERNET_PACKET_SIZE \
    (MINIMUM_ETHERNET_FRAME_SIZE - ETHERNET_FCS_SIZE)
#define CRC_LENGTH                   ETHERNET_FCS_SIZE
#define MAX_JUMBO_FRAME_SIZE         0x3F00


/* 802.1q VLAN Packet Sizes */
#define VLAN_TAG_SIZE                     4     /* 802.3ac tag (not DMAed) */

/* Ethertype field values */
#define ETHERNET_IEEE_VLAN_TYPE 0x8100  /* 802.3ac packet */
#define ETHERNET_IP_TYPE        0x0800  /* IP packets */
#define ETHERNET_ARP_TYPE       0x0806  /* Address Resolution Protocol (ARP) */

/* Packet Header defines */
#define IP_PROTOCOL_TCP    6
#define IP_PROTOCOL_UDP    0x11
#define MAX_PS_BUFFERS 4

/* Filters */
#define BP_ADDRESSING_NUM_UNICAST          16   /* Unicast filter entries */
#define BP_ADDRESSING_MC_TBL_SIZE          128  /* Multicast Filter Table (4096 bits) */
#define BP_ADDRESSING_VLAN_FILTER_TBL_SIZE 128  /* VLAN Filter Table (4096 bits) */

#define BP_ADDRESSING_NUM_UNICAST_ICH8LAN  7
#define BP_ADDRESSING_MC_TBL_SIZE_ICH8LAN  32


/* Receive Address Register */
struct bp_addressing_rar {
    volatile uint32_t low;      /* receive address low */
    volatile uint32_t high;     /* receive address high */
};

/* Number of entries in the Multicast Table Array (MTA). */
#define BP_ADDRESSING_NUM_MTA_REGISTERS 128
#define BP_ADDRESSING_NUM_MTA_REGISTERS_ICH8LAN 32

/* IPv4 Address Table Entry */
struct bp_addressing_ipv4_at_entry {
    volatile uint32_t ipv4_addr;        /* IP Address (RW) */
    volatile uint32_t reserved;
};

/* Four wakeup IP addresses are supported */
#define BP_ADDRESSING_WAKEUP_IP_ADDRESS_COUNT_MAX 4
#define BP_ADDRESSING_IP4AT_SIZE                  BP_ADDRESSING_WAKEUP_IP_ADDRESS_COUNT_MAX
#define BP_ADDRESSING_IP4AT_SIZE_ICH8LAN          3
#define BP_ADDRESSING_IP6AT_SIZE                  1

/* IPv6 Address Table Entry */
struct bp_addressing_ipv6_at_entry {
    volatile uint8_t ipv6_addr[16];
};

/* Flexible Filter Length Table Entry */
struct bp_addressing_fflt_entry {
    volatile uint32_t length;   /* Flexible Filter Length (RW) */
    volatile uint32_t reserved;
};

/* Flexible Filter Mask Table Entry */
struct bp_addressing_ffmt_entry {
    volatile uint32_t mask;     /* Flexible Filter Mask (RW) */
    volatile uint32_t reserved;
};

/* Flexible Filter Value Table Entry */
struct bp_addressing_ffvt_entry {
    volatile uint32_t value;    /* Flexible Filter Value (RW) */
    volatile uint32_t reserved;
};

/* Four Flexible Filters are supported */
#define BP_ADDRESSING_FLEXIBLE_FILTER_COUNT_MAX 4

/* Each Flexible Filter is at most 128 (0x80) bytes in length */
#define BP_ADDRESSING_FLEXIBLE_FILTER_SIZE_MAX  128

#define BP_ADDRESSING_FFLT_SIZE BP_ADDRESSING_FLEXIBLE_FILTER_COUNT_MAX
#define BP_ADDRESSING_FFMT_SIZE BP_ADDRESSING_FLEXIBLE_FILTER_SIZE_MAX
#define BP_ADDRESSING_FFVT_SIZE BP_ADDRESSING_FLEXIBLE_FILTER_SIZE_MAX

#define BP_ADDRESSING_DISABLE_SERDES_LOOPBACK   0x0400

/* Register Set. (82543, 82544)
 *
 * Registers are defined to be 32 bits and  should be accessed as 32 bit values.
 * These registers are physically located on the NIC, but are mapped into the
 * host memory address space.
 *
 * RW - register is both readable and writable
 * RO - register is read only
 * WO - register is write only
 * R/clr - register is read only and is cleared when read
 * A - register array
 */
#define BP_ADDRESSING_CTRL     0x00000  /* Device Control - RW */
#define BP_ADDRESSING_CTRL_DUP 0x00004  /* Device Control Duplicate (Shadow) - RW */
#define BP_ADDRESSING_STATUS   0x00008  /* Device Status - RO */
#define BP_ADDRESSING_EECD     0x00010  /* EEPROM/Flash Control - RW */
#define BP_ADDRESSING_EERD     0x00014  /* EEPROM Read - RW */
#define BP_ADDRESSING_CTRL_EXT 0x00018  /* Extended Device Control - RW */
#define BP_ADDRESSING_FLA      0x0001C  /* Flash Access - RW */
#define BP_ADDRESSING_MDIC     0x00020  /* MDI Control - RW */
#define BP_ADDRESSING_SCTL     0x00024  /* SerDes Control - RW */
#define BP_ADDRESSING_FEXTNVM  0x00028  /* Future Extended NVM register */
#define BP_ADDRESSING_FCAL     0x00028  /* Flow Control Address Low - RW */
#define BP_ADDRESSING_FCAH     0x0002C  /* Flow Control Address High -RW */
#define BP_ADDRESSING_FCT      0x00030  /* Flow Control Type - RW */
#define BP_ADDRESSING_VET      0x00038  /* VLAN Ether Type - RW */
#define BP_ADDRESSING_ICR      0x000C0  /* Interrupt Cause Read - R/clr */
#define BP_ADDRESSING_ITR      0x000C4  /* Interrupt Throttling Rate - RW */
#define BP_ADDRESSING_ICS      0x000C8  /* Interrupt Cause Set - WO */
#define BP_ADDRESSING_IMS      0x000D0  /* Interrupt Mask Set - RW */
#define BP_ADDRESSING_IMC      0x000D8  /* Interrupt Mask Clear - WO */
#define BP_ADDRESSING_IAM      0x000E0  /* Interrupt Acknowledge Auto Mask */
#define BP_ADDRESSING_RCTL     0x00100  /* RX Control - RW */
#define BP_ADDRESSING_RDTR1    0x02820  /* RX Delay Timer (1) - RW */
#define BP_ADDRESSING_RDBAL1   0x02900  /* RX Descriptor Base Address Low (1) - RW */
#define BP_ADDRESSING_RDBAH1   0x02904  /* RX Descriptor Base Address High (1) - RW */
#define BP_ADDRESSING_RDLEN1   0x02908  /* RX Descriptor Length (1) - RW */
#define BP_ADDRESSING_RDH1     0x02910  /* RX Descriptor Head (1) - RW */
#define BP_ADDRESSING_RDT1     0x02918  /* RX Descriptor Tail (1) - RW */
#define BP_ADDRESSING_FCTTV    0x00170  /* Flow Control Transmit Timer Value - RW */
#define BP_ADDRESSING_TXCW     0x00178  /* TX Configuration Word - RW */
#define BP_ADDRESSING_RXCW     0x00180  /* RX Configuration Word - RO */
#define BP_ADDRESSING_TCTL     0x00400  /* TX Control - RW */
#define BP_ADDRESSING_TCTL_EXT 0x00404  /* Extended TX Control - RW */
#define BP_ADDRESSING_TIPG     0x00410  /* TX Inter-packet gap -RW */
#define BP_ADDRESSING_TBT      0x00448  /* TX Burst Timer - RW */
#define BP_ADDRESSING_AIT      0x00458  /* Adaptive Interframe Spacing Throttle - RW */
#define BP_ADDRESSING_LEDCTL   0x00E00  /* LED Control - RW */
#define BP_ADDRESSING_EXTCNF_CTRL  0x00F00  /* Extended Configuration Control */
#define BP_ADDRESSING_EXTCNF_SIZE  0x00F08  /* Extended Configuration Size */
#define BP_ADDRESSING_PHY_CTRL     0x00F10  /* PHY Control Register in CSR */
#define FEXTNVM_SW_CONFIG  0x0001
#define BP_ADDRESSING_PBA      0x01000  /* Packet Buffer Allocation - RW */
#define BP_ADDRESSING_PBS      0x01008  /* Packet Buffer Size */
#define BP_ADDRESSING_EEMNGCTL 0x01010  /* MNG EEprom Control */
#define BP_ADDRESSING_FLASH_UPDATES 1000
#define BP_ADDRESSING_EEARBC   0x01024  /* EEPROM Auto Read Bus Control */
#define BP_ADDRESSING_FLASHT   0x01028  /* FLASH Timer Register */
#define BP_ADDRESSING_EEWR     0x0102C  /* EEPROM Write Register - RW */
#define BP_ADDRESSING_FLSWCTL  0x01030  /* FLASH control register */
#define BP_ADDRESSING_FLSWDATA 0x01034  /* FLASH data register */
#define BP_ADDRESSING_FLSWCNT  0x01038  /* FLASH Access Counter */
#define BP_ADDRESSING_FLOP     0x0103C  /* FLASH Opcode Register */
#define BP_ADDRESSING_ERT      0x02008  /* Early Rx Threshold - RW */
#define BP_ADDRESSING_FCRTL    0x02160  /* Flow Control Receive Threshold Low - RW */
#define BP_ADDRESSING_FCRTH    0x02168  /* Flow Control Receive Threshold High - RW */
#define BP_ADDRESSING_PSRCTL   0x02170  /* Packet Split Receive Control - RW */
#define BP_ADDRESSING_RDBAL    0x02800  /* RX Descriptor Base Address Low - RW */
#define BP_ADDRESSING_RDBAH    0x02804  /* RX Descriptor Base Address High - RW */
#define BP_ADDRESSING_RDLEN    0x02808  /* RX Descriptor Length - RW */
#define BP_ADDRESSING_RDH      0x02810  /* RX Descriptor Head - RW */
#define BP_ADDRESSING_RDT      0x02818  /* RX Descriptor Tail - RW */
#define BP_ADDRESSING_RDTR     0x02820  /* RX Delay Timer - RW */
#define BP_ADDRESSING_RDBAL0   BP_ADDRESSING_RDBAL /* RX Desc Base Address Low (0) - RW */
#define BP_ADDRESSING_RDBAH0   BP_ADDRESSING_RDBAH /* RX Desc Base Address High (0) - RW */
#define BP_ADDRESSING_RDLEN0   BP_ADDRESSING_RDLEN /* RX Desc Length (0) - RW */
#define BP_ADDRESSING_RDH0     BP_ADDRESSING_RDH   /* RX Desc Head (0) - RW */
#define BP_ADDRESSING_RDT0     BP_ADDRESSING_RDT   /* RX Desc Tail (0) - RW */
#define BP_ADDRESSING_RDTR0    BP_ADDRESSING_RDTR  /* RX Delay Timer (0) - RW */
#define BP_ADDRESSING_RXDCTL   0x02828  /* RX Descriptor Control queue 0 - RW */
#define BP_ADDRESSING_RXDCTL1  0x02928  /* RX Descriptor Control queue 1 - RW */
#define BP_ADDRESSING_RADV     0x0282C  /* RX Interrupt Absolute Delay Timer - RW */
#define BP_ADDRESSING_RSRPD    0x02C00  /* RX Small Packet Detect - RW */
#define BP_ADDRESSING_RAID     0x02C08  /* Receive Ack Interrupt Delay - RW */
#define BP_ADDRESSING_TXDMAC   0x03000  /* TX DMA Control - RW */
#define BP_ADDRESSING_KABGTXD  0x03004  /* AFE Band Gap Transmit Ref Data */
#define BP_ADDRESSING_TDFH     0x03410  /* TX Data FIFO Head - RW */
#define BP_ADDRESSING_TDFT     0x03418  /* TX Data FIFO Tail - RW */
#define BP_ADDRESSING_TDFHS    0x03420  /* TX Data FIFO Head Saved - RW */
#define BP_ADDRESSING_TDFTS    0x03428  /* TX Data FIFO Tail Saved - RW */
#define BP_ADDRESSING_TDFPC    0x03430  /* TX Data FIFO Packet Count - RW */
#define BP_ADDRESSING_TDBAL    0x03800  /* TX Descriptor Base Address Low - RW */
#define BP_ADDRESSING_TDBAH    0x03804  /* TX Descriptor Base Address High - RW */
#define BP_ADDRESSING_TDLEN    0x03808  /* TX Descriptor Length - RW */
#define BP_ADDRESSING_TDH      0x03810  /* TX Descriptor Head - RW */
#define BP_ADDRESSING_TDT      0x03818  /* TX Descripotr Tail - RW */
#define BP_ADDRESSING_TIDV     0x03820  /* TX Interrupt Delay Value - RW */
#define BP_ADDRESSING_TXDCTL   0x03828  /* TX Descriptor Control - RW */
#define BP_ADDRESSING_TADV     0x0382C  /* TX Interrupt Absolute Delay Val - RW */
#define BP_ADDRESSING_TSPMT    0x03830  /* TCP Segmentation PAD & Min Threshold - RW */
#define BP_ADDRESSING_TARC0    0x03840  /* TX Arbitration Count (0) */
#define BP_ADDRESSING_TDBAL1   0x03900  /* TX Desc Base Address Low (1) - RW */
#define BP_ADDRESSING_TDBAH1   0x03904  /* TX Desc Base Address High (1) - RW */
#define BP_ADDRESSING_TDLEN1   0x03908  /* TX Desc Length (1) - RW */
#define BP_ADDRESSING_TDH1     0x03910  /* TX Desc Head (1) - RW */
#define BP_ADDRESSING_TDT1     0x03918  /* TX Desc Tail (1) - RW */
#define BP_ADDRESSING_TXDCTL1  0x03928  /* TX Descriptor Control (1) - RW */
#define BP_ADDRESSING_TARC1    0x03940  /* TX Arbitration Count (1) */
#define BP_ADDRESSING_CRCERRS  0x04000  /* CRC Error Count - R/clr */
#define BP_ADDRESSING_ALGNERRC 0x04004  /* Alignment Error Count - R/clr */
#define BP_ADDRESSING_SYMERRS  0x04008  /* Symbol Error Count - R/clr */
#define BP_ADDRESSING_RXERRC   0x0400C  /* Receive Error Count - R/clr */
#define BP_ADDRESSING_MPC      0x04010  /* Missed Packet Count - R/clr */
#define BP_ADDRESSING_SCC      0x04014  /* Single Collision Count - R/clr */
#define BP_ADDRESSING_ECOL     0x04018  /* Excessive Collision Count - R/clr */
#define BP_ADDRESSING_MCC      0x0401C  /* Multiple Collision Count - R/clr */
#define BP_ADDRESSING_LATECOL  0x04020  /* Late Collision Count - R/clr */
#define BP_ADDRESSING_COLC     0x04028  /* Collision Count - R/clr */
#define BP_ADDRESSING_DC       0x04030  /* Defer Count - R/clr */
#define BP_ADDRESSING_TNCRS    0x04034  /* TX-No CRS - R/clr */
#define BP_ADDRESSING_SEC      0x04038  /* Sequence Error Count - R/clr */
#define BP_ADDRESSING_CEXTERR  0x0403C  /* Carrier Extension Error Count - R/clr */
#define BP_ADDRESSING_RLEC     0x04040  /* Receive Length Error Count - R/clr */
#define BP_ADDRESSING_XONRXC   0x04048  /* XON RX Count - R/clr */
#define BP_ADDRESSING_XONTXC   0x0404C  /* XON TX Count - R/clr */
#define BP_ADDRESSING_XOFFRXC  0x04050  /* XOFF RX Count - R/clr */
#define BP_ADDRESSING_XOFFTXC  0x04054  /* XOFF TX Count - R/clr */
#define BP_ADDRESSING_FCRUC    0x04058  /* Flow Control RX Unsupported Count- R/clr */
#define BP_ADDRESSING_PRC64    0x0405C  /* Packets RX (64 bytes) - R/clr */
#define BP_ADDRESSING_PRC127   0x04060  /* Packets RX (65-127 bytes) - R/clr */
#define BP_ADDRESSING_PRC255   0x04064  /* Packets RX (128-255 bytes) - R/clr */
#define BP_ADDRESSING_PRC511   0x04068  /* Packets RX (255-511 bytes) - R/clr */
#define BP_ADDRESSING_PRC1023  0x0406C  /* Packets RX (512-1023 bytes) - R/clr */
#define BP_ADDRESSING_PRC1522  0x04070  /* Packets RX (1024-1522 bytes) - R/clr */
#define BP_ADDRESSING_GPRC     0x04074  /* Good Packets RX Count - R/clr */
#define BP_ADDRESSING_BPRC     0x04078  /* Broadcast Packets RX Count - R/clr */
#define BP_ADDRESSING_MPRC     0x0407C  /* Multicast Packets RX Count - R/clr */
#define BP_ADDRESSING_GPTC     0x04080  /* Good Packets TX Count - R/clr */
#define BP_ADDRESSING_GORCL    0x04088  /* Good Octets RX Count Low - R/clr */
#define BP_ADDRESSING_GORCH    0x0408C  /* Good Octets RX Count High - R/clr */
#define BP_ADDRESSING_GOTCL    0x04090  /* Good Octets TX Count Low - R/clr */
#define BP_ADDRESSING_GOTCH    0x04094  /* Good Octets TX Count High - R/clr */
#define BP_ADDRESSING_RNBC     0x040A0  /* RX No Buffers Count - R/clr */
#define BP_ADDRESSING_RUC      0x040A4  /* RX Undersize Count - R/clr */
#define BP_ADDRESSING_RFC      0x040A8  /* RX Fragment Count - R/clr */
#define BP_ADDRESSING_ROC      0x040AC  /* RX Oversize Count - R/clr */
#define BP_ADDRESSING_RJC      0x040B0  /* RX Jabber Count - R/clr */
#define BP_ADDRESSING_MGTPRC   0x040B4  /* Management Packets RX Count - R/clr */
#define BP_ADDRESSING_MGTPDC   0x040B8  /* Management Packets Dropped Count - R/clr */
#define BP_ADDRESSING_MGTPTC   0x040BC  /* Management Packets TX Count - R/clr */
#define BP_ADDRESSING_TORL     0x040C0  /* Total Octets RX Low - R/clr */
#define BP_ADDRESSING_TORH     0x040C4  /* Total Octets RX High - R/clr */
#define BP_ADDRESSING_TOTL     0x040C8  /* Total Octets TX Low - R/clr */
#define BP_ADDRESSING_TOTH     0x040CC  /* Total Octets TX High - R/clr */
#define BP_ADDRESSING_TPR      0x040D0  /* Total Packets RX - R/clr */
#define BP_ADDRESSING_TPT      0x040D4  /* Total Packets TX - R/clr */
#define BP_ADDRESSING_PTC64    0x040D8  /* Packets TX (64 bytes) - R/clr */
#define BP_ADDRESSING_PTC127   0x040DC  /* Packets TX (65-127 bytes) - R/clr */
#define BP_ADDRESSING_PTC255   0x040E0  /* Packets TX (128-255 bytes) - R/clr */
#define BP_ADDRESSING_PTC511   0x040E4  /* Packets TX (256-511 bytes) - R/clr */
#define BP_ADDRESSING_PTC1023  0x040E8  /* Packets TX (512-1023 bytes) - R/clr */
#define BP_ADDRESSING_PTC1522  0x040EC  /* Packets TX (1024-1522 Bytes) - R/clr */
#define BP_ADDRESSING_MPTC     0x040F0  /* Multicast Packets TX Count - R/clr */
#define BP_ADDRESSING_BPTC     0x040F4  /* Broadcast Packets TX Count - R/clr */
#define BP_ADDRESSING_TSCTC    0x040F8  /* TCP Segmentation Context TX - R/clr */
#define BP_ADDRESSING_TSCTFC   0x040FC  /* TCP Segmentation Context TX Fail - R/clr */
#define BP_ADDRESSING_IAC      0x04100  /* Interrupt Assertion Count */
#define BP_ADDRESSING_ICRXPTC  0x04104  /* Interrupt Cause Rx Packet Timer Expire Count */
#define BP_ADDRESSING_ICRXATC  0x04108  /* Interrupt Cause Rx Absolute Timer Expire Count */
#define BP_ADDRESSING_ICTXPTC  0x0410C  /* Interrupt Cause Tx Packet Timer Expire Count */
#define BP_ADDRESSING_ICTXATC  0x04110  /* Interrupt Cause Tx Absolute Timer Expire Count */
#define BP_ADDRESSING_ICTXQEC  0x04118  /* Interrupt Cause Tx Queue Empty Count */
#define BP_ADDRESSING_ICTXQMTC 0x0411C  /* Interrupt Cause Tx Queue Minimum Threshold Count */
#define BP_ADDRESSING_ICRXDMTC 0x04120  /* Interrupt Cause Rx Descriptor Minimum Threshold Count */
#define BP_ADDRESSING_ICRXOC   0x04124  /* Interrupt Cause Receiver Overrun Count */
#define BP_ADDRESSING_RXCSUM   0x05000  /* RX Checksum Control - RW */
#define BP_ADDRESSING_RFCTL    0x05008  /* Receive Filter Control*/
#define BP_ADDRESSING_MTA      0x05200  /* Multicast Table Array - RW Array */
#define BP_ADDRESSING_RA       0x05400  /* Receive Address - RW Array */
#define BP_ADDRESSING_VFTA     0x05600  /* VLAN Filter Table Array - RW Array */
#define BP_ADDRESSING_WUC      0x05800  /* Wakeup Control - RW */
#define BP_ADDRESSING_WUFC     0x05808  /* Wakeup Filter Control - RW */
#define BP_ADDRESSING_WUS      0x05810  /* Wakeup Status - RO */
#define BP_ADDRESSING_MANC     0x05820  /* Management Control - RW */
#define BP_ADDRESSING_IPAV     0x05838  /* IP Address Valid - RW */
#define BP_ADDRESSING_IP4AT    0x05840  /* IPv4 Address Table - RW Array */
#define BP_ADDRESSING_IP6AT    0x05880  /* IPv6 Address Table - RW Array */
#define BP_ADDRESSING_WUPL     0x05900  /* Wakeup Packet Length - RW */
#define BP_ADDRESSING_WUPM     0x05A00  /* Wakeup Packet Memory - RO A */
#define BP_ADDRESSING_FFLT     0x05F00  /* Flexible Filter Length Table - RW Array */
#define BP_ADDRESSING_HOST_IF  0x08800  /* Host Interface */
#define BP_ADDRESSING_FFMT     0x09000  /* Flexible Filter Mask Table - RW Array */
#define BP_ADDRESSING_FFVT     0x09800  /* Flexible Filter Value Table - RW Array */

#define BP_ADDRESSING_KUMCTRLSTA 0x00034 /* MAC-PHY interface - RW */
#define BP_ADDRESSING_MDPHYA     0x0003C  /* PHY address - RW */
#define BP_ADDRESSING_MANC2H     0x05860  /* Managment Control To Host - RW */
#define BP_ADDRESSING_SW_FW_SYNC 0x05B5C /* Software-Firmware Synchronization - RW */

#define BP_ADDRESSING_GCR       0x05B00 /* PCI-Ex Control */
#define BP_ADDRESSING_GSCL_1    0x05B10 /* PCI-Ex Statistic Control #1 */
#define BP_ADDRESSING_GSCL_2    0x05B14 /* PCI-Ex Statistic Control #2 */
#define BP_ADDRESSING_GSCL_3    0x05B18 /* PCI-Ex Statistic Control #3 */
#define BP_ADDRESSING_GSCL_4    0x05B1C /* PCI-Ex Statistic Control #4 */
#define BP_ADDRESSING_FACTPS    0x05B30 /* Function Active and Power State to MNG */
#define BP_ADDRESSING_SWSM      0x05B50 /* SW Semaphore */
#define BP_ADDRESSING_FWSM      0x05B54 /* FW Semaphore */
#define BP_ADDRESSING_FFLT_DBG  0x05F04 /* Debug Register */
#define BP_ADDRESSING_HICR      0x08F00 /* Host Inteface Control */

/* RSS registers */
#define BP_ADDRESSING_CPUVEC    0x02C10 /* CPU Vector Register - RW */
#define BP_ADDRESSING_MRQC      0x05818 /* Multiple Receive Control - RW */
#define BP_ADDRESSING_RETA      0x05C00 /* Redirection Table - RW Array */
#define BP_ADDRESSING_RSSRK     0x05C80 /* RSS Random Key - RW Array */
#define BP_ADDRESSING_RSSIM     0x05864 /* RSS Interrupt Mask */
#define BP_ADDRESSING_RSSIR     0x05868 /* RSS Interrupt Request */
/* Register Set (82542)
 *
 * Some of the 82542 registers are located at different offsets than they are
 * in more current versions of the 8254x. Despite the difference in location,
 * the registers function in the same manner.
 */
#define BP_ADDRESSING_82542_CTRL     BP_ADDRESSING_CTRL
#define BP_ADDRESSING_82542_CTRL_DUP BP_ADDRESSING_CTRL_DUP
#define BP_ADDRESSING_82542_STATUS   BP_ADDRESSING_STATUS
#define BP_ADDRESSING_82542_EECD     BP_ADDRESSING_EECD
#define BP_ADDRESSING_82542_EERD     BP_ADDRESSING_EERD
#define BP_ADDRESSING_82542_CTRL_EXT BP_ADDRESSING_CTRL_EXT
#define BP_ADDRESSING_82542_FLA      BP_ADDRESSING_FLA
#define BP_ADDRESSING_82542_MDIC     BP_ADDRESSING_MDIC
#define BP_ADDRESSING_82542_SCTL     BP_ADDRESSING_SCTL
#define BP_ADDRESSING_82542_FEXTNVM  BP_ADDRESSING_FEXTNVM
#define BP_ADDRESSING_82542_FCAL     BP_ADDRESSING_FCAL
#define BP_ADDRESSING_82542_FCAH     BP_ADDRESSING_FCAH
#define BP_ADDRESSING_82542_FCT      BP_ADDRESSING_FCT
#define BP_ADDRESSING_82542_VET      BP_ADDRESSING_VET
#define BP_ADDRESSING_82542_RA       0x00040
#define BP_ADDRESSING_82542_ICR      BP_ADDRESSING_ICR
#define BP_ADDRESSING_82542_ITR      BP_ADDRESSING_ITR
#define BP_ADDRESSING_82542_ICS      BP_ADDRESSING_ICS
#define BP_ADDRESSING_82542_IMS      BP_ADDRESSING_IMS
#define BP_ADDRESSING_82542_IMC      BP_ADDRESSING_IMC
#define BP_ADDRESSING_82542_RCTL     BP_ADDRESSING_RCTL
#define BP_ADDRESSING_82542_RDTR     0x00108
#define BP_ADDRESSING_82542_RDBAL    0x00110
#define BP_ADDRESSING_82542_RDBAH    0x00114
#define BP_ADDRESSING_82542_RDLEN    0x00118
#define BP_ADDRESSING_82542_RDH      0x00120
#define BP_ADDRESSING_82542_RDT      0x00128
#define BP_ADDRESSING_82542_RDTR0    BP_ADDRESSING_82542_RDTR
#define BP_ADDRESSING_82542_RDBAL0   BP_ADDRESSING_82542_RDBAL
#define BP_ADDRESSING_82542_RDBAH0   BP_ADDRESSING_82542_RDBAH
#define BP_ADDRESSING_82542_RDLEN0   BP_ADDRESSING_82542_RDLEN
#define BP_ADDRESSING_82542_RDH0     BP_ADDRESSING_82542_RDH
#define BP_ADDRESSING_82542_RDT0     BP_ADDRESSING_82542_RDT
#define BP_ADDRESSING_82542_SRRCTL(_n) (0x280C + ((_n) << 8)) /* Split and Replication
                                                       * RX Control - RW */
#define BP_ADDRESSING_82542_DCA_RXCTRL(_n) (0x02814 + ((_n) << 8))
#define BP_ADDRESSING_82542_RDBAH3   0x02B04 /* RX Desc Base High Queue 3 - RW */
#define BP_ADDRESSING_82542_RDBAL3   0x02B00 /* RX Desc Low Queue 3 - RW */
#define BP_ADDRESSING_82542_RDLEN3   0x02B08 /* RX Desc Length Queue 3 - RW */
#define BP_ADDRESSING_82542_RDH3     0x02B10 /* RX Desc Head Queue 3 - RW */
#define BP_ADDRESSING_82542_RDT3     0x02B18 /* RX Desc Tail Queue 3 - RW */
#define BP_ADDRESSING_82542_RDBAL2   0x02A00 /* RX Desc Base Low Queue 2 - RW */
#define BP_ADDRESSING_82542_RDBAH2   0x02A04 /* RX Desc Base High Queue 2 - RW */
#define BP_ADDRESSING_82542_RDLEN2   0x02A08 /* RX Desc Length Queue 2 - RW */
#define BP_ADDRESSING_82542_RDH2     0x02A10 /* RX Desc Head Queue 2 - RW */
#define BP_ADDRESSING_82542_RDT2     0x02A18 /* RX Desc Tail Queue 2 - RW */
#define BP_ADDRESSING_82542_RDTR1    0x00130
#define BP_ADDRESSING_82542_RDBAL1   0x00138
#define BP_ADDRESSING_82542_RDBAH1   0x0013C
#define BP_ADDRESSING_82542_RDLEN1   0x00140
#define BP_ADDRESSING_82542_RDH1     0x00148
#define BP_ADDRESSING_82542_RDT1     0x00150
#define BP_ADDRESSING_82542_FCRTH    0x00160
#define BP_ADDRESSING_82542_FCRTL    0x00168
#define BP_ADDRESSING_82542_FCTTV    BP_ADDRESSING_FCTTV
#define BP_ADDRESSING_82542_TXCW     BP_ADDRESSING_TXCW
#define BP_ADDRESSING_82542_RXCW     BP_ADDRESSING_RXCW
#define BP_ADDRESSING_82542_MTA      0x00200
#define BP_ADDRESSING_82542_TCTL     BP_ADDRESSING_TCTL
#define BP_ADDRESSING_82542_TCTL_EXT BP_ADDRESSING_TCTL_EXT
#define BP_ADDRESSING_82542_TIPG     BP_ADDRESSING_TIPG
#define BP_ADDRESSING_82542_TDBAL    0x00420
#define BP_ADDRESSING_82542_TDBAH    0x00424
#define BP_ADDRESSING_82542_TDLEN    0x00428
#define BP_ADDRESSING_82542_TDH      0x00430
#define BP_ADDRESSING_82542_TDT      0x00438
#define BP_ADDRESSING_82542_TIDV     0x00440
#define BP_ADDRESSING_82542_TBT      BP_ADDRESSING_TBT
#define BP_ADDRESSING_82542_AIT      BP_ADDRESSING_AIT
#define BP_ADDRESSING_82542_VFTA     0x00600
#define BP_ADDRESSING_82542_LEDCTL   BP_ADDRESSING_LEDCTL
#define BP_ADDRESSING_82542_PBA      BP_ADDRESSING_PBA
#define BP_ADDRESSING_82542_PBS      BP_ADDRESSING_PBS
#define BP_ADDRESSING_82542_EEMNGCTL BP_ADDRESSING_EEMNGCTL
#define BP_ADDRESSING_82542_EEARBC   BP_ADDRESSING_EEARBC
#define BP_ADDRESSING_82542_FLASHT   BP_ADDRESSING_FLASHT
#define BP_ADDRESSING_82542_EEWR     BP_ADDRESSING_EEWR
#define BP_ADDRESSING_82542_FLSWCTL  BP_ADDRESSING_FLSWCTL
#define BP_ADDRESSING_82542_FLSWDATA BP_ADDRESSING_FLSWDATA
#define BP_ADDRESSING_82542_FLSWCNT  BP_ADDRESSING_FLSWCNT
#define BP_ADDRESSING_82542_FLOP     BP_ADDRESSING_FLOP
#define BP_ADDRESSING_82542_EXTCNF_CTRL  BP_ADDRESSING_EXTCNF_CTRL
#define BP_ADDRESSING_82542_EXTCNF_SIZE  BP_ADDRESSING_EXTCNF_SIZE
#define BP_ADDRESSING_82542_PHY_CTRL BP_ADDRESSING_PHY_CTRL
#define BP_ADDRESSING_82542_ERT      BP_ADDRESSING_ERT
#define BP_ADDRESSING_82542_RXDCTL   BP_ADDRESSING_RXDCTL
#define BP_ADDRESSING_82542_RXDCTL1  BP_ADDRESSING_RXDCTL1
#define BP_ADDRESSING_82542_RADV     BP_ADDRESSING_RADV
#define BP_ADDRESSING_82542_RSRPD    BP_ADDRESSING_RSRPD
#define BP_ADDRESSING_82542_TXDMAC   BP_ADDRESSING_TXDMAC
#define BP_ADDRESSING_82542_KABGTXD  BP_ADDRESSING_KABGTXD
#define BP_ADDRESSING_82542_TDFHS    BP_ADDRESSING_TDFHS
#define BP_ADDRESSING_82542_TDFTS    BP_ADDRESSING_TDFTS
#define BP_ADDRESSING_82542_TDFPC    BP_ADDRESSING_TDFPC
#define BP_ADDRESSING_82542_TXDCTL   BP_ADDRESSING_TXDCTL
#define BP_ADDRESSING_82542_TADV     BP_ADDRESSING_TADV
#define BP_ADDRESSING_82542_TSPMT    BP_ADDRESSING_TSPMT
#define BP_ADDRESSING_82542_CRCERRS  BP_ADDRESSING_CRCERRS
#define BP_ADDRESSING_82542_ALGNERRC BP_ADDRESSING_ALGNERRC
#define BP_ADDRESSING_82542_SYMERRS  BP_ADDRESSING_SYMERRS
#define BP_ADDRESSING_82542_RXERRC   BP_ADDRESSING_RXERRC
#define BP_ADDRESSING_82542_MPC      BP_ADDRESSING_MPC
#define BP_ADDRESSING_82542_SCC      BP_ADDRESSING_SCC
#define BP_ADDRESSING_82542_ECOL     BP_ADDRESSING_ECOL
#define BP_ADDRESSING_82542_MCC      BP_ADDRESSING_MCC
#define BP_ADDRESSING_82542_LATECOL  BP_ADDRESSING_LATECOL
#define BP_ADDRESSING_82542_COLC     BP_ADDRESSING_COLC
#define BP_ADDRESSING_82542_DC       BP_ADDRESSING_DC
#define BP_ADDRESSING_82542_TNCRS    BP_ADDRESSING_TNCRS
#define BP_ADDRESSING_82542_SEC      BP_ADDRESSING_SEC
#define BP_ADDRESSING_82542_CEXTERR  BP_ADDRESSING_CEXTERR
#define BP_ADDRESSING_82542_RLEC     BP_ADDRESSING_RLEC
#define BP_ADDRESSING_82542_XONRXC   BP_ADDRESSING_XONRXC
#define BP_ADDRESSING_82542_XONTXC   BP_ADDRESSING_XONTXC
#define BP_ADDRESSING_82542_XOFFRXC  BP_ADDRESSING_XOFFRXC
#define BP_ADDRESSING_82542_XOFFTXC  BP_ADDRESSING_XOFFTXC
#define BP_ADDRESSING_82542_FCRUC    BP_ADDRESSING_FCRUC
#define BP_ADDRESSING_82542_PRC64    BP_ADDRESSING_PRC64
#define BP_ADDRESSING_82542_PRC127   BP_ADDRESSING_PRC127
#define BP_ADDRESSING_82542_PRC255   BP_ADDRESSING_PRC255
#define BP_ADDRESSING_82542_PRC511   BP_ADDRESSING_PRC511
#define BP_ADDRESSING_82542_PRC1023  BP_ADDRESSING_PRC1023
#define BP_ADDRESSING_82542_PRC1522  BP_ADDRESSING_PRC1522
#define BP_ADDRESSING_82542_GPRC     BP_ADDRESSING_GPRC
#define BP_ADDRESSING_82542_BPRC     BP_ADDRESSING_BPRC
#define BP_ADDRESSING_82542_MPRC     BP_ADDRESSING_MPRC
#define BP_ADDRESSING_82542_GPTC     BP_ADDRESSING_GPTC
#define BP_ADDRESSING_82542_GORCL    BP_ADDRESSING_GORCL
#define BP_ADDRESSING_82542_GORCH    BP_ADDRESSING_GORCH
#define BP_ADDRESSING_82542_GOTCL    BP_ADDRESSING_GOTCL
#define BP_ADDRESSING_82542_GOTCH    BP_ADDRESSING_GOTCH
#define BP_ADDRESSING_82542_RNBC     BP_ADDRESSING_RNBC
#define BP_ADDRESSING_82542_RUC      BP_ADDRESSING_RUC
#define BP_ADDRESSING_82542_RFC      BP_ADDRESSING_RFC
#define BP_ADDRESSING_82542_ROC      BP_ADDRESSING_ROC
#define BP_ADDRESSING_82542_RJC      BP_ADDRESSING_RJC
#define BP_ADDRESSING_82542_MGTPRC   BP_ADDRESSING_MGTPRC
#define BP_ADDRESSING_82542_MGTPDC   BP_ADDRESSING_MGTPDC
#define BP_ADDRESSING_82542_MGTPTC   BP_ADDRESSING_MGTPTC
#define BP_ADDRESSING_82542_TORL     BP_ADDRESSING_TORL
#define BP_ADDRESSING_82542_TORH     BP_ADDRESSING_TORH
#define BP_ADDRESSING_82542_TOTL     BP_ADDRESSING_TOTL
#define BP_ADDRESSING_82542_TOTH     BP_ADDRESSING_TOTH
#define BP_ADDRESSING_82542_TPR      BP_ADDRESSING_TPR
#define BP_ADDRESSING_82542_TPT      BP_ADDRESSING_TPT
#define BP_ADDRESSING_82542_PTC64    BP_ADDRESSING_PTC64
#define BP_ADDRESSING_82542_PTC127   BP_ADDRESSING_PTC127
#define BP_ADDRESSING_82542_PTC255   BP_ADDRESSING_PTC255
#define BP_ADDRESSING_82542_PTC511   BP_ADDRESSING_PTC511
#define BP_ADDRESSING_82542_PTC1023  BP_ADDRESSING_PTC1023
#define BP_ADDRESSING_82542_PTC1522  BP_ADDRESSING_PTC1522
#define BP_ADDRESSING_82542_MPTC     BP_ADDRESSING_MPTC
#define BP_ADDRESSING_82542_BPTC     BP_ADDRESSING_BPTC
#define BP_ADDRESSING_82542_TSCTC    BP_ADDRESSING_TSCTC
#define BP_ADDRESSING_82542_TSCTFC   BP_ADDRESSING_TSCTFC
#define BP_ADDRESSING_82542_RXCSUM   BP_ADDRESSING_RXCSUM
#define BP_ADDRESSING_82542_WUC      BP_ADDRESSING_WUC
#define BP_ADDRESSING_82542_WUFC     BP_ADDRESSING_WUFC
#define BP_ADDRESSING_82542_WUS      BP_ADDRESSING_WUS
#define BP_ADDRESSING_82542_MANC     BP_ADDRESSING_MANC
#define BP_ADDRESSING_82542_IPAV     BP_ADDRESSING_IPAV
#define BP_ADDRESSING_82542_IP4AT    BP_ADDRESSING_IP4AT
#define BP_ADDRESSING_82542_IP6AT    BP_ADDRESSING_IP6AT
#define BP_ADDRESSING_82542_WUPL     BP_ADDRESSING_WUPL
#define BP_ADDRESSING_82542_WUPM     BP_ADDRESSING_WUPM
#define BP_ADDRESSING_82542_FFLT     BP_ADDRESSING_FFLT
#define BP_ADDRESSING_82542_TDFH     0x08010
#define BP_ADDRESSING_82542_TDFT     0x08018
#define BP_ADDRESSING_82542_FFMT     BP_ADDRESSING_FFMT
#define BP_ADDRESSING_82542_FFVT     BP_ADDRESSING_FFVT
#define BP_ADDRESSING_82542_HOST_IF  BP_ADDRESSING_HOST_IF
#define BP_ADDRESSING_82542_IAM         BP_ADDRESSING_IAM
#define BP_ADDRESSING_82542_EEMNGCTL    BP_ADDRESSING_EEMNGCTL
#define BP_ADDRESSING_82542_PSRCTL      BP_ADDRESSING_PSRCTL
#define BP_ADDRESSING_82542_RAID        BP_ADDRESSING_RAID
#define BP_ADDRESSING_82542_TARC0       BP_ADDRESSING_TARC0
#define BP_ADDRESSING_82542_TDBAL1      BP_ADDRESSING_TDBAL1
#define BP_ADDRESSING_82542_TDBAH1      BP_ADDRESSING_TDBAH1
#define BP_ADDRESSING_82542_TDLEN1      BP_ADDRESSING_TDLEN1
#define BP_ADDRESSING_82542_TDH1        BP_ADDRESSING_TDH1
#define BP_ADDRESSING_82542_TDT1        BP_ADDRESSING_TDT1
#define BP_ADDRESSING_82542_TXDCTL1     BP_ADDRESSING_TXDCTL1
#define BP_ADDRESSING_82542_TARC1       BP_ADDRESSING_TARC1
#define BP_ADDRESSING_82542_RFCTL       BP_ADDRESSING_RFCTL
#define BP_ADDRESSING_82542_GCR         BP_ADDRESSING_GCR
#define BP_ADDRESSING_82542_GSCL_1      BP_ADDRESSING_GSCL_1
#define BP_ADDRESSING_82542_GSCL_2      BP_ADDRESSING_GSCL_2
#define BP_ADDRESSING_82542_GSCL_3      BP_ADDRESSING_GSCL_3
#define BP_ADDRESSING_82542_GSCL_4      BP_ADDRESSING_GSCL_4
#define BP_ADDRESSING_82542_FACTPS      BP_ADDRESSING_FACTPS
#define BP_ADDRESSING_82542_SWSM        BP_ADDRESSING_SWSM
#define BP_ADDRESSING_82542_FWSM        BP_ADDRESSING_FWSM
#define BP_ADDRESSING_82542_FFLT_DBG    BP_ADDRESSING_FFLT_DBG
#define BP_ADDRESSING_82542_IAC         BP_ADDRESSING_IAC
#define BP_ADDRESSING_82542_ICRXPTC     BP_ADDRESSING_ICRXPTC
#define BP_ADDRESSING_82542_ICRXATC     BP_ADDRESSING_ICRXATC
#define BP_ADDRESSING_82542_ICTXPTC     BP_ADDRESSING_ICTXPTC
#define BP_ADDRESSING_82542_ICTXATC     BP_ADDRESSING_ICTXATC
#define BP_ADDRESSING_82542_ICTXQEC     BP_ADDRESSING_ICTXQEC
#define BP_ADDRESSING_82542_ICTXQMTC    BP_ADDRESSING_ICTXQMTC
#define BP_ADDRESSING_82542_ICRXDMTC    BP_ADDRESSING_ICRXDMTC
#define BP_ADDRESSING_82542_ICRXOC      BP_ADDRESSING_ICRXOC
#define BP_ADDRESSING_82542_HICR        BP_ADDRESSING_HICR

#define BP_ADDRESSING_82542_CPUVEC      BP_ADDRESSING_CPUVEC
#define BP_ADDRESSING_82542_MRQC        BP_ADDRESSING_MRQC
#define BP_ADDRESSING_82542_RETA        BP_ADDRESSING_RETA
#define BP_ADDRESSING_82542_RSSRK       BP_ADDRESSING_RSSRK
#define BP_ADDRESSING_82542_RSSIM       BP_ADDRESSING_RSSIM
#define BP_ADDRESSING_82542_RSSIR       BP_ADDRESSING_RSSIR
#define BP_ADDRESSING_82542_KUMCTRLSTA BP_ADDRESSING_KUMCTRLSTA
#define BP_ADDRESSING_82542_SW_FW_SYNC BP_ADDRESSING_SW_FW_SYNC

/* Statistics counters collected by the MAC */
struct bp_addressing_hw_stats {
    uint64_t crcerrs;
    uint64_t algnerrc;
    uint64_t symerrs;
    uint64_t rxerrc;
    uint64_t mpc;
    uint64_t scc;
    uint64_t ecol;
    uint64_t mcc;
    uint64_t latecol;
    uint64_t colc;
    uint64_t dc;
    uint64_t tncrs;
    uint64_t sec;
    uint64_t cexterr;
    uint64_t rlec;
    uint64_t xonrxc;
    uint64_t xontxc;
    uint64_t xoffrxc;
    uint64_t xofftxc;
    uint64_t fcruc;
    uint64_t prc64;
    uint64_t prc127;
    uint64_t prc255;
    uint64_t prc511;
    uint64_t prc1023;
    uint64_t prc1522;
    uint64_t gprc;
    uint64_t bprc;
    uint64_t mprc;
    uint64_t gptc;
    uint64_t gorcl;
    uint64_t gorch;
    uint64_t gotcl;
    uint64_t gotch;
    uint64_t rnbc;
    uint64_t ruc;
    uint64_t rfc;
    uint64_t roc;
    uint64_t rjc;
    uint64_t mgprc;
    uint64_t mgpdc;
    uint64_t mgptc;
    uint64_t torl;
    uint64_t torh;
    uint64_t totl;
    uint64_t toth;
    uint64_t tpr;
    uint64_t tpt;
    uint64_t ptc64;
    uint64_t ptc127;
    uint64_t ptc255;
    uint64_t ptc511;
    uint64_t ptc1023;
    uint64_t ptc1522;
    uint64_t mptc;
    uint64_t bptc;
    uint64_t tsctc;
    uint64_t tsctfc;
    uint64_t iac;
    uint64_t icrxptc;
    uint64_t icrxatc;
    uint64_t ictxptc;
    uint64_t ictxatc;
    uint64_t ictxqec;
    uint64_t ictxqmtc;
    uint64_t icrxdmtc;
    uint64_t icrxoc;
};

/* Structure containing variables used by the shared code (bp_addressing_hw.c) */
struct bp_addressing_hw {
    uint8_t *hw_addr;
    uint8_t *flash_address;
    bp_addressing_mac_type mac_type;
    bp_addressing_phy_type phy_type;
    uint32_t phy_init_script;
    bp_addressing_media_type media_type;
    void *back;
    struct bp_addressing_shadow_ram *eeprom_shadow_ram;
    uint32_t flash_bank_size;
    uint32_t flash_base_addr;
    bp_addressing_fc_type fc;
    bp_addressing_bus_speed bus_speed;
    bp_addressing_bus_width bus_width;
    bp_addressing_bus_type bus_type;
    struct bp_addressing_eeprom_info eeprom;
    bp_addressing_ms_type master_slave;
    bp_addressing_ms_type original_master_slave;
    bp_addressing_ffe_config ffe_config_state;
    uint32_t asf_firmware_present;
    uint32_t eeprom_semaphore_present;
    uint32_t swfw_sync_present;
    uint32_t swfwhw_semaphore_present;
    unsigned long io_base;
    uint32_t phy_id;
    uint32_t phy_revision;
    uint32_t phy_addr;
    uint32_t original_fc;
    uint32_t txcw;
    uint32_t autoneg_failed;
    uint32_t max_frame_size;
    uint32_t min_frame_size;
    uint32_t mc_filter_type;
    uint32_t num_mc_addrs;
    uint32_t collision_delta;
    uint32_t tx_packet_delta;
    uint32_t ledctl_default;
    uint32_t ledctl_mode1;
    uint32_t ledctl_mode2;
    boolean_t tx_pkt_filtering;
    struct bp_addressing_host_mng_dhcp_cookie mng_cookie;
    uint16_t phy_spd_default;
    uint16_t autoneg_advertised;
    uint16_t pci_cmd_word;
    uint16_t fc_high_water;
    uint16_t fc_low_water;
    uint16_t fc_pause_time;
    uint16_t current_ifs_val;
    uint16_t ifs_min_val;
    uint16_t ifs_max_val;
    uint16_t ifs_step_size;
    uint16_t ifs_ratio;
    uint16_t device_id;
    uint16_t vendor_id;
    uint16_t subsystem_id;
    uint16_t subsystem_vendor_id;
    uint8_t revision_id;
    uint8_t autoneg;
    uint8_t mdix;
    uint8_t forced_speed_duplex;
    uint8_t wait_autoneg_complete;
    uint8_t dma_fairness;
    uint8_t mac_addr[NODE_ADDRESS_SIZE];
    uint8_t perm_mac_addr[NODE_ADDRESS_SIZE];
    boolean_t disable_polarity_correction;
    boolean_t speed_downgraded;
    bp_addressing_smart_speed smart_speed;
    bp_addressing_dsp_config dsp_config_state;
    boolean_t get_link_status;
    boolean_t serdes_link_down;
    boolean_t tbi_compatibility_en;
    boolean_t tbi_compatibility_on;
    boolean_t laa_is_present;
    boolean_t phy_reset_disable;
    boolean_t fc_send_xon;
    boolean_t fc_strict_ieee;
    boolean_t report_tx_early;
    boolean_t adaptive_ifs;
    boolean_t ifs_params_forced;
    boolean_t in_ifs_mode;
    boolean_t mng_reg_access_disabled;
    boolean_t leave_av_bit_off;
    boolean_t kmrn_lock_loss_workaround_disabled;
};


#define BP_ADDRESSING_EEPROM_SWDPIN0   0x0001   /* SWDPIN 0 EEPROM Value */
#define BP_ADDRESSING_EEPROM_LED_LOGIC 0x0020   /* Led Logic Word */
#define BP_ADDRESSING_EEPROM_RW_REG_DATA   16   /* Offset to data in EEPROM read/write registers */
#define BP_ADDRESSING_EEPROM_RW_REG_DONE   2    /* Offset to READ/WRITE done bit */
#define BP_ADDRESSING_EEPROM_RW_REG_START  1    /* First bit for telling part to start operation */
#define BP_ADDRESSING_EEPROM_RW_ADDR_SHIFT 2    /* Shift to the address bits */
#define BP_ADDRESSING_EEPROM_POLL_WRITE    1    /* Flag for polling for write complete */
#define BP_ADDRESSING_EEPROM_POLL_READ     0    /* Flag for polling for read complete */
/* Register Bit Masks */
/* Device Control */
#define BP_ADDRESSING_CTRL_FD       0x00000001  /* Full duplex.0=half; 1=full */
#define BP_ADDRESSING_CTRL_BEM      0x00000002  /* Endian Mode.0=little,1=big */
#define BP_ADDRESSING_CTRL_PRIOR    0x00000004  /* Priority on PCI. 0=rx,1=fair */
#define BP_ADDRESSING_CTRL_GIO_MASTER_DISABLE 0x00000004 /*Blocks new Master requests */
#define BP_ADDRESSING_CTRL_LRST     0x00000008  /* Link reset. 0=normal,1=reset */
#define BP_ADDRESSING_CTRL_TME      0x00000010  /* Test mode. 0=normal,1=test */
#define BP_ADDRESSING_CTRL_SLE      0x00000020  /* Serial Link on 0=dis,1=en */
#define BP_ADDRESSING_CTRL_ASDE     0x00000020  /* Auto-speed detect enable */
#define BP_ADDRESSING_CTRL_SLU      0x00000040  /* Set link up (Force Link) */
#define BP_ADDRESSING_CTRL_ILOS     0x00000080  /* Invert Loss-Of Signal */
#define BP_ADDRESSING_CTRL_SPD_SEL  0x00000300  /* Speed Select Mask */
#define BP_ADDRESSING_CTRL_SPD_10   0x00000000  /* Force 10Mb */
#define BP_ADDRESSING_CTRL_SPD_100  0x00000100  /* Force 100Mb */
#define BP_ADDRESSING_CTRL_SPD_1000 0x00000200  /* Force 1Gb */
#define BP_ADDRESSING_CTRL_BEM32    0x00000400  /* Big Endian 32 mode */
#define BP_ADDRESSING_CTRL_FRCSPD   0x00000800  /* Force Speed */
#define BP_ADDRESSING_CTRL_FRCDPX   0x00001000  /* Force Duplex */
#define BP_ADDRESSING_CTRL_D_UD_EN  0x00002000  /* Dock/Undock enable */
#define BP_ADDRESSING_CTRL_D_UD_POLARITY 0x00004000 /* Defined polarity of Dock/Undock indication in SDP[0] */
#define BP_ADDRESSING_CTRL_FORCE_PHY_RESET 0x00008000 /* Reset both PHY ports, through PHYRST_N pin */
#define BP_ADDRESSING_CTRL_EXT_LINK_EN 0x00010000 /* enable link status from external LINK_0 and LINK_1 pins */
#define BP_ADDRESSING_CTRL_SWDPIN0  0x00040000  /* SWDPIN 0 value */
#define BP_ADDRESSING_CTRL_SWDPIN1  0x00080000  /* SWDPIN 1 value */
#define BP_ADDRESSING_CTRL_SWDPIN2  0x00100000  /* SWDPIN 2 value */
#define BP_ADDRESSING_CTRL_SWDPIN3  0x00200000  /* SWDPIN 3 value */
#define BP_ADDRESSING_CTRL_SWDPIO0  0x00400000  /* SWDPIN 0 Input or output */
#define BP_ADDRESSING_CTRL_SWDPIO1  0x00800000  /* SWDPIN 1 input or output */
#define BP_ADDRESSING_CTRL_SWDPIO2  0x01000000  /* SWDPIN 2 input or output */
#define BP_ADDRESSING_CTRL_SWDPIO3  0x02000000  /* SWDPIN 3 input or output */
#define BP_ADDRESSING_CTRL_RST      0x04000000  /* Global reset */
#define BP_ADDRESSING_CTRL_RFCE     0x08000000  /* Receive Flow Control enable */
#define BP_ADDRESSING_CTRL_TFCE     0x10000000  /* Transmit flow control enable */
#define BP_ADDRESSING_CTRL_RTE      0x20000000  /* Routing tag enable */
#define BP_ADDRESSING_CTRL_VME      0x40000000  /* IEEE VLAN mode enable */
#define BP_ADDRESSING_CTRL_PHY_RST  0x80000000  /* PHY Reset */
#define BP_ADDRESSING_CTRL_SW2FW_INT 0x02000000  /* Initiate an interrupt to manageability engine */

/* Device Status */
#define BP_ADDRESSING_STATUS_FD         0x00000001      /* Full duplex.0=half,1=full */
#define BP_ADDRESSING_STATUS_LU         0x00000002      /* Link up.0=no,1=link */
#define BP_ADDRESSING_STATUS_FUNC_MASK  0x0000000C      /* PCI Function Mask */
#define BP_ADDRESSING_STATUS_FUNC_SHIFT 2
#define BP_ADDRESSING_STATUS_FUNC_0     0x00000000      /* Function 0 */
#define BP_ADDRESSING_STATUS_FUNC_1     0x00000004      /* Function 1 */
#define BP_ADDRESSING_STATUS_TXOFF      0x00000010      /* transmission paused */
#define BP_ADDRESSING_STATUS_TBIMODE    0x00000020      /* TBI mode */
#define BP_ADDRESSING_STATUS_SPEED_MASK 0x000000C0
#define BP_ADDRESSING_STATUS_SPEED_10   0x00000000      /* Speed 10Mb/s */
#define BP_ADDRESSING_STATUS_SPEED_100  0x00000040      /* Speed 100Mb/s */
#define BP_ADDRESSING_STATUS_SPEED_1000 0x00000080      /* Speed 1000Mb/s */
#define BP_ADDRESSING_STATUS_LAN_INIT_DONE 0x00000200   /* Lan Init Completion
                                                   by EEPROM/Flash */
#define BP_ADDRESSING_STATUS_ASDV       0x00000300      /* Auto speed detect value */
#define BP_ADDRESSING_STATUS_DOCK_CI    0x00000800      /* Change in Dock/Undock state. Clear on write '0'. */
#define BP_ADDRESSING_STATUS_GIO_MASTER_ENABLE 0x00080000 /* Status of Master requests. */
#define BP_ADDRESSING_STATUS_MTXCKOK    0x00000400      /* MTX clock running OK */
#define BP_ADDRESSING_STATUS_PCI66      0x00000800      /* In 66Mhz slot */
#define BP_ADDRESSING_STATUS_BUS64      0x00001000      /* In 64 bit slot */
#define BP_ADDRESSING_STATUS_PCIX_MODE  0x00002000      /* PCI-X mode */
#define BP_ADDRESSING_STATUS_PCIX_SPEED 0x0000C000      /* PCI-X bus speed */
#define BP_ADDRESSING_STATUS_BMC_SKU_0  0x00100000 /* BMC USB redirect disabled */
#define BP_ADDRESSING_STATUS_BMC_SKU_1  0x00200000 /* BMC SRAM disabled */
#define BP_ADDRESSING_STATUS_BMC_SKU_2  0x00400000 /* BMC SDRAM disabled */
#define BP_ADDRESSING_STATUS_BMC_CRYPTO 0x00800000 /* BMC crypto disabled */
#define BP_ADDRESSING_STATUS_BMC_LITE   0x01000000 /* BMC external code execution disabled */
#define BP_ADDRESSING_STATUS_RGMII_ENABLE 0x02000000 /* RGMII disabled */
#define BP_ADDRESSING_STATUS_FUSE_8       0x04000000
#define BP_ADDRESSING_STATUS_FUSE_9       0x08000000
#define BP_ADDRESSING_STATUS_SERDES0_DIS  0x10000000 /* SERDES disabled on port 0 */
#define BP_ADDRESSING_STATUS_SERDES1_DIS  0x20000000 /* SERDES disabled on port 1 */

/* Constants used to intrepret the masked PCI-X bus speed. */
#define BP_ADDRESSING_STATUS_PCIX_SPEED_66  0x00000000 /* PCI-X bus speed  50-66 MHz */
#define BP_ADDRESSING_STATUS_PCIX_SPEED_100 0x00004000 /* PCI-X bus speed  66-100 MHz */
#define BP_ADDRESSING_STATUS_PCIX_SPEED_133 0x00008000 /* PCI-X bus speed 100-133 MHz */

/* EEPROM/Flash Control */
#define BP_ADDRESSING_EECD_SK        0x00000001 /* EEPROM Clock */
#define BP_ADDRESSING_EECD_CS        0x00000002 /* EEPROM Chip Select */
#define BP_ADDRESSING_EECD_DI        0x00000004 /* EEPROM Data In */
#define BP_ADDRESSING_EECD_DO        0x00000008 /* EEPROM Data Out */
#define BP_ADDRESSING_EECD_FWE_MASK  0x00000030
#define BP_ADDRESSING_EECD_FWE_DIS   0x00000010 /* Disable FLASH writes */
#define BP_ADDRESSING_EECD_FWE_EN    0x00000020 /* Enable FLASH writes */
#define BP_ADDRESSING_EECD_FWE_SHIFT 4
#define BP_ADDRESSING_EECD_REQ       0x00000040 /* EEPROM Access Request */
#define BP_ADDRESSING_EECD_GNT       0x00000080 /* EEPROM Access Grant */
#define BP_ADDRESSING_EECD_PRES      0x00000100 /* EEPROM Present */
#define BP_ADDRESSING_EECD_SIZE      0x00000200 /* EEPROM Size (0=64 word 1=256 word) */
#define BP_ADDRESSING_EECD_ADDR_BITS 0x00000400 /* EEPROM Addressing bits based on type
                                         * (0-small, 1-large) */
#define BP_ADDRESSING_EECD_TYPE      0x00002000 /* EEPROM Type (1-SPI, 0-Microwire) */
#ifndef BP_ADDRESSING_EEPROM_GRANT_ATTEMPTS
#define BP_ADDRESSING_EEPROM_GRANT_ATTEMPTS 1000 /* EEPROM # attempts to gain grant */
#endif
#define BP_ADDRESSING_EECD_AUTO_RD          0x00000200  /* EEPROM Auto Read done */
#define BP_ADDRESSING_EECD_SIZE_EX_MASK     0x00007800  /* EEprom Size */
#define BP_ADDRESSING_EECD_SIZE_EX_SHIFT    11
#define BP_ADDRESSING_EECD_NVADDS    0x00018000 /* NVM Address Size */
#define BP_ADDRESSING_EECD_SELSHAD   0x00020000 /* Select Shadow RAM */
#define BP_ADDRESSING_EECD_INITSRAM  0x00040000 /* Initialize Shadow RAM */
#define BP_ADDRESSING_EECD_FLUPD     0x00080000 /* Update FLASH */
#define BP_ADDRESSING_EECD_AUPDEN    0x00100000 /* Enable Autonomous FLASH update */
#define BP_ADDRESSING_EECD_SHADV     0x00200000 /* Shadow RAM Data Valid */
#define BP_ADDRESSING_EECD_SEC1VAL   0x00400000 /* Sector One Valid */
#define BP_ADDRESSING_EECD_SECVAL_SHIFT      22
#define BP_ADDRESSING_STM_OPCODE     0xDB00
#define BP_ADDRESSING_HICR_FW_RESET  0xC0

#define BP_ADDRESSING_SHADOW_RAM_WORDS     2048
#define BP_ADDRESSING_ICH8_NVM_SIG_WORD    0x13
#define BP_ADDRESSING_ICH8_NVM_SIG_MASK    0xC0

/* EEPROM Read */
#define BP_ADDRESSING_EERD_START      0x00000001 /* Start Read */
#define BP_ADDRESSING_EERD_DONE       0x00000010 /* Read Done */
#define BP_ADDRESSING_EERD_ADDR_SHIFT 8
#define BP_ADDRESSING_EERD_ADDR_MASK  0x0000FF00 /* Read Address */
#define BP_ADDRESSING_EERD_DATA_SHIFT 16
#define BP_ADDRESSING_EERD_DATA_MASK  0xFFFF0000 /* Read Data */

/* SPI EEPROM Status Register */
#define EEPROM_STATUS_RDY_SPI  0x01
#define EEPROM_STATUS_WEN_SPI  0x02
#define EEPROM_STATUS_BP0_SPI  0x04
#define EEPROM_STATUS_BP1_SPI  0x08
#define EEPROM_STATUS_WPEN_SPI 0x80

/* Extended Device Control */
#define BP_ADDRESSING_CTRL_EXT_GPI0_EN   0x00000001 /* Maps SDP4 to GPI0 */
#define BP_ADDRESSING_CTRL_EXT_GPI1_EN   0x00000002 /* Maps SDP5 to GPI1 */
#define BP_ADDRESSING_CTRL_EXT_PHYINT_EN BP_ADDRESSING_CTRL_EXT_GPI1_EN
#define BP_ADDRESSING_CTRL_EXT_GPI2_EN   0x00000004 /* Maps SDP6 to GPI2 */
#define BP_ADDRESSING_CTRL_EXT_GPI3_EN   0x00000008 /* Maps SDP7 to GPI3 */
#define BP_ADDRESSING_CTRL_EXT_SDP4_DATA 0x00000010 /* Value of SW Defineable Pin 4 */
#define BP_ADDRESSING_CTRL_EXT_SDP5_DATA 0x00000020 /* Value of SW Defineable Pin 5 */
#define BP_ADDRESSING_CTRL_EXT_PHY_INT   BP_ADDRESSING_CTRL_EXT_SDP5_DATA
#define BP_ADDRESSING_CTRL_EXT_SDP6_DATA 0x00000040 /* Value of SW Defineable Pin 6 */
#define BP_ADDRESSING_CTRL_EXT_SDP7_DATA 0x00000080 /* Value of SW Defineable Pin 7 */
#define BP_ADDRESSING_CTRL_EXT_SDP4_DIR  0x00000100 /* Direction of SDP4 0=in 1=out */
#define BP_ADDRESSING_CTRL_EXT_SDP5_DIR  0x00000200 /* Direction of SDP5 0=in 1=out */
#define BP_ADDRESSING_CTRL_EXT_SDP6_DIR  0x00000400 /* Direction of SDP6 0=in 1=out */
#define BP_ADDRESSING_CTRL_EXT_SDP7_DIR  0x00000800 /* Direction of SDP7 0=in 1=out */
#define BP_ADDRESSING_CTRL_EXT_ASDCHK    0x00001000 /* Initiate an ASD sequence */
#define BP_ADDRESSING_CTRL_EXT_EE_RST    0x00002000 /* Reinitialize from EEPROM */
#define BP_ADDRESSING_CTRL_EXT_IPS       0x00004000 /* Invert Power State */
#define BP_ADDRESSING_CTRL_EXT_SPD_BYPS  0x00008000 /* Speed Select Bypass */
#define BP_ADDRESSING_CTRL_EXT_RO_DIS    0x00020000 /* Relaxed Ordering disable */
#define BP_ADDRESSING_CTRL_EXT_LINK_MODE_MASK 0x00C00000
#define BP_ADDRESSING_CTRL_EXT_LINK_MODE_GMII 0x00000000
#define BP_ADDRESSING_CTRL_EXT_LINK_MODE_TBI  0x00C00000
#define BP_ADDRESSING_CTRL_EXT_LINK_MODE_KMRN    0x00000000
#define BP_ADDRESSING_CTRL_EXT_LINK_MODE_SERDES  0x00C00000
#define BP_ADDRESSING_CTRL_EXT_WR_WMARK_MASK  0x03000000
#define BP_ADDRESSING_CTRL_EXT_WR_WMARK_256   0x00000000
#define BP_ADDRESSING_CTRL_EXT_WR_WMARK_320   0x01000000
#define BP_ADDRESSING_CTRL_EXT_WR_WMARK_384   0x02000000
#define BP_ADDRESSING_CTRL_EXT_WR_WMARK_448   0x03000000
#define BP_ADDRESSING_CTRL_EXT_DRV_LOAD       0x10000000  /* Driver loaded bit for FW */
#define BP_ADDRESSING_CTRL_EXT_IAME           0x08000000  /* Interrupt acknowledge Auto-mask */
#define BP_ADDRESSING_CTRL_EXT_INT_TIMER_CLR  0x20000000  /* Clear Interrupt timers after IMS clear */
#define BP_ADDRESSING_CRTL_EXT_PB_PAREN       0x01000000 /* packet buffer parity error detection enabled */
#define BP_ADDRESSING_CTRL_EXT_DF_PAREN       0x02000000 /* descriptor FIFO parity error detection enable */
#define BP_ADDRESSING_CTRL_EXT_GHOST_PAREN    0x40000000

/* MDI Control */
#define BP_ADDRESSING_MDIC_DATA_MASK 0x0000FFFF
#define BP_ADDRESSING_MDIC_REG_MASK  0x001F0000
#define BP_ADDRESSING_MDIC_REG_SHIFT 16
#define BP_ADDRESSING_MDIC_PHY_MASK  0x03E00000
#define BP_ADDRESSING_MDIC_PHY_SHIFT 21
#define BP_ADDRESSING_MDIC_OP_WRITE  0x04000000
#define BP_ADDRESSING_MDIC_OP_READ   0x08000000
#define BP_ADDRESSING_MDIC_READY     0x10000000
#define BP_ADDRESSING_MDIC_INT_EN    0x20000000
#define BP_ADDRESSING_MDIC_ERROR     0x40000000

#define BP_ADDRESSING_KUMCTRLSTA_MASK           0x0000FFFF
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET         0x001F0000
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_SHIFT   16
#define BP_ADDRESSING_KUMCTRLSTA_REN            0x00200000

#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_FIFO_CTRL      0x00000000
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_CTRL           0x00000001
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_INB_CTRL       0x00000002
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_DIAG           0x00000003
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_TIMEOUTS       0x00000004
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_INB_PARAM      0x00000009
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_HD_CTRL        0x00000010
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_M2P_SERDES     0x0000001E
#define BP_ADDRESSING_KUMCTRLSTA_OFFSET_M2P_MODES      0x0000001F

/* Use byte values for the following shift parameters
 * Usage:
 *     psrctl |= (((ROUNDUP(value0, 128) >> BP_ADDRESSING_PSRCTL_BSIZE0_SHIFT) &
 *                  BP_ADDRESSING_PSRCTL_BSIZE0_MASK) |
 *                ((ROUNDUP(value1, 1024) >> BP_ADDRESSING_PSRCTL_BSIZE1_SHIFT) &
 *                  BP_ADDRESSING_PSRCTL_BSIZE1_MASK) |
 *                ((ROUNDUP(value2, 1024) << BP_ADDRESSING_PSRCTL_BSIZE2_SHIFT) &
 *                  BP_ADDRESSING_PSRCTL_BSIZE2_MASK) |
 *                ((ROUNDUP(value3, 1024) << BP_ADDRESSING_PSRCTL_BSIZE3_SHIFT) |;
 *                  BP_ADDRESSING_PSRCTL_BSIZE3_MASK))
 * where value0 = [128..16256],  default=256
 *       value1 = [1024..64512], default=4096
 *       value2 = [0..64512],    default=4096
 *       value3 = [0..64512],    default=0
 */

#define BP_ADDRESSING_PSRCTL_BSIZE0_MASK   0x0000007F
#define BP_ADDRESSING_PSRCTL_BSIZE1_MASK   0x00003F00
#define BP_ADDRESSING_PSRCTL_BSIZE2_MASK   0x003F0000
#define BP_ADDRESSING_PSRCTL_BSIZE3_MASK   0x3F000000

#define BP_ADDRESSING_PSRCTL_BSIZE0_SHIFT  7            /* Shift _right_ 7 */
#define BP_ADDRESSING_PSRCTL_BSIZE1_SHIFT  2            /* Shift _right_ 2 */
#define BP_ADDRESSING_PSRCTL_BSIZE2_SHIFT  6            /* Shift _left_ 6 */
#define BP_ADDRESSING_PSRCTL_BSIZE3_SHIFT 14            /* Shift _left_ 14 */

/* SW_W_SYNC definitions */
#define BP_ADDRESSING_SWFW_EEP_SM     0x0001
#define BP_ADDRESSING_SWFW_PHY0_SM    0x0002
#define BP_ADDRESSING_SWFW_PHY1_SM    0x0004
#define BP_ADDRESSING_SWFW_MAC_CSR_SM 0x0008

/* Wake Up Filter Control */
#define BP_ADDRESSING_WUFC_LNKC 0x00000001 /* Link Status Change Wakeup Enable */
#define BP_ADDRESSING_WUFC_MAG  0x00000002 /* Magic Packet Wakeup Enable */
#define BP_ADDRESSING_WUFC_EX   0x00000004 /* Directed Exact Wakeup Enable */
#define BP_ADDRESSING_WUFC_MC   0x00000008 /* Directed Multicast Wakeup Enable */
#define BP_ADDRESSING_WUFC_BC   0x00000010 /* Broadcast Wakeup Enable */
#define BP_ADDRESSING_WUFC_ARP  0x00000020 /* ARP Request Packet Wakeup Enable */
#define BP_ADDRESSING_WUFC_IPV4 0x00000040 /* Directed IPv4 Packet Wakeup Enable */
#define BP_ADDRESSING_WUFC_IPV6 0x00000080 /* Directed IPv6 Packet Wakeup Enable */
#define BP_ADDRESSING_WUFC_IGNORE_TCO      0x00008000 /* Ignore WakeOn TCO packets */
#define BP_ADDRESSING_WUFC_FLX0 0x00010000 /* Flexible Filter 0 Enable */
#define BP_ADDRESSING_WUFC_FLX1 0x00020000 /* Flexible Filter 1 Enable */
#define BP_ADDRESSING_WUFC_FLX2 0x00040000 /* Flexible Filter 2 Enable */
#define BP_ADDRESSING_WUFC_FLX3 0x00080000 /* Flexible Filter 3 Enable */
#define BP_ADDRESSING_WUFC_ALL_FILTERS 0x000F00FF /* Mask for all wakeup filters */
#define BP_ADDRESSING_WUFC_FLX_OFFSET 16       /* Offset to the Flexible Filters bits */
#define BP_ADDRESSING_WUFC_FLX_FILTERS 0x000F0000 /* Mask for the 4 flexible filters */

/* Management Control */
#define BP_ADDRESSING_MANC_SMBUS_EN      0x00000001 /* SMBus Enabled - RO */
#define BP_ADDRESSING_MANC_ASF_EN        0x00000002 /* ASF Enabled - RO */
#define BP_ADDRESSING_MANC_R_ON_FORCE    0x00000004 /* Reset on Force TCO - RO */
#define BP_ADDRESSING_MANC_RMCP_EN       0x00000100 /* Enable RCMP 026Fh Filtering */
#define BP_ADDRESSING_MANC_0298_EN       0x00000200 /* Enable RCMP 0298h Filtering */
#define BP_ADDRESSING_MANC_IPV4_EN       0x00000400 /* Enable IPv4 */
#define BP_ADDRESSING_MANC_IPV6_EN       0x00000800 /* Enable IPv6 */
#define BP_ADDRESSING_MANC_SNAP_EN       0x00001000 /* Accept LLC/SNAP */
#define BP_ADDRESSING_MANC_ARP_EN        0x00002000 /* Enable ARP Request Filtering */
#define BP_ADDRESSING_MANC_NEIGHBOR_EN   0x00004000 /* Enable Neighbor Discovery
                                             * Filtering */
#define BP_ADDRESSING_MANC_ARP_RES_EN    0x00008000 /* Enable ARP response Filtering */
#define BP_ADDRESSING_MANC_TCO_RESET     0x00010000 /* TCO Reset Occurred */
#define BP_ADDRESSING_MANC_RCV_TCO_EN    0x00020000 /* Receive TCO Packets Enabled */
#define BP_ADDRESSING_MANC_REPORT_STATUS 0x00040000 /* Status Reporting Enabled */
#define BP_ADDRESSING_MANC_RCV_ALL       0x00080000 /* Receive All Enabled */
#define BP_ADDRESSING_MANC_BLK_PHY_RST_ON_IDE   0x00040000 /* Block phy resets */
#define BP_ADDRESSING_MANC_EN_MAC_ADDR_FILTER   0x00100000 /* Enable MAC address
                                                    * filtering */
#define BP_ADDRESSING_MANC_EN_MNG2HOST   0x00200000 /* Enable MNG packets to host
                                             * memory */
#define BP_ADDRESSING_MANC_EN_IP_ADDR_FILTER    0x00400000 /* Enable IP address
                                                    * filtering */
#define BP_ADDRESSING_MANC_EN_XSUM_FILTER   0x00800000 /* Enable checksum filtering */
#define BP_ADDRESSING_MANC_BR_EN            0x01000000 /* Enable broadcast filtering */
#define BP_ADDRESSING_MANC_SMB_REQ       0x01000000 /* SMBus Request */
#define BP_ADDRESSING_MANC_SMB_GNT       0x02000000 /* SMBus Grant */
#define BP_ADDRESSING_MANC_SMB_CLK_IN    0x04000000 /* SMBus Clock In */
#define BP_ADDRESSING_MANC_SMB_DATA_IN   0x08000000 /* SMBus Data In */
#define BP_ADDRESSING_MANC_SMB_DATA_OUT  0x10000000 /* SMBus Data Out */
#define BP_ADDRESSING_MANC_SMB_CLK_OUT   0x20000000 /* SMBus Clock Out */

#define BP_ADDRESSING_MANC_SMB_DATA_OUT_SHIFT  28 /* SMBus Data Out Shift */
#define BP_ADDRESSING_MANC_SMB_CLK_OUT_SHIFT   29 /* SMBus Clock Out Shift */

/* SW Semaphore Register */
#define BP_ADDRESSING_SWSM_SMBI         0x00000001 /* Driver Semaphore bit */
#define BP_ADDRESSING_SWSM_SWESMBI      0x00000002 /* FW Semaphore bit */
#define BP_ADDRESSING_SWSM_WMNG         0x00000004 /* Wake MNG Clock */
#define BP_ADDRESSING_SWSM_DRV_LOAD     0x00000008 /* Driver Loaded Bit */

/* FW Semaphore Register */
#define BP_ADDRESSING_FWSM_MODE_MASK    0x0000000E /* FW mode */
#define BP_ADDRESSING_FWSM_MODE_SHIFT            1
#define BP_ADDRESSING_FWSM_FW_VALID     0x00008000 /* FW established a valid mode */

#define BP_ADDRESSING_FWSM_RSPCIPHY        0x00000040 /* Reset PHY on PCI reset */
#define BP_ADDRESSING_FWSM_DISSW           0x10000000 /* FW disable SW Write Access */
#define BP_ADDRESSING_FWSM_SKUSEL_MASK     0x60000000 /* LAN SKU select */
#define BP_ADDRESSING_FWSM_SKUEL_SHIFT     29
#define BP_ADDRESSING_FWSM_SKUSEL_EMB      0x0 /* Embedded SKU */
#define BP_ADDRESSING_FWSM_SKUSEL_CONS     0x1 /* Consumer SKU */
#define BP_ADDRESSING_FWSM_SKUSEL_PERF_100 0x2 /* Perf & Corp 10/100 SKU */
#define BP_ADDRESSING_FWSM_SKUSEL_PERF_GBE 0x3 /* Perf & Copr GbE SKU */

/* FFLT Debug Register */
#define BP_ADDRESSING_FFLT_DBG_INVC     0x00100000 /* Invalid /C/ code handling */

typedef enum {
    bp_addressing_mng_mode_none     = 0,
    bp_addressing_mng_mode_asf,
    bp_addressing_mng_mode_pt,
    bp_addressing_mng_mode_ipmi,
    bp_addressing_mng_mode_host_interface_only
} bp_addressing_mng_mode;

/* Host Inteface Control Register */
#define BP_ADDRESSING_HICR_EN           0x00000001  /* Enable Bit - RO */
#define BP_ADDRESSING_HICR_C            0x00000002  /* Driver sets this bit when done
                                             * to put command in RAM */
#define BP_ADDRESSING_HICR_SV           0x00000004  /* Status Validity */
#define BP_ADDRESSING_HICR_FWR          0x00000080  /* FW reset. Set by the Host */

/* Host Interface Command Interface - Address range 0x8800-0x8EFF */
#define BP_ADDRESSING_HI_MAX_DATA_LENGTH         252 /* Host Interface data length */
#define BP_ADDRESSING_HI_MAX_BLOCK_BYTE_LENGTH  1792 /* Number of bytes in range */
#define BP_ADDRESSING_HI_MAX_BLOCK_DWORD_LENGTH  448 /* Number of dwords in range */
#define BP_ADDRESSING_HI_COMMAND_TIMEOUT         500 /* Time in ms to process HI command */

struct bp_addressing_host_command_header {
    uint8_t command_id;
    uint8_t command_length;
    uint8_t command_options;   /* I/F bits for command, status for return */
    uint8_t checksum;
};
struct bp_addressing_host_command_info {
    struct bp_addressing_host_command_header command_header;  /* Command Head/Command Result Head has 4 bytes */
    uint8_t command_data[BP_ADDRESSING_HI_MAX_DATA_LENGTH];   /* Command data can length 0..252 */
};

/* Host SMB register #0 */
#define BP_ADDRESSING_HSMC0R_CLKIN      0x00000001  /* SMB Clock in */
#define BP_ADDRESSING_HSMC0R_DATAIN     0x00000002  /* SMB Data in */
#define BP_ADDRESSING_HSMC0R_DATAOUT    0x00000004  /* SMB Data out */
#define BP_ADDRESSING_HSMC0R_CLKOUT     0x00000008  /* SMB Clock out */

/* Host SMB register #1 */
#define BP_ADDRESSING_HSMC1R_CLKIN      BP_ADDRESSING_HSMC0R_CLKIN
#define BP_ADDRESSING_HSMC1R_DATAIN     BP_ADDRESSING_HSMC0R_DATAIN
#define BP_ADDRESSING_HSMC1R_DATAOUT    BP_ADDRESSING_HSMC0R_DATAOUT
#define BP_ADDRESSING_HSMC1R_CLKOUT     BP_ADDRESSING_HSMC0R_CLKOUT

/* FW Status Register */
#define BP_ADDRESSING_FWSTS_FWS_MASK    0x000000FF  /* FW Status */

/* Wake Up Packet Length */
#define BP_ADDRESSING_WUPL_LENGTH_MASK 0x0FFF   /* Only the lower 12 bits are valid */

#define BP_ADDRESSING_MDALIGN          4096

/* PCI-Ex registers */

/* PCI-Ex Control Register */
#define BP_ADDRESSING_GCR_RXD_NO_SNOOP          0x00000001
#define BP_ADDRESSING_GCR_RXDSCW_NO_SNOOP       0x00000002
#define BP_ADDRESSING_GCR_RXDSCR_NO_SNOOP       0x00000004
#define BP_ADDRESSING_GCR_TXD_NO_SNOOP          0x00000008
#define BP_ADDRESSING_GCR_TXDSCW_NO_SNOOP       0x00000010
#define BP_ADDRESSING_GCR_TXDSCR_NO_SNOOP       0x00000020

#define PCI_EX_NO_SNOOP_ALL (BP_ADDRESSING_GCR_RXD_NO_SNOOP         | \
                             BP_ADDRESSING_GCR_RXDSCW_NO_SNOOP      | \
                             BP_ADDRESSING_GCR_RXDSCR_NO_SNOOP      | \
                             BP_ADDRESSING_GCR_TXD_NO_SNOOP         | \
                             BP_ADDRESSING_GCR_TXDSCW_NO_SNOOP      | \
                             BP_ADDRESSING_GCR_TXDSCR_NO_SNOOP)

#define PCI_EX_82566_SNOOP_ALL PCI_EX_NO_SNOOP_ALL

#define BP_ADDRESSING_GCR_L1_ACT_WITHOUT_L0S_RX 0x08000000
/* Function Active and Power State to MNG */
#define BP_ADDRESSING_FACTPS_FUNC0_POWER_STATE_MASK         0x00000003
#define BP_ADDRESSING_FACTPS_LAN0_VALID                     0x00000004
#define BP_ADDRESSING_FACTPS_FUNC0_AUX_EN                   0x00000008
#define BP_ADDRESSING_FACTPS_FUNC1_POWER_STATE_MASK         0x000000C0
#define BP_ADDRESSING_FACTPS_FUNC1_POWER_STATE_SHIFT        6
#define BP_ADDRESSING_FACTPS_LAN1_VALID                     0x00000100
#define BP_ADDRESSING_FACTPS_FUNC1_AUX_EN                   0x00000200
#define BP_ADDRESSING_FACTPS_FUNC2_POWER_STATE_MASK         0x00003000
#define BP_ADDRESSING_FACTPS_FUNC2_POWER_STATE_SHIFT        12
#define BP_ADDRESSING_FACTPS_IDE_ENABLE                     0x00004000
#define BP_ADDRESSING_FACTPS_FUNC2_AUX_EN                   0x00008000
#define BP_ADDRESSING_FACTPS_FUNC3_POWER_STATE_MASK         0x000C0000
#define BP_ADDRESSING_FACTPS_FUNC3_POWER_STATE_SHIFT        18
#define BP_ADDRESSING_FACTPS_SP_ENABLE                      0x00100000
#define BP_ADDRESSING_FACTPS_FUNC3_AUX_EN                   0x00200000
#define BP_ADDRESSING_FACTPS_FUNC4_POWER_STATE_MASK         0x03000000
#define BP_ADDRESSING_FACTPS_FUNC4_POWER_STATE_SHIFT        24
#define BP_ADDRESSING_FACTPS_IPMI_ENABLE                    0x04000000
#define BP_ADDRESSING_FACTPS_FUNC4_AUX_EN                   0x08000000
#define BP_ADDRESSING_FACTPS_MNGCG                          0x20000000
#define BP_ADDRESSING_FACTPS_LAN_FUNC_SEL                   0x40000000
#define BP_ADDRESSING_FACTPS_PM_STATE_CHANGED               0x80000000

/* EEPROM Commands - Microwire */
#define EEPROM_READ_OPCODE_MICROWIRE  0x6  /* EEPROM read opcode */
#define EEPROM_WRITE_OPCODE_MICROWIRE 0x5  /* EEPROM write opcode */
#define EEPROM_ERASE_OPCODE_MICROWIRE 0x7  /* EEPROM erase opcode */
#define EEPROM_EWEN_OPCODE_MICROWIRE  0x13 /* EEPROM erase/write enable */
#define EEPROM_EWDS_OPCODE_MICROWIRE  0x10 /* EEPROM erast/write disable */

/* EEPROM Commands - SPI */
#define EEPROM_MAX_RETRY_SPI    5000 /* Max wait of 5ms, for RDY signal */
#define EEPROM_READ_OPCODE_SPI      0x03  /* EEPROM read opcode */
#define EEPROM_WRITE_OPCODE_SPI     0x02  /* EEPROM write opcode */
#define EEPROM_A8_OPCODE_SPI        0x08  /* opcode bit-3 = address bit-8 */
#define EEPROM_WREN_OPCODE_SPI      0x06  /* EEPROM set Write Enable latch */
#define EEPROM_WRDI_OPCODE_SPI      0x04  /* EEPROM reset Write Enable latch */
#define EEPROM_RDSR_OPCODE_SPI      0x05  /* EEPROM read Status register */
#define EEPROM_WRSR_OPCODE_SPI      0x01  /* EEPROM write Status register */
#define EEPROM_ERASE4K_OPCODE_SPI   0x20  /* EEPROM ERASE 4KB */
#define EEPROM_ERASE64K_OPCODE_SPI  0xD8  /* EEPROM ERASE 64KB */
#define EEPROM_ERASE256_OPCODE_SPI  0xDB  /* EEPROM ERASE 256B */

/* EEPROM Size definitions */
#define EEPROM_WORD_SIZE_SHIFT  6
#define EEPROM_SIZE_SHIFT       10
#define EEPROM_SIZE_MASK        0x1C00

/* EEPROM Word Offsets */
#define EEPROM_COMPAT                 0x0003
#define EEPROM_ID_LED_SETTINGS        0x0004
#define EEPROM_VERSION                0x0005
#define EEPROM_SERDES_AMPLITUDE       0x0006 /* For SERDES output amplitude adjustment. */
#define EEPROM_PHY_CLASS_WORD         0x0007
#define EEPROM_INIT_CONTROL1_REG      0x000A
#define EEPROM_INIT_CONTROL2_REG      0x000F
#define EEPROM_SWDEF_PINS_CTRL_PORT_1 0x0010
#define EEPROM_INIT_CONTROL3_PORT_B   0x0014
#define EEPROM_INIT_3GIO_3            0x001A
#define EEPROM_SWDEF_PINS_CTRL_PORT_0 0x0020
#define EEPROM_INIT_CONTROL3_PORT_A   0x0024
#define EEPROM_CFG                    0x0012
#define EEPROM_FLASH_VERSION          0x0032
#define EEPROM_CHECKSUM_REG           0x003F

#define BP_ADDRESSING_EEPROM_CFG_DONE         0x00040000   /* MNG config cycle done */
#define BP_ADDRESSING_EEPROM_CFG_DONE_PORT_1  0x00080000   /* ...for second port */

/* Word definitions for ID LED Settings */
#define ID_LED_RESERVED_0000 0x0000
#define ID_LED_RESERVED_FFFF 0xFFFF
#define ID_LED_RESERVED_82573  0xF746
#define ID_LED_DEFAULT_82573   0x1811
#define ID_LED_DEFAULT       ((ID_LED_OFF1_ON2 << 12) | \
                              (ID_LED_OFF1_OFF2 << 8) | \
                              (ID_LED_DEF1_DEF2 << 4) | \
                              (ID_LED_DEF1_DEF2))
#define ID_LED_DEFAULT_ICH8LAN  ((ID_LED_DEF1_DEF2 << 12) | \
                                 (ID_LED_DEF1_OFF2 <<  8) | \
                                 (ID_LED_DEF1_ON2  <<  4) | \
                                 (ID_LED_DEF1_DEF2))
#define ID_LED_DEF1_DEF2     0x1
#define ID_LED_DEF1_ON2      0x2
#define ID_LED_DEF1_OFF2     0x3
#define ID_LED_ON1_DEF2      0x4
#define ID_LED_ON1_ON2       0x5
#define ID_LED_ON1_OFF2      0x6
#define ID_LED_OFF1_DEF2     0x7
#define ID_LED_OFF1_ON2      0x8
#define ID_LED_OFF1_OFF2     0x9

#define IGP_ACTIVITY_LED_MASK   0xFFFFF0FF
#define IGP_ACTIVITY_LED_ENABLE 0x0300
#define IGP_LED3_MODE           0x07000000


/* Mask bits for SERDES amplitude adjustment in Word 6 of the EEPROM */
#define EEPROM_SERDES_AMPLITUDE_MASK  0x000F

/* Mask bit for PHY class in Word 7 of the EEPROM */
#define EEPROM_PHY_CLASS_A   0x8000

/* Mask bits for fields in Word 0x0a of the EEPROM */
#define EEPROM_WORD0A_ILOS   0x0010
#define EEPROM_WORD0A_SWDPIO 0x01E0
#define EEPROM_WORD0A_LRST   0x0200
#define EEPROM_WORD0A_FD     0x0400
#define EEPROM_WORD0A_66MHZ  0x0800

/* Mask bits for fields in Word 0x0f of the EEPROM */
#define EEPROM_WORD0F_PAUSE_MASK 0x3000
#define EEPROM_WORD0F_PAUSE      0x1000
#define EEPROM_WORD0F_ASM_DIR    0x2000
#define EEPROM_WORD0F_ANE        0x0800
#define EEPROM_WORD0F_SWPDIO_EXT 0x00F0
#define EEPROM_WORD0F_LPLU       0x0001

/* Mask bits for fields in Word 0x10/0x20 of the EEPROM */
#define EEPROM_WORD1020_GIGA_DISABLE         0x0010
#define EEPROM_WORD1020_GIGA_DISABLE_NON_D0A 0x0008

/* Mask bits for fields in Word 0x1a of the EEPROM */
#define EEPROM_WORD1A_ASPM_MASK  0x000C

/* For checksumming, the sum of all words in the EEPROM should equal 0xBABA. */
#define EEPROM_SUM 0xBABA

/* EEPROM Map defines (WORD OFFSETS)*/
#define EEPROM_NODE_ADDRESS_BYTE_0 0
#define EEPROM_PBA_BYTE_1          8

#define EEPROM_RESERVED_WORD          0xFFFF

/* EEPROM Map Sizes (Byte Counts) */
#define PBA_SIZE 4

/* Collision related configuration parameters */
#define BP_ADDRESSING_COLLISION_THRESHOLD       15
#define BP_ADDRESSING_CT_SHIFT                  4
/* Collision distance is a 0-based value that applies to
 * half-duplex-capable hardware only. */
#define BP_ADDRESSING_COLLISION_DISTANCE        63
#define BP_ADDRESSING_COLLISION_DISTANCE_82542  64
#define BP_ADDRESSING_FDX_COLLISION_DISTANCE    BP_ADDRESSING_COLLISION_DISTANCE
#define BP_ADDRESSING_HDX_COLLISION_DISTANCE    BP_ADDRESSING_COLLISION_DISTANCE
#define BP_ADDRESSING_COLD_SHIFT                12

/* Number of Transmit and Receive Descriptors must be a multiple of 8 */
#define REQ_TX_DESCRIPTOR_MULTIPLE  8
#define REQ_RX_DESCRIPTOR_MULTIPLE  8

/* Default values for the transmit IPG register */
#define DEFAULT_82542_TIPG_IPGT        10
#define DEFAULT_82543_TIPG_IPGT_FIBER  9
#define DEFAULT_82543_TIPG_IPGT_COPPER 8

#define BP_ADDRESSING_TIPG_IPGT_MASK  0x000003FF
#define BP_ADDRESSING_TIPG_IPGR1_MASK 0x000FFC00
#define BP_ADDRESSING_TIPG_IPGR2_MASK 0x3FF00000

#define DEFAULT_82542_TIPG_IPGR1 2
#define DEFAULT_82543_TIPG_IPGR1 8
#define BP_ADDRESSING_TIPG_IPGR1_SHIFT  10

#define DEFAULT_82542_TIPG_IPGR2 10
#define DEFAULT_82543_TIPG_IPGR2 6
#define DEFAULT_80003ES2LAN_TIPG_IPGR2 7
#define BP_ADDRESSING_TIPG_IPGR2_SHIFT  20

#define DEFAULT_80003ES2LAN_TIPG_IPGT_10_100 0x00000009
#define DEFAULT_80003ES2LAN_TIPG_IPGT_1000   0x00000008
#define BP_ADDRESSING_TXDMAC_DPP 0x00000001

/* Adaptive IFS defines */
#define TX_THRESHOLD_START     8
#define TX_THRESHOLD_INCREMENT 10
#define TX_THRESHOLD_DECREMENT 1
#define TX_THRESHOLD_STOP      190
#define TX_THRESHOLD_DISABLE   0
#define TX_THRESHOLD_TIMER_MS  10000
#define MIN_NUM_XMITS          1000
#define IFS_MAX                80
#define IFS_STEP               10
#define IFS_MIN                40
#define IFS_RATIO              4

/* Extended Configuration Control and Size */
#define BP_ADDRESSING_EXTCNF_CTRL_PCIE_WRITE_ENABLE 0x00000001
#define BP_ADDRESSING_EXTCNF_CTRL_PHY_WRITE_ENABLE  0x00000002
#define BP_ADDRESSING_EXTCNF_CTRL_D_UD_ENABLE       0x00000004
#define BP_ADDRESSING_EXTCNF_CTRL_D_UD_LATENCY      0x00000008
#define BP_ADDRESSING_EXTCNF_CTRL_D_UD_OWNER        0x00000010
#define BP_ADDRESSING_EXTCNF_CTRL_MDIO_SW_OWNERSHIP 0x00000020
#define BP_ADDRESSING_EXTCNF_CTRL_MDIO_HW_OWNERSHIP 0x00000040
#define BP_ADDRESSING_EXTCNF_CTRL_EXT_CNF_POINTER   0x0FFF0000

#define BP_ADDRESSING_EXTCNF_SIZE_EXT_PHY_LENGTH    0x000000FF
#define BP_ADDRESSING_EXTCNF_SIZE_EXT_DOCK_LENGTH   0x0000FF00
#define BP_ADDRESSING_EXTCNF_SIZE_EXT_PCIE_LENGTH   0x00FF0000
#define BP_ADDRESSING_EXTCNF_CTRL_LCD_WRITE_ENABLE  0x00000001
#define BP_ADDRESSING_EXTCNF_CTRL_SWFLAG            0x00000020

/* PBA constants */
#define BP_ADDRESSING_PBA_8K 0x0008    /* 8KB, default Rx allocation */
#define BP_ADDRESSING_PBA_12K 0x000C    /* 12KB, default Rx allocation */
#define BP_ADDRESSING_PBA_16K 0x0010    /* 16KB, default TX allocation */
#define BP_ADDRESSING_PBA_22K 0x0016
#define BP_ADDRESSING_PBA_24K 0x0018
#define BP_ADDRESSING_PBA_30K 0x001E
#define BP_ADDRESSING_PBA_32K 0x0020
#define BP_ADDRESSING_PBA_34K 0x0022
#define BP_ADDRESSING_PBA_38K 0x0026
#define BP_ADDRESSING_PBA_40K 0x0028
#define BP_ADDRESSING_PBA_48K 0x0030    /* 48KB, default RX allocation */

#define BP_ADDRESSING_PBS_16K BP_ADDRESSING_PBA_16K

/* Flow Control Constants */
#define FLOW_CONTROL_ADDRESS_LOW  0x00C28001
#define FLOW_CONTROL_ADDRESS_HIGH 0x00000100
#define FLOW_CONTROL_TYPE         0x8808

/* The historical defaults for the flow control values are given below. */
#define FC_DEFAULT_HI_THRESH        (0x8000)    /* 32KB */
#define FC_DEFAULT_LO_THRESH        (0x4000)    /* 16KB */
#define FC_DEFAULT_TX_TIMER         (0x100)     /* ~130 us */

/* PCIX Config space */
#define PCIX_COMMAND_REGISTER    0xE6
#define PCIX_STATUS_REGISTER_LO  0xE8
#define PCIX_STATUS_REGISTER_HI  0xEA

#define PCIX_COMMAND_MMRBC_MASK      0x000C
#define PCIX_COMMAND_MMRBC_SHIFT     0x2
#define PCIX_STATUS_HI_MMRBC_MASK    0x0060
#define PCIX_STATUS_HI_MMRBC_SHIFT   0x5
#define PCIX_STATUS_HI_MMRBC_4K      0x3
#define PCIX_STATUS_HI_MMRBC_2K      0x2


/* Number of bits required to shift right the "pause" bits from the
 * EEPROM (bits 13:12) to the "pause" (bits 8:7) field in the TXCW register.
 */
#define PAUSE_SHIFT 5

/* Number of bits required to shift left the "SWDPIO" bits from the
 * EEPROM (bits 8:5) to the "SWDPIO" (bits 25:22) field in the CTRL register.
 */
#define SWDPIO_SHIFT 17

/* Number of bits required to shift left the "SWDPIO_EXT" bits from the
 * EEPROM word F (bits 7:4) to the bits 11:8 of The Extended CTRL register.
 */
#define SWDPIO__EXT_SHIFT 4

/* Number of bits required to shift left the "ILOS" bit from the EEPROM
 * (bit 4) to the "ILOS" (bit 7) field in the CTRL register.
 */
#define ILOS_SHIFT  3


#define RECEIVE_BUFFER_ALIGN_SIZE  (256)

/* Number of milliseconds we wait for auto-negotiation to complete */
#define LINK_UP_TIMEOUT             500

/* Number of 100 microseconds we wait for PCI Express master disable */
#define MASTER_DISABLE_TIMEOUT      800
/* Number of milliseconds we wait for Eeprom auto read bit done after MAC reset */
#define AUTO_READ_DONE_TIMEOUT      10
/* Number of milliseconds we wait for PHY configuration done after MAC reset */
#define PHY_CFG_TIMEOUT             100

#define BP_ADDRESSING_TX_BUFFER_SIZE ((uint32_t)1514)

/* The carrier extension symbol, as received by the NIC. */
#define CARRIER_EXTENSION   0x0F

/* Capabilities for SKU Control  */
#define IGP3_CAP_INITIATE_TEAM       0x0001 /* Able to initiate a team */
#define IGP3_CAP_WFM                 0x0002 /* Support WoL and PXE */
#define IGP3_CAP_ASF                 0x0004 /* Support ASF */
#define IGP3_CAP_LPLU                0x0008 /* Support Low Power Link Up */
#define IGP3_CAP_DC_AUTO_SPEED       0x0010 /* Support AC/DC Auto Link Speed */
#define IGP3_CAP_SPD                 0x0020 /* Support Smart Power Down */
#define IGP3_CAP_MULT_QUEUE          0x0040 /* Support 2 tx & 2 rx queues */
#define IGP3_CAP_RSS                 0x0080 /* Support RSS */
#define IGP3_CAP_8021PQ              0x0100 /* Support 802.1Q & 802.1p */
#define IGP3_CAP_AMT_CB              0x0200 /* Support active manageability and circuit breaker */

#define IGP3_PPC_JORDAN_EN           0x0001
#define IGP3_PPC_JORDAN_GIGA_SPEED   0x0002

#define IGP3_KMRN_PMC_EE_IDLE_LINK_DIS         0x0001
#define IGP3_KMRN_PMC_K0S_ENTRY_LATENCY_MASK   0x001E
#define IGP3_KMRN_PMC_K0S_MODE1_EN_GIGA        0x0020
#define IGP3_KMRN_PMC_K0S_MODE1_EN_100         0x0040

#define IGP3BP_ADDRESSING_PHY_MISC_CTRL                0x1B   /* Misc. Ctrl register */
#define IGP3_PHY_MISC_DUPLEX_MANUAL_SET        0x1000 /* Duplex Manual Set */

#define IGP3_KMRN_EXT_CTRL  PHY_REG(770, 18)
#define IGP3_KMRN_EC_DIS_INBAND    0x0080

#define IGP03BP_ADDRESSING_E_PHY_ID  0x02A80390
#define IFE_E_PHY_ID         0x02A80330 /* 10/100 PHY */
#define IFE_PLUS_E_PHY_ID    0x02A80320
#define IFE_C_E_PHY_ID       0x02A80310

#define IFE_PHY_EXTENDED_STATUS_CONTROL   0x10  /* 100BaseTx Extended Status, Control and Address */
#define IFE_PHY_SPECIAL_CONTROL           0x11  /* 100BaseTx PHY special control register */
#define IFE_PHY_RCV_FALSE_CARRIER         0x13  /* 100BaseTx Receive False Carrier Counter */
#define IFE_PHY_RCV_DISCONNECT            0x14  /* 100BaseTx Receive Disconnet Counter */
#define IFE_PHY_RCV_ERROT_FRAME           0x15  /* 100BaseTx Receive Error Frame Counter */
#define IFE_PHY_RCV_SYMBOL_ERR            0x16  /* Receive Symbol Error Counter */
#define IFE_PHY_PREM_EOF_ERR              0x17  /* 100BaseTx Receive Premature End Of Frame Error Counter */
#define IFE_PHY_RCV_EOF_ERR               0x18  /* 10BaseT Receive End Of Frame Error Counter */
#define IFE_PHY_TX_JABBER_DETECT          0x19  /* 10BaseT Transmit Jabber Detect Counter */
#define IFE_PHY_EQUALIZER                 0x1A  /* PHY Equalizer Control and Status */
#define IFE_PHY_SPECIAL_CONTROL_LED       0x1B  /* PHY special control and LED configuration */
#define IFE_PHY_MDIX_CONTROL              0x1C  /* MDI/MDI-X Control register */
#define IFE_PHY_HWI_CONTROL               0x1D  /* Hardware Integrity Control (HWI) */

#define IFE_PESC_REDUCED_POWER_DOWN_DISABLE  0x2000  /* Defaut 1 = Disable auto reduced power down */
#define IFE_PESC_100BTX_POWER_DOWN           0x0400  /* Indicates the power state of 100BASE-TX */
#define IFE_PESC_10BTX_POWER_DOWN            0x0200  /* Indicates the power state of 10BASE-T */
#define IFE_PESC_POLARITY_REVERSED           0x0100  /* Indicates 10BASE-T polarity */
#define IFE_PESC_PHY_ADDR_MASK               0x007C  /* Bit 6:2 for sampled PHY address */
#define IFE_PESC_SPEED                       0x0002  /* Auto-negotiation speed result 1=100Mbs, 0=10Mbs */
#define IFE_PESC_DUPLEX                      0x0001  /* Auto-negotiation duplex result 1=Full, 0=Half */
#define IFE_PESC_POLARITY_REVERSED_SHIFT     8

#define IFE_PSC_DISABLE_DYNAMIC_POWER_DOWN   0x0100  /* 1 = Dyanmic Power Down disabled */
#define IFE_PSC_FORCE_POLARITY               0x0020  /* 1=Reversed Polarity, 0=Normal */
#define IFE_PSC_AUTO_POLARITY_DISABLE        0x0010  /* 1=Auto Polarity Disabled, 0=Enabled */
#define IFE_PSC_JABBER_FUNC_DISABLE          0x0001  /* 1=Jabber Disabled, 0=Normal Jabber Operation */
#define IFE_PSC_FORCE_POLARITY_SHIFT         5
#define IFE_PSC_AUTO_POLARITY_DISABLE_SHIFT  4

#define IFE_PMC_AUTO_MDIX                    0x0080  /* 1=enable MDI/MDI-X feature, default 0=disabled */
#define IFE_PMC_FORCE_MDIX                   0x0040  /* 1=force MDIX-X, 0=force MDI */
#define IFE_PMC_MDIX_STATUS                  0x0020  /* 1=MDI-X, 0=MDI */
#define IFE_PMC_AUTO_MDIX_COMPLETE           0x0010  /* Resolution algorthm is completed */
#define IFE_PMC_MDIX_MODE_SHIFT              6
#define IFE_PHC_MDIX_RESET_ALL_MASK          0x0000  /* Disable auto MDI-X */

#define IFE_PHC_HWI_ENABLE                   0x8000  /* Enable the HWI feature */
#define IFE_PHC_ABILITY_CHECK                0x4000  /* 1= Test Passed, 0=failed */
#define IFE_PHC_TEST_EXEC                    0x2000  /* PHY launch test pulses on the wire */
#define IFE_PHC_HIGHZ                        0x0200  /* 1 = Open Circuit */
#define IFE_PHC_LOWZ                         0x0400  /* 1 = Short Circuit */
#define IFE_PHC_LOW_HIGH_Z_MASK              0x0600  /* Mask for indication type of problem on the line */
#define IFE_PHC_DISTANCE_MASK                0x01FF  /* Mask for distance to the cable problem, in 80cm granularity */
#define IFE_PHC_RESET_ALL_MASK               0x0000  /* Disable HWI */
#define IFE_PSCL_PROBE_MODE                  0x0020  /* LED Probe mode */
#define IFE_PSCL_PROBE_LEDS_OFF              0x0006  /* Force LEDs 0 and 2 off */
#define IFE_PSCL_PROBE_LEDS_ON               0x0007  /* Force LEDs 0 and 2 on */

#define ICH8_FLASH_COMMAND_TIMEOUT           500   /* 500 ms , should be adjusted */
#define ICH8_FLASH_CYCLE_REPEAT_COUNT        10    /* 10 cycles , should be adjusted */
#define ICH8_FLASH_SEG_SIZE_256              256
#define ICH8_FLASH_SEG_SIZE_4K               4096
#define ICH8_FLASH_SEG_SIZE_64K              65536

#define ICH8_CYCLE_READ                      0x0
#define ICH8_CYCLE_RESERVED                  0x1
#define ICH8_CYCLE_WRITE                     0x2
#define ICH8_CYCLE_ERASE                     0x3

#define ICH8_FLASH_GFPREG   0x0000
#define ICH8_FLASH_HSFSTS   0x0004
#define ICH8_FLASH_HSFCTL   0x0006
#define ICH8_FLASH_FADDR    0x0008
#define ICH8_FLASH_FDATA0   0x0010
#define ICH8_FLASH_FRACC    0x0050
#define ICH8_FLASH_FREG0    0x0054
#define ICH8_FLASH_FREG1    0x0058
#define ICH8_FLASH_FREG2    0x005C
#define ICH8_FLASH_FREG3    0x0060
#define ICH8_FLASH_FPR0     0x0074
#define ICH8_FLASH_FPR1     0x0078
#define ICH8_FLASH_SSFSTS   0x0090
#define ICH8_FLASH_SSFCTL   0x0092
#define ICH8_FLASH_PREOP    0x0094
#define ICH8_FLASH_OPTYPE   0x0096
#define ICH8_FLASH_OPMENU   0x0098

#define ICH8_FLASH_REG_MAPSIZE      0x00A0
#define ICH8_FLASH_SECTOR_SIZE      4096
#define ICH8_GFPREG_BASE_MASK       0x1FFF
#define ICH8_FLASH_LINEAR_ADDR_MASK 0x00FFFFFF

/* ICH8 GbE Flash Hardware Sequencing Flash Status Register bit breakdown */
/* Offset 04h HSFSTS */
union ich8_hws_flash_status {
    struct ich8_hsfsts {
#ifdef BP_ADDRESSING_BIG_ENDIAN
        uint16_t reserved2      :6;
        uint16_t fldesvalid     :1;
        uint16_t flockdn        :1;
        uint16_t flcdone        :1;
        uint16_t flcerr         :1;
        uint16_t dael           :1;
        uint16_t berasesz       :2;
        uint16_t flcinprog      :1;
        uint16_t reserved1      :2;
#else
        uint16_t flcdone        :1;   /* bit 0 Flash Cycle Done */
        uint16_t flcerr         :1;   /* bit 1 Flash Cycle Error */
        uint16_t dael           :1;   /* bit 2 Direct Access error Log */
        uint16_t berasesz       :2;   /* bit 4:3 Block/Sector Erase Size */
        uint16_t flcinprog      :1;   /* bit 5 flash SPI cycle in Progress */
        uint16_t reserved1      :2;   /* bit 13:6 Reserved */
        uint16_t reserved2      :6;   /* bit 13:6 Reserved */
        uint16_t fldesvalid     :1;   /* bit 14 Flash Descriptor Valid */
        uint16_t flockdn        :1;   /* bit 15 Flash Configuration Lock-Down */
#endif
    } hsf_status;
    uint16_t regval;
};

/* ICH8 GbE Flash Hardware Sequencing Flash control Register bit breakdown */
/* Offset 06h FLCTL */
union ich8_hws_flash_ctrl {
    struct ich8_hsflctl {
#ifdef BP_ADDRESSING_BIG_ENDIAN
        uint16_t fldbcount      :2;
        uint16_t flockdn        :6;
        uint16_t flcgo          :1;
        uint16_t flcycle        :2;
        uint16_t reserved       :5;
#else
        uint16_t flcgo          :1;   /* 0 Flash Cycle Go */
        uint16_t flcycle        :2;   /* 2:1 Flash Cycle */
        uint16_t reserved       :5;   /* 7:3 Reserved  */
        uint16_t fldbcount      :2;   /* 9:8 Flash Data Byte Count */
        uint16_t flockdn        :6;   /* 15:10 Reserved */
#endif
    } hsf_ctrl;
    uint16_t regval;
};

/* ICH8 Flash Region Access Permissions */
union ich8_hws_flash_regacc {
    struct ich8_flracc {
#ifdef BP_ADDRESSING_BIG_ENDIAN
        uint32_t gmwag          :8;
        uint32_t gmrag          :8;
        uint32_t grwa           :8;
        uint32_t grra           :8;
#else
        uint32_t grra           :8;   /* 0:7 GbE region Read Access */
        uint32_t grwa           :8;   /* 8:15 GbE region Write Access */
        uint32_t gmrag          :8;   /* 23:16 GbE Master Read Access Grant  */
        uint32_t gmwag          :8;   /* 31:24 GbE Master Write Access Grant */
#endif
    } hsf_flregacc;
    uint16_t regval;
};

#endif /* _BP_ADDRESSING_HW_H_ */
