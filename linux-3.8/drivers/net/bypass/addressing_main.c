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

#include "addressing.h"
char bp_addressing_driver_name[] = "addressing";


extern int pw_card_conf;

/* bp_addressing_pci_tbl - PCI Device ID Table
 *
 * Last entry must be all 0s
 *
 * Macro expands to...
 *   {PCI_DEVICE(PCI_VENDOR_ID_INTEL, device_id)}
 */
static struct pci_device_id bp_addressing_pci_tbl[] = {
	INTEL_BP_ADDRESSING_ETHERNET_DEVICE(0x105E),
	INTEL_BP_ADDRESSING_ETHERNET_DEVICE(0x105F),
	INTEL_BP_ADDRESSING_ETHERNET_DEVICE(0x1096),
	/* required last entry */
	{0,}
};

//MODULE_DEVICE_TABLE(pci, bp_addressing_pci_tbl);

/* Local Function Prototypes */
static int bp_addressing_init_module(void);
int adressing(struct bp_addressing_adapter *adapter,int model);
static void __devexit bp_addressing_remove(struct pci_dev *pdev);
static int bp_addressing_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static int bp_addressing_sw_init(struct bp_addressing_adapter *adapter);

static struct pci_driver bp_addressing_driver = {
	.name     = bp_addressing_driver_name,
	.id_table = bp_addressing_pci_tbl,
	.probe    = bp_addressing_probe,
	.remove   = __devexit_p(bp_addressing_remove),
};

/**
 * bp_addressing_init_module - Driver Registration Routine
 *
 * bp_addressing_init_module is the first routine called when the driver is
 * loaded. All it does is register with the PCI subsystem.
 **/

static int __init
bp_addressing_init_module(void)
{
        int ret;
        ret=pci_register_driver(&bp_addressing_driver);
        pci_unregister_driver(&bp_addressing_driver);

        return -EINTR;
}

module_init(bp_addressing_init_module);

/**
 * bp_addressing_probe - Device Initialization Routine
 * @pdev: PCI device information struct
 * @ent: entry in bp_addressing_pci_tbl
 *
 * Returns 0 on success, negative on failure
 *
 * bp_addressing_probe initializes an adapter identified by a pci_dev structure.
 * The OS initialization, configuring of the adapter private structure,
 * and a hardware reset occur.
 **/

static int __devinit
bp_addressing_probe(struct pci_dev *pdev,
            const struct pci_device_id *ent)
{
	struct net_device *netdev;
	struct bp_addressing_adapter *adapter;
	unsigned long mmio_start, mmio_len;
	int  err;
	int model=0;
	if ((err = pci_enable_device(pdev)))
		return err;

	if ((err = pci_request_regions(pdev, bp_addressing_driver_name)))
		return err;

	pci_set_master(pdev);

	netdev = alloc_etherdev(sizeof(struct bp_addressing_adapter));
	if (!netdev) {
		err = -ENOMEM;
		goto err_alloc_etherdev;
	}

	SET_MODULE_OWNER(netdev);
	SET_NETDEV_DEV(netdev, &pdev->dev);

	pci_set_drvdata(pdev, netdev);
	adapter = netdev_priv(netdev);
	adapter->netdev = netdev;
	adapter->pdev = pdev;
	adapter->hw.back = adapter;

	mmio_start = pci_resource_start(pdev, BAR_0);
	mmio_len = pci_resource_len(pdev, BAR_0);

	adapter->hw.hw_addr = ioremap(mmio_start, mmio_len);
	if (!adapter->hw.hw_addr) {
		err = -EIO;
		goto err_ioremap;
	}

	/* setup the private structure */

	if ((err = bp_addressing_sw_init(adapter)))
		goto err_sw_init;

	/* initialize eeprom parameters */

	if (bp_addressing_init_eeprom_params(&adapter->hw)) {
		BP_ADDRESSING_ERR("EEPROM initialization failed\n");
		return -EIO;
	}
	if(pw_card_conf!=0)
	{
	    model=(pw_card_conf>>4|((pw_card_conf&0xf)<<8));
    	    adressing(adapter,model);
         }else
             printk("please input pw_card_conf\n");
	return 0;

err_sw_init:
	iounmap(adapter->hw.hw_addr);
err_ioremap:
	free_netdev(netdev);
err_alloc_etherdev:
	pci_release_regions(pdev);
	return err;
 
}

/**
 * bp_addressing_remove - Device Removal Routine
 * @pdev: PCI device information struct
 *
 * bp_addressing_remove is called by the PCI subsystem to alert the driver
 * that it should release a PCI device.  The could be caused by a
 * Hot-Plug event, or because the driver is going to be removed from
 * memory.
 **/

static void __devexit
bp_addressing_remove(struct pci_dev *pdev)
{
	struct net_device *netdev = pci_get_drvdata(pdev);
	struct bp_addressing_adapter *adapter = netdev_priv(netdev);

	iounmap(adapter->hw.hw_addr);
	if (adapter->hw.flash_address)
		iounmap(adapter->hw.flash_address);
	pci_release_regions(pdev);

	free_netdev(netdev);

	pci_disable_device(pdev);
}

/**
 * bp_addressing_sw_init - Initialize general software structures (struct bp_addressing_adapter)
 * @adapter: board private structure to initialize
 *
 * bp_addressing_sw_init initializes the Adapter private data structure.
 * Fields are initialized based on PCI device information and
 * OS network device settings (MTU size).
 **/

static int __devinit
bp_addressing_sw_init(struct bp_addressing_adapter *adapter)
{
	struct bp_addressing_hw *hw = &adapter->hw;
	struct net_device *netdev = adapter->netdev;
	struct pci_dev *pdev = adapter->pdev;
#ifdef CONFIG_BP_ADDRESSING_NAPI
	int i;
#endif

	/* PCI config space info */

	hw->vendor_id = pdev->vendor;
	hw->device_id = pdev->device;
	hw->subsystem_vendor_id = pdev->subsystem_vendor;
	hw->subsystem_id = pdev->subsystem_device;

	pci_read_config_byte(pdev, PCI_REVISION_ID, &hw->revision_id);

	pci_read_config_word(pdev, PCI_COMMAND, &hw->pci_cmd_word);

	adapter->rx_buffer_len = MAXIMUM_ETHERNET_VLAN_SIZE;
	adapter->rx_ps_bsize0 = BP_ADDRESSING_RXBUFFER_128;
	hw->max_frame_size = netdev->mtu +
			     ENET_HEADER_SIZE + ETHERNET_FCS_SIZE;
	hw->min_frame_size = MINIMUM_ETHERNET_FRAME_SIZE;

	/* identify the MAC */        
	if (bp_addressing_set_mac_type(hw)) {
		DPRINTK(PROBE, ERR, "Unknown MAC Type\n");
		return -EIO;
	}
    return 0;
}

/* bp_addressing_main.c */
