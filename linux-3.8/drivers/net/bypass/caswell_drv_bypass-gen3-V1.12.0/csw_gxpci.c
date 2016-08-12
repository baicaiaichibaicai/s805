/*******************************************************************************

  CASwell(R) Gen3 Bypass Linux driver
  Copyright(c) 2012 Angus Cheng <angus.cheng@cas-well.com>
                    Zeno  Lai   <zeno.lai@cas-well.com>

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

#include "csw.h"
#include "gen3bp.h"
#include "csw_gxpci.h"

#define MAX_SLOTN_PER_BOARD      8
#define MAX_SEG_PER_CARD         8

DEFINE_MUTEX(tile_lock);
LIST_HEAD(csw_tilecard_list);

static u8 card_num = 0;
static u8 bpindex[MAX_SLOTN_PER_BOARD][MAX_SEG_PER_CARD];

static struct cas_disp_priv_t cas_disp_priv[MAX_SLOTN_PER_BOARD];

static const struct csw_pci_device_id csw_tilecard_tbl[] = {
	{ PCI_CWDEVICE_TILE(TILERA, GX36_DEV_ID_CANNON), csw_hw_gx36ep },
	{ 0, }
};

int cas_disp_read(int link_index, uint64_t opcmd, void *h2t_data, size_t h2t_size, void *t2h_data, size_t *t2h_size){
	unsigned long flags;
	const int POLLING_MSECS = 20;
	const int MAX_MSECS = 3000;
	int sleep_msecs = 0;
	int  ret = 0;
	struct cas_disp_priv_t *cas_priv = &cas_disp_priv[link_index];
	uint64_t complete;

	spin_lock_irqsave(&cas_priv->lock, flags);

	if (readl(&cas_priv->host_regs->status) == PCIE_STATUS_RUNNING){

		memset((void *)cas_priv->cmd->h2t_data, 0 , sizeof(uint8_t)*128);
		memset((void *)cas_priv->cmd->t2h_data, 0 , sizeof(uint8_t)*1024);

		writeq(opcmd, &cas_priv->cmd->opcmd);
		memcpy((void *)cas_priv->cmd->h2t_data, h2t_data, h2t_size);
		writeq(1, &cas_priv->cmd->complete);

		writeq(PCIE_HOST_INTR_CPL_AVAIL, cas_priv->irq);

		while((complete = readq(&cas_priv->cmd->complete)) == 1){
			if (sleep_msecs >= MAX_MSECS)
				break;
			mdelay(POLLING_MSECS);
			sleep_msecs += POLLING_MSECS;
		}

		if (sleep_msecs >= MAX_MSECS){
			ret = -1;
			printk("[CAS-BYPASS]: Command timeout\n");
		} else if (complete == 3){
			ret = -2;
			printk("[CAS-BYPASS]: Command failed\n");
		} else if (complete == 4){
			ret = -3;
		} else {
			ret = 0;
			memcpy(t2h_data, cas_priv->cmd->t2h_data, readq(&cas_priv->cmd->t2h_size));
			*t2h_size = readq(&cas_priv->cmd->t2h_size);
		}

		writeq(0, &cas_priv->cmd->complete);
	}

	spin_unlock_irqrestore(&cas_priv->lock, flags);

	return ret;
}

int cas_disp_write(int link_index, uint64_t opcmd, void *h2t_data, size_t h2t_size, void *t2h_data, size_t t2h_size){
	unsigned long flags;
	const int POLLING_MSECS = 20;
	const int MAX_MSECS = 3000;
	int sleep_msecs = 0;
	int  ret = 0;
	struct cas_disp_priv_t *cas_priv = &cas_disp_priv[link_index];
	uint64_t complete;

	spin_lock_irqsave(&cas_priv->lock, flags);

	if (readl(&cas_priv->host_regs->status) == PCIE_STATUS_RUNNING){

		memset((void *)cas_priv->cmd->h2t_data, 0 , sizeof(uint8_t)*128);
		memset((void *)cas_priv->cmd->t2h_data, 0 , sizeof(uint8_t)*1024);

		writeq(opcmd, &cas_priv->cmd->opcmd);
		memcpy((void *)cas_priv->cmd->h2t_data, h2t_data, h2t_size);
		memcpy((void *)cas_priv->cmd->t2h_data, t2h_data, t2h_size);
		writeq(1, &cas_priv->cmd->complete);

		writeq(PCIE_HOST_INTR_CPL_AVAIL, cas_priv->irq);

		while((complete = readq(&cas_priv->cmd->complete)) == 1){
			if (sleep_msecs >= MAX_MSECS)
				break;
			mdelay(POLLING_MSECS);
			sleep_msecs += POLLING_MSECS;
		}

		if (sleep_msecs >= MAX_MSECS){
			ret = -1;
			printk("[CAS-BYPASS]: Command timeout\n");
		} else if (complete == 3){
			ret = -2;
			printk("[CAS-BYPASS]: Command failed\n");
		} else if (complete == 4){
			ret = -3;
		} else {
			ret = 0;
		}

		writeq(0, &cas_priv->cmd->complete);
	}

	spin_unlock_irqrestore(&cas_priv->lock, flags);

	return ret;
}

s32 csw_tilecard_read(struct csw_device *csw, u16 cmd, u8 *data)
{
	s32 ret_val = 0;
	u16 cidx = ((csw->addr & 0xFFFF0000) >> 16) - 1;
	u16 segno = csw->addr & 0xFF;
	struct cas_bp_cmd_t bp_cmd;
	ssize_t t2h_size;

	if (cmd == BP_OPCODE_BPID){
		*data = bpindex[cidx][segno];
	} else {
		bp_cmd.cwid = (csw->addr & 0xFF00) >> 8;
		if ((cmd & 0xFFF0) == BP_OPCODE_BPID) {
			bp_cmd.cmd = BP_OPCODE_BPID;
		} else {
			bp_cmd.cmd = cmd;
		}
		ret_val = cas_disp_read(cidx, CAS_CMD_BP_READ_OPT, &bp_cmd, sizeof(struct cas_bp_cmd_t), data, &t2h_size);
	}

	return ret_val;
}

s32 csw_tilecard_write(struct csw_device *csw, u16 cmd, u8 data)
{
	s32 ret_val = 0;
	u16 cidx = ((csw->addr & 0xFFFF0000) >> 16) - 1;
	u16 segno = csw->addr & 0xFF;
	struct cas_bp_cmd_t bp_cmd;

	if (cmd == BP_OPCODE_BPID){
		bpindex[cidx][segno] = data;
	} else {
		bp_cmd.cwid = (csw->addr & 0xFF00) >> 8;
		bp_cmd.cmd = cmd;
		ret_val = cas_disp_write(cidx, CAS_CMD_BP_WRITE_OPT, &bp_cmd, sizeof(struct cas_bp_cmd_t), &data, sizeof(u8));
	}

	return ret_val;
}

static int check_tile_dev(struct csw_device *csw)
{
	int ret_val;
	u8 segno = 0;

	/*
	 * Upload bpyass segment number from Tilera
	 */
	ret_val = csw->ops.read(csw, BP_OPCODE_BPID | 1, &segno);
	if (ret_val)
		return -ENODEV;

	csw->addr |= segno;

	return ret_val;
}

