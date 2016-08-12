/*******************************************************************************

  CASwell(R) Network Bypass Linux driver
  Copyright(c) 2011 Zeno Lai <zeno.lai@cas-well.com>

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

*******************************************************************************/

#include <linux/version.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include "bypass_kversion.h"
#include "bypass_slot.h"

static struct pci_bus *mb_slot_sbus[PCI_ADDCARD_MAX_SLOTS][SLOT_BRIDGE_NUM];
static struct pci_dev *slot_dev[PCI_ADDCARD_MAX_SLOTS][CARD_DEV_MAX];

uint16_t abn482_set_eeprom_data(uint8_t slotn, struct pci_dev *pdev);
uint16_t abn484_set_eeprom_data(uint8_t slotn, struct pci_dev *pdev);
/* 
 * 0x20 // eeprom SDP register
 * 0x40 // eeprom SDP register
 */
uint8_t eeprom_offset[EEPROM_SDP_NUM]={0x20,0x40} ;
/*
 * set slot number to the cirrect address.
 */
u8 abn482_eep_sdp_data[PCI_ADDCARD_MAX_SLOTS][EEPROM_SDP_NUM]={
                                                                {0x80,0x00},
                                                                {0x81,0x01},
                                                                {0xC0,0x40},
                                                                {0xC1,0x41}
                                                         };

u8 abn484_eep_sdp_data[PCI_ADDCARD_MAX_SLOTS][EEPROM_SDP_NUM]={
                                                                {0x4C,0x0C},
                                                                {0x4D,0x0D},
                                                                {0x4E,0x0E},
                                                                {0x4F,0x0F}
                                                         };

u16 get_bypass_id(struct pci_dev *pdev)
{
	struct net_device *netdev=NULL;
	struct ethtool_eeprom *eep;
	u8 data[2]={0,0};
	u16 bypass_id=0;

	netdev=pci_get_drvdata(pdev);
	eep = kmalloc(sizeof(struct ethtool_eeprom) , GFP_KERNEL);
	if(!eep)
		return -ENOMEM;
	
	eep->cmd=ETHTOOL_GEEPROM;
	eep->len=0x2;
	eep->offset=GET_BYPASSID_OFFSET;
	netdev->ethtool_ops->get_eeprom(netdev,eep,data);
	bypass_id=(((data[1]<<8)|data[0]));
	kfree(eep);
	return bypass_id;
}

int get_slot_subordinate(int mb_idx)
{
	int i, j;
	int bidx,bus=0;
	int ret = -ENODEV;
	struct pci_dev *pdev = NULL;

	for (i=0; i<PCI_ADDCARD_MAX_SLOTS; i++){            // scan slot
		bidx = 0;
		for (j=0; j<SLOT_BRIDGE_NUM; j++){     // scan bridge per slot
			if (!mb_slots[mb_idx].slot[i][j].slotno)
				break;

			if(mb_slots[mb_idx].slot[i][j].extend){
			    for(bus=mb_slots[mb_idx].slot[i][j].bus+1;bus<255;bus++){	
				pdev = pci_scan_bus_and_slot(bus,
			                             mb_slots[mb_idx].slot[i][j].devfn);
			        if (pdev){
					mb_slot_sbus[i][bidx++] = pdev->subordinate;
					ret = 0;
			        }
			    }
			}else{
			    pdev = pci_scan_bus_and_slot(mb_slots[mb_idx].slot[i][j].bus,
			                             mb_slots[mb_idx].slot[i][j].devfn);
			    if (pdev){
				mb_slot_sbus[i][bidx++] = pdev->subordinate;
				ret = 0;
			    }
			}
		}
	}

	return ret;
}

int is_abn_series(struct pci_dev *pdev){
	if ((pdev->device==I82571EB_COPPER_DID) || \
		(pdev->device==I82571EB_FIBER_DID) || \
		(pdev->device==I82574L_COPPER_DID))
		return 1;
	else
		return 0;

}
/*
 * Search for LAN Controller device on each bus bridged to specified slot.
 */
void search_slot_dev(int slot_id, struct pci_bus *slot_bridge[],int mb_idx)
{
	int i = 0, j;
	int didx = 0,ret=0;
	u16 bypass_id=0;
	struct pci_dev *pdev;
	int slotn= mb_slots[mb_idx].slot[slot_id][0].slotno;
	for (i=0; i<SLOT_BRIDGE_NUM && slot_bridge[i]; i++){
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0) )
		for (j=slot_bridge[i]->busn_res.start; j<=slot_bridge[i]->busn_res.end; j++){
#else
		for (j=slot_bridge[i]->secondary; j<=slot_bridge[i]->subordinate; j++){
#endif
		if ((pdev = pci_scan_bus_and_slot(j, 0))!= NULL){
				
		    if( (pdev->class == CLASS_LANCTRL) && (is_abn_series(pdev)) ){
			bypass_id=get_bypass_id(pdev);
			if (bypass_id == BYPASSID_ABN484){
			    ret=abn484_set_eeprom_data(slotn-1,pdev);
			    slot_dev[slot_id][didx++] = pdev;
			}else if (bypass_id == BYPASSID_ABN482){ 
			    ret=abn482_set_eeprom_data(slotn-1,pdev);
			    slot_dev[slot_id][didx++] = pdev;
			}else if (bypass_id == BYPASSID_ABN484L){
			    slot_dev[slot_id][didx++] = pdev;
			}
		    }else if ( (pdev->class == CLASS_LANCTRL) && \
		       (pdev->subsystem_vendor == PORTWELL_VID) ){
			slot_dev[slot_id][didx++] = pdev;
		    }
		}
	    }
	}
}

