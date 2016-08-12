#ifndef _BYPASS_PCI_H_
#define _BYPASS_PCI_H_

#define CLASS_BRIDGE          0x60400
#define CLASS_LANCTRL         0x20000

#define MB_NAME_LEN           10
#define MAX_PCI_SLOTS_PER_CPU 4
#define PCI_ADDCARD_MAX_SLOTS 8        // Max of slot number per board
#define SLOT_BRIDGE_NUM       6        // Max of bridge number per slot
#define CARD_DEV_MAX          4        // Max of LAN chip number per card

/* PCI device config */
#define INTEL_VID             0x8086
#define PORTWELL_VID          0x15bb
#define I82571EB_COPPER_DID   0x105e
#define I82571EB_FIBER_DID    0x105f
#define I82580EB_COPPER_DID   0x150e
#define I82580EB_FIBER_DID    0x150f
#define IX540_COPPER_DID      0x1528
#define I82574L_COPPER_DID    0x10d3

/* Intel LAN controller register */
#define INTEL_CTRL            0x00000  /* Device Control */
#define INTEL_CTRL_EXT        0x00018  /* Extended Device Control */

/* Intel SDP pin definition */
#define INTEL_SDP0_DATA       0x00040000
#define INTEL_SDP1_DATA       0x00080000
#define INTEL_SDP0_IODIR      0x00400000
#define INTEL_SDP1_IODIR      0x00800000
#define INTEL_SDP2_DATA       0x00000040
#define INTEL_SDP3_DATA       0x00000080
#define INTEL_SDP2_IODIR      0x00000400
#define INTEL_SDP3_IODIR      0x00000800

/* EEPROM config */
#define GET_BYPASSID_OFFSET   0x10
#define EEPROM_SDP_NUM        2
#define GET_BYPASSSEG_NUM     0x12

/* BYPASSID define */
#define BYPASSID_ABN482       0x4820
#define BYPASSID_ABN484       0x4840
#define BYPASSID_ABN484L      0x4841
#endif
