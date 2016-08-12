#ifndef __BPYPASS_PCIWDT_H__
#define __BPYPASS_PCIWDT_H__

#include "bypass_kversion.h"
#include "bypass_pci.h"
#include PCI_REG_HEAD

#define IS_IOSPACE(reg)         (reg & 0x1)

#define IS_THE_SAME_PCI_TYPE(new_info, database_info)               \
        ( (new_info->vid == database_info->vid ) && (new_info->did == database_info->did ) && \
            (new_info->svid == database_info->svid ) && (new_info->sdid == database_info->sdid ) )

#define IOADDR_OFFSET				0x0u
#define IODATA_OFFSET				0x4u
#define CTRL_ADDRESS_TO_SLOT_ID(addr)		(addr & 0x3u)
#define DEV_MASK_A				0x1u
#define DEV_MASK_B				0x0u

/* for PCIWDT use */
#define INTEL_82571EB_SDP_LOW_OFFSET        INTEL_CTRL
#define INTEL_82571EB_SDP_HIGH_OFFSET       INTEL_CTRL_EXT

#define INTEL_82571EB_SDP0_REG_OFFSET       INTEL_82571EB_SDP_LOW_OFFSET
#define INTEL_82571EB_SDP0_DIR_MASK         INTEL_SDP0_IODIR
#define INTEL_82571EB_SDP0_DATA_MASK        INTEL_SDP0_DATA
#define INTEL_82571EB_SDP1_REG_OFFSET       INTEL_82571EB_SDP_LOW_OFFSET
#define INTEL_82571EB_SDP1_DIR_MASK         INTEL_SDP1_IODIR
#define INTEL_82571EB_SDP1_DATA_MASK        INTEL_SDP1_DATA
#define INTEL_82571EB_SDP2_REG_OFFSET       INTEL_82571EB_SDP_HIGH_OFFSET
#define INTEL_82571EB_SDP2_DIR_MASK         INTEL_SDP2_IODIR
#define INTEL_82571EB_SDP2_DATA_MASK        INTEL_SDP2_DATA
#define INTEL_82571EB_SDP3_REG_OFFSET       INTEL_82571EB_SDP_HIGH_OFFSET
#define INTEL_82571EB_SDP3_DIR_MASK         INTEL_SDP3_IODIR
#define INTEL_82571EB_SDP3_DATA_MASK        INTEL_SDP3_DATA

#define SDID_ABN484_SDP				0x2005
#define ABN484_WDT_LOAD_A_REG			INTEL_82571EB_SDP1_REG_OFFSET
#define ABN484_WDT_LOAD_A_DIR			INTEL_82571EB_SDP1_DIR_MASK
#define ABN484_WDT_LOAD_A_MASK			INTEL_82571EB_SDP1_DATA_MASK
#define ABN484_WDT_LOAD_B_REG			INTEL_82571EB_SDP0_REG_OFFSET
#define ABN484_WDT_LOAD_B_DIR			INTEL_82571EB_SDP0_DIR_MASK
#define ABN484_WDT_LOAD_B_MASK			INTEL_82571EB_SDP0_DATA_MASK

#define SDID_ABN482_SDP				0x2007
#define ABN482_WDT_LOAD_REG			INTEL_82571EB_SDP3_REG_OFFSET
#define ABN482_WDT_LOAD_DIR			INTEL_82571EB_SDP3_DIR_MASK
#define ABN482_WDT_LOAD_MASK			INTEL_82571EB_SDP3_DATA_MASK
static const bp_pci_cmd abn484_pci_wdt_refresh[] = {
	{
	    .reg_offset =	ABN484_WDT_LOAD_A_REG,
	    .dir_mask =		ABN484_WDT_LOAD_A_DIR,
	    .cmd_mask =		ABN484_WDT_LOAD_A_MASK,
	},{
	    .reg_offset =	ABN484_WDT_LOAD_B_REG,
	    .dir_mask =		ABN484_WDT_LOAD_B_DIR,
	    .cmd_mask =		ABN484_WDT_LOAD_B_MASK,
	}
};


static const bp_pci_initdata abn484_bp_pci_initdata = {
	.info = {
	    .vid =			    INTEL_VID,
	    .did =			    I82571EB_COPPER_DID,
	    .svid =			    PORTWELL_VID,
	    .sdid =			    SDID_ABN484_SDP,
	    .io_bar_offset =		    PCI_BASE_ADDRESS_2,
	    .slot_dev_mask =		    DEV_MASK_A, 
	    .slot_chk_reg =		    INTEL_82571EB_SDP2_REG_OFFSET,
	    .slot_chk_mask =		    INTEL_82571EB_SDP2_DATA_MASK,
	    .slot_id_reg0=		    INTEL_82571EB_SDP_LOW_OFFSET,
	    .slot_id_reg1=		    INTEL_82571EB_SDP_LOW_OFFSET,
	    .slot_id_mask0 =		    INTEL_82571EB_SDP0_DATA_MASK,
	    .slot_id_mask1 =		    INTEL_82571EB_SDP1_DATA_MASK,
	},
	.refresh_wdt_cmds = abn484_pci_wdt_refresh,
};
static const bp_pci_cmd abn482_pci_wdt_refresh[] = {
	{
	    .reg_offset =	ABN482_WDT_LOAD_REG,
	    .dir_mask =		ABN482_WDT_LOAD_DIR,
	    .cmd_mask =		ABN482_WDT_LOAD_MASK,
	}
};
static const bp_pci_initdata abn482_bp_pci_initdata = {
	.info = {
	    .vid =			    INTEL_VID,
	    .did =			    I82571EB_FIBER_DID,
	    .svid =			    PORTWELL_VID,
	    .sdid =			    SDID_ABN482_SDP,
	    .io_bar_offset =		    PCI_BASE_ADDRESS_2,
	    .slot_dev_mask =		    DEV_MASK_B,
	    .slot_chk_reg =		    INTEL_82571EB_SDP2_REG_OFFSET,
	    .slot_chk_mask =		    INTEL_82571EB_SDP2_DATA_MASK,
	    .slot_id_reg1=		    INTEL_82571EB_SDP_HIGH_OFFSET,
	    .slot_id_reg0=		    INTEL_82571EB_SDP_LOW_OFFSET,
	    .slot_id_mask0 =		    INTEL_82571EB_SDP0_DATA_MASK,
	    .slot_id_mask1 =		    INTEL_82571EB_SDP2_DATA_MASK,
	},
	.refresh_wdt_cmds = abn482_pci_wdt_refresh,
};

#endif /*__BPYPASS_PCI__*/