static struct csw_device* csw_h2c_dev_init(struct pci_dev *pdev,
                                           u16 hwtype, int devn)
{
	int ret = 0;
	struct csw_device *csw;

	csw = kzalloc(sizeof(struct csw_device), GFP_KERNEL);
	csw->dev.parent = &pdev->dev;
	strlcpy(csw->name, "csw_tile", sizeof(csw->name));
#if 1
	csw->cwid = 0x30C0 & 0x00FF;
	csw->type = 0x30C0 & 0xFF00;
#else
	csw->cwid = pdev->subsystem_device & 0x00FF;
	csw->type = pdev->subsystem_device & 0xFF00;
#endif
	csw->class = CSW_CLASS_BYPASS;
	strlcpy(csw->hwif, "tile", sizeof(csw->hwif));
	csw->hwtype = hwtype;
	csw->addr = ((card_num + 1) << 16) | ((devn & 0xFF) << 8);

	csw->ops.read  = csw_tilecard_read;
	csw->ops.write = csw_tilecard_write;

	ret = check_tile_dev(csw);
	if (ret)
		goto err_dev;

	ret = csw_register_device(csw);
	if (ret)
		goto err_dev;

	mutex_lock(&tile_lock);
	list_add_tail(&csw->bus_list, &csw_tilecard_list);
	mutex_unlock(&tile_lock);
	return csw;

err_dev:
	kfree(csw);
	return NULL;
}

static void csw_tilecard_del(struct csw_device *csw)
{
	list_del(&csw->bus_list);
	csw_unregister_device(csw);
}