int get_dev_iomap(int *didx, void __iomem* hw_addr[], struct pci_dev *sdev[], int dev_no)
{
	int i;
	int ret = -1;
	struct pci_dev *pdevf1;

	for (i=0; i<dev_no && sdev[i]; i++){
		// get fn(0)
		hw_addr[*didx] = ioremap(pci_resource_start(sdev[i], 0),
		                         pci_resource_len(sdev[i], 0));
		if(!hw_addr[*didx])
			goto out;
		else
			*didx += 1;

		// get fn(1)
		pdevf1 = pci_scan_bus_and_slot(sdev[i]->bus->number,
		                               sdev[i]->devfn + 0x1u);
		if (!pdevf1)
			goto out;
		hw_addr[*didx] = ioremap(pci_resource_start(pdevf1, 0),
		                       pci_resource_len(pdevf1, 0));
		if(!hw_addr[*didx])
			goto out;
		else
			*didx += 1;
	}

	ret = 0;
out:
	return ret;
}

void set_sdp(void __iomem* reg, uint32_t mask, uint8_t value)
{
	uint32_t val;

	val = readl(reg);
	// set pin as output
	val |= (mask << 4);
	// set bit
	if (value)
		val |= mask;
	else
		val &= ~mask;
	writel(val, reg);
}

uint16_t abn482_set_eeprom_data(uint8_t slotn, struct pci_dev *pdev)
{
	struct net_device *netdev=NULL;
	struct ethtool_eeprom *eep;
	u8 seeprom_value[2];
	u8 data[2];
	int i=0;
	netdev=pci_get_drvdata(pdev);
	eep = kmalloc(sizeof(struct ethtool_eeprom), GFP_KERNEL);
	if(!eep)
		return -ENOMEM;
		
	for(i=0;i<2;i++){
		eep->cmd=ETHTOOL_GEEPROM;
		eep->magic=(pdev->vendor|(pdev->device<<16));
		eep->len=0x1;
		eep->offset=eeprom_offset[i];
		netdev->ethtool_ops->get_eeprom(netdev,eep,data);
		if(data[0]!=abn482_eep_sdp_data[slotn][i]){
			eep->cmd=ETHTOOL_SEEPROM;
			seeprom_value[0]=abn482_eep_sdp_data[slotn][i];
			netdev->ethtool_ops->set_eeprom(netdev,eep,seeprom_value);
		}
	}
	kfree(eep);
	return 0;
}

uint16_t abn484_set_eeprom_data(uint8_t slotn, struct pci_dev *pdev)
{
	struct net_device *netdev=NULL;
	struct ethtool_eeprom *eep;
	u8 seeprom_value[2];
	u8 data[2],seg[2];
	int i=0;

	netdev=pci_get_drvdata(pdev);
	eep = kmalloc(sizeof(struct ethtool_eeprom), GFP_KERNEL);
	if(!eep)
		return -ENOMEM;
	eep->cmd=ETHTOOL_GEEPROM;
	eep->len=0x1;
	eep->offset=GET_BYPASSSEG_NUM;	// eeprom segment register
	netdev->ethtool_ops->get_eeprom(netdev,eep,seg);

	for(i=0;i<2;i++){
		eep->offset=eeprom_offset[i];
		// Seg number 0x10 is I82571EB first chip, 0x11 is I82671EB second chip.
		if (((seg[0]==0x11)&&(eep->offset==0x20))){
			;// Ignore the 0x20 address at second chip.
		}else{
			netdev->ethtool_ops->get_eeprom(netdev,eep,data);
			if(data[0]!=abn484_eep_sdp_data[slotn][i]){
				eep->cmd=ETHTOOL_SEEPROM;
				eep->magic=(pdev->vendor|(pdev->device<<16));
				seeprom_value[0]=abn484_eep_sdp_data[slotn][i];
				netdev->ethtool_ops->set_eeprom(netdev,eep,seeprom_value);
			}
		}
	}
	kfree(eep);
	return 0;
}

int abn484_address(uint8_t slotn, struct pci_dev *sdev[])
{
	void __iomem* hw_addr[4];
	int i, didx = 0;
	int ret = -1;

	if ((ret = get_dev_iomap(&didx, hw_addr, sdev, 2)))
		goto out;

	// set slot number (A1,A0) on SDP[1:0]
	// => will be all the same on pca9555_A, pca9555_B and pca9559
	for (i=0; i<3; i++){
		set_sdp(hw_addr[i]+INTEL_CTRL, INTEL_SDP0_DATA, (slotn % 4) & 0x01);
		set_sdp(hw_addr[i]+INTEL_CTRL, INTEL_SDP1_DATA, (slotn % 4) & 0x02);
	}
	// pca9555_A A2 SDP0[2]=0; pca9555_B A2 SDP1[2]=1
	set_sdp(hw_addr[0]+INTEL_CTRL_EXT, INTEL_SDP2_DATA, 0);
	set_sdp(hw_addr[1]+INTEL_CTRL_EXT, INTEL_SDP2_DATA, 1);

out:
	for (i=0; i<didx; i++)
		iounmap(hw_addr[i]);

	return ret;
}

int abn482_address(uint8_t slotn, struct pci_dev *sdev[])
{
	void __iomem* hw_addr[2];
	int i, didx = 0;
	int ret = -1;

	if ((ret = get_dev_iomap(&didx, hw_addr, sdev, 1)))
		goto out;

	// set slot number (A1,A0) on SDP[2:0]
	// => will be all the same on pca9555_A, pca9555_B
	for (i=0; i<2; i++){
		set_sdp(hw_addr[i]+INTEL_CTRL, INTEL_SDP0_DATA, (slotn % 4) & 0x01);
		set_sdp(hw_addr[i]+INTEL_CTRL_EXT, INTEL_SDP2_DATA, (slotn % 4) & 0x02);
	}
	// pca9555_A A2 SDP0[3]=0; pca9555_B A2 SDP1[3]=1
	set_sdp(hw_addr[0]+INTEL_CTRL_EXT, INTEL_SDP3_DATA, 0);
	set_sdp(hw_addr[1]+INTEL_CTRL_EXT, INTEL_SDP3_DATA, 1);

out:
	for (i=0; i<didx; i++)
		iounmap(hw_addr[i]);

	return ret;
}

int set_card_conf(int mb_idx)
{
	int i, shift;
	uint32_t cparam = 0;
	uint8_t sno;
	u16 bypass_id=0;
	int ret = 0;

	for (i=0; i<PCI_ADDCARD_MAX_SLOTS; i++){
		if (slot_dev[i][0]){
			sno = mb_slots[mb_idx].slot[i][0].slotno;
			shift = (sno - 1) * 4;
			switch (slot_dev[i][0]->subsystem_device){
			case 0x2001:
			case 0x2005:
				cparam |= 0x1 << shift;
				ret = abn484_address(sno-1, slot_dev[i]);
				break;
			case 0x2002:
				// Check if it is NIP55140 or not.
				if (slot_dev[i][0]->device == IX540_COPPER_DID)
					cparam |= 0x7 << shift;
				else if (slot_dev[i][0]->device != I82580EB_FIBER_DID)
					cparam |= 0x3 << shift;
				else
					cparam |= 0x4 << shift;
				break;
			case 0x2003:
			case 0x2007:
				ret = abn482_address(sno-1, slot_dev[i]);
			case 0x2006:
				cparam |= 0x2 << shift;
				break;
			case 0x2008:
				cparam |= 0x6 << shift;
				break;
			default:// The old ABN Series only needs to meet the bypass ID field.
				if(is_abn_series(slot_dev[i][0]))
					bypass_id=get_bypass_id(slot_dev[i][0]);
				switch(bypass_id){
				case BYPASSID_ABN484L:
					cparam |= 0x3 << shift;
					break;
				case BYPASSID_ABN484:
					ret = abn484_address(sno-1, slot_dev[i]);
					cparam |= 0x1 << shift;
					break;
				case BYPASSID_ABN482:
					ret = abn482_address(sno-1, slot_dev[i]);
					cparam |= 0x2 << shift;
					break;
				}
				break;
			}

			if (ret)
				printk("Slot[%d] Addressing Fail!\n", sno);
		}
	}
	return cparam;
}

int gen2bp_init(char *board, uint32_t *card_conf)
{
	int mb_idx, mb_cnt, i;
	int ret = -ENODEV;

	for (mb_idx=0; mb_idx<sizeof(mb_slots)/sizeof(struct bypass_slot); mb_idx++){
		for(mb_cnt=0;mb_cnt<MB_NAME_LEN;mb_cnt++){
			if(strcmp(board, mb_slots[mb_idx].boardname[mb_cnt])== 0){
				ret = 0;
				break;
			}
		}
		if (ret==0)
			break;
	}

	if (ret)
		goto out;

	ret = get_slot_subordinate(mb_idx);
	if (ret)
		goto out;

	for (i=0; i<PCI_ADDCARD_MAX_SLOTS; i++){
		search_slot_dev(i, mb_slot_sbus[i],mb_idx);
	}

	*card_conf = set_card_conf(mb_idx);
	printk("card_conf = 0x%08x\n", *card_conf);

out:
	return ret;
}