static struct csw_device* csw_tile_add_pci(struct pci_dev *pdev,
                                           const struct csw_pci_device_id *ent)
{
	struct csw_device *csw = NULL, *tmp = NULL;
	struct cas_disp_priv_t *cas_priv = &cas_disp_priv[card_num];
	struct tilera_bypass_info_t bp_info[40];
	u8 data;
	ssize_t t2h_size;
	int i;

	if (card_num >= MAX_SLOTN_PER_BOARD) {
		printk("[CAS-BYPASS]: Card %d - Exceed maximum number of slots\n", card_num);
		return NULL;
	}

	spin_lock_init(&cas_priv->lock);

	cas_priv->host_regs = ioremap(pci_resource_start(pdev, 0) + GXPCI_CAS_HOST_REGS_OFFSET, sizeof(struct cas_host_regs_t));
	if (cas_priv->host_regs == NULL){
		printk("[CAS-BYPASS]: Card %d - Host register ioremap failed\n", card_num);
		goto err_host;
	}

	if (cas_priv->host_regs->status != PCIE_STATUS_RUNNING) {
		printk("[CAS-BYPASS]: Card %d - Tilera side is not ready\n", card_num);
		return NULL;
	}

	cas_priv->cmd = ioremap(pci_resource_start(pdev, 0) + GXPCI_CAS_BP_OFFSET, sizeof(struct cas_cmd_t));
	if (cas_priv->cmd == NULL){
		printk("[CAS-BYPASS]: Card %d - Bypass Command ioremap failed\n", card_num);
		goto err_cmd;
	}

	cas_priv->irq = ioremap(pci_resource_start(pdev, 0) + GXPCI_CAS_MMI_REGS_OFFSET, sizeof(uint64_t));
	if (cas_priv->irq == NULL){
		printk("[CAS-BYPASS]: Card %d - Bypass MMI ioremap failed\n", card_num);
		iounmap(cas_priv->cmd);
		goto err_irq;
	}

	/* Check if bypass card has registered */
	cas_disp_read(card_num, CAS_CMD_BP_GET_STATUS, 0, 0, &data, &t2h_size);
	if (data == TILE_FUNC_REG) {
		goto err_init;
	}

	/* Get the information of all bypass devices */
	cas_disp_read(card_num, CAS_CMD_BP_GET_INFO, 0, 0, &bp_info, &t2h_size);
	for (i = 0; i < (t2h_size/sizeof(struct tilera_bypass_info_t)); i++) {
		if ((tmp = csw_h2c_dev_init(pdev, (u16) ent->driver_data, ((bp_info[i].instance << 4) | bp_info[i].channel))) == NULL) {
			continue;
		}
		csw = tmp;
	}

	if (csw == NULL) {
		goto err_init;
	}

	/* Register bypass card */
	data = TILE_FUNC_REG;
	cas_disp_write(card_num, CAS_CMD_BP_SET_STATUS, 0, 0, &data, sizeof(u8));


	card_num++;

	return csw;

err_init:
	iounmap(cas_priv->irq);
err_irq:
	iounmap(cas_priv->cmd);
	cas_priv->cmd = NULL;
err_cmd:
	iounmap(cas_priv->host_regs);
err_host:
	return NULL;
}

int __devinit csw_tilecard_init(void)
{
	int retval = 0;
	int i, j;

	// Initialize bp index
	for (i = 0; i < MAX_SLOTN_PER_BOARD; i++) {
		for (j = 0; j < MAX_SEG_PER_CARD; j++) {
			bpindex[i][j] = 0xFF;
		}
	}

	retval = csw_pci_probe(csw_tilecard_tbl, csw_tile_add_pci);

	return retval;
}

void csw_tilecard_exit(void)
{
	struct csw_device *csw, *tmp;
	int i;
	u8 data;

	mutex_lock(&tile_lock);
	list_for_each_entry_safe(csw, tmp, &csw_tilecard_list, bus_list)
		csw_tilecard_del(csw);

	for (i = 0; i < MAX_SLOTN_PER_BOARD; i++){
		if (cas_disp_priv[i].cmd != NULL) {
			/* Unregister bypass card */
			data = TILE_FUNC_UNREG;
			cas_disp_write(i, CAS_CMD_BP_SET_STATUS, 0, 0, &data, sizeof(u8));

			iounmap(cas_disp_priv[i].host_regs);
			iounmap(cas_disp_priv[i].cmd);
			iounmap(cas_disp_priv[i].irq);
		}
	}

	mutex_unlock(&tile_lock);
}


