/*******************************************************************************

  CASwell(R) CSW SPI Linux driver
  Copyright(c) 2012 Zeno Lai <zeno.lai@cas-well.com>

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
#include "csw_spi.h"
#include "gen3bp.h"

#include "intel_defines.h"
#include "amd_defines.h"
//#include "tilera_defines.h"

static DEFINE_MUTEX(list_lock);
LIST_HEAD(csw_spi_list);

static DEFINE_MUTEX(onboard_lock);
static int onboard_flag = 0;
static u8 __iomem* onboard_mmio;

static const struct csw_pci_device_id csw_pci_tbl[] = {
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_82583V), csw_hw_i82583 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_82580_COPPER), csw_hw_i82580 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_82580_FIBER), csw_hw_i82580 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_82580_SERDES), csw_hw_i82580 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_82580_SGMII), csw_hw_i82580 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_82580_COPPER_DUAL), csw_hw_i82580 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_I350_COPPER), csw_hw_i82580 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_I350_FIBER), csw_hw_i82580 },
	{ PCI_CWDEVICE(INTEL, IXGBE_DEV_ID_82599_SFP), csw_hw_i82599 },	
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_I210_COPPER), csw_hw_i210 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_X540_COPPER), csw_hw_x540 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_I354_SGMII), csw_hw_i354 },
	{ PCI_CWDEVICE(INTEL, E1000_DEV_ID_DH89XXCC_SGMII), csw_hw_dh89xxcc},
	{ PCI_CWDEVICE(REALTEK, RTL8168_DEV_ID), csw_hw_rtl8168 },
	{ 0, }
};

s32 csw_spi_read_microcon(struct csw_device *csw, u16 cmd, u8 *data);
s32 csw_spi_write_microcon(struct csw_device *csw, u16 cmd, u8 data);

static u32 get_ioreg32(u16 io_idx, u16 io_data, u16 addr)
{
	u32 value = 0;
	int i;

	for (i=0; i<4; i++){
		outb(addr++, io_idx);
		value |= inb(io_data) << (i * 8);
	}

	return value;
}

static int check_spi_dev(struct csw_device *csw)
{
	int ret_val = 0;
#ifndef HW_VERIFICATION
	u8 hw_ena;
	/* Check H/W enable setting */
	ret_val = csw->ops.read(csw, BP_OPCODE_HW_ENABLE, &hw_ena);
	if (ret_val) goto err_dev;
	if (hw_ena != 0x1) {
		ret_val = -ENODEV;
	}
	CW_SPI_DEBUG("check_spi_dev:%d\n", ret_val);

err_dev:
#endif
	return ret_val;
}

static int a50m_gpio_init(struct csw_device *csw)
{
	int ret = 0;
	u32 acpimmio_addr;
	u8 __iomem* iomux_mmio;
	u8  value;

	csw->csw_mutex = &onboard_lock;

	// enable AcpiMMio space
	if (!onboard_flag){
		outb(ACPI_MMIO_EN, PM_INDEX);
		value = (inb(PM_DATA) | ACPI_MMIO_DECODE_EN) & ~ACPI_MMIO_SEL;
		outb(value, PM_DATA);
		acpimmio_addr = get_ioreg32(PM_INDEX, PM_DATA, ACPI_MMIO_EN) &
		                0xFFFFF000;
		if (acpimmio_addr == 0)
			return -ENODEV;
		onboard_mmio = ioremap(acpimmio_addr + A50M_GPIO_BAR,
		                       A50M_GPIO_MEMSIZE);
		if (onboard_mmio == NULL)
			return -ENODEV;

		// initial IoMux
		iomux_mmio = ioremap(acpimmio_addr + A50M_IOMUX_BAR,
		                     A50M_GPIO_MEMSIZE);
		if (iomux_mmio == NULL)
			return -ENODEV;

		value = 0x01;
		pci_reg_writeb(iomux_mmio, A50M_SS_REG, value);
		pci_reg_writeb(iomux_mmio, A50M_SK_REG, value);
		pci_reg_writeb(iomux_mmio, A50M_SI_REG, value);
		pci_reg_writeb(iomux_mmio, A50M_SO_REG, value);
		iounmap(iomux_mmio);

		onboard_flag = 1;
	}

	csw->hw_addr = onboard_mmio;

	// initial direction
	value = pci_reg_readb(csw->hw_addr, A50M_SS_REG);
	value = (value & ~A50M_GPIO_OUT_ENB) | A50M_GPIO_OUT;
	pci_reg_writeb(csw->hw_addr, A50M_SS_REG, value);

	value = pci_reg_readb(csw->hw_addr, A50M_SK_REG);
	value &= ~(A50M_GPIO_OUT_ENB|A50M_GPIO_OUT);
	pci_reg_writeb(csw->hw_addr, A50M_SK_REG, value);

	value = pci_reg_readb(csw->hw_addr, A50M_SI_REG);
	value &= ~(A50M_GPIO_OUT_ENB|A50M_GPIO_OUT);
	pci_reg_writeb(csw->hw_addr, A50M_SI_REG, value);

	value = pci_reg_readb(csw->hw_addr, A50M_SO_REG);
	value |= A50M_GPIO_OUT_ENB;
	pci_reg_writeb(csw->hw_addr, A50M_SO_REG, value);

	return ret;
}

static struct csw_device* csw_spi_add_pci(struct pci_dev *pdev,
                                          const struct csw_pci_device_id *ent)
{
	int ret = 0;
	struct csw_device *csw;
	u8 vpd[VPD_SIZE];
	u16 subdevice = 0;
	int kw_addr = -ENOENT, r_des_addr = -ENOENT;
	int start = 0;

	if (pdev->subsystem_vendor == RVB) {
		ret = pci_read_vpd(pdev, start, VPD_SIZE, vpd);
		if (ret == -ENODEV) {
			return NULL;
		}

		r_des_addr = pci_vpd_find_tag(vpd, start, VPD_SIZE, READONLY_VPD_DESCRIPTOR);
		if (r_des_addr == -ENOENT) {
			return NULL;
		}

		kw_addr = pci_vpd_find_info_keyword(vpd, r_des_addr, VPD_SIZE, CSW_SID_VPD_KW);
		if (kw_addr == -ENOENT) {
			return NULL;
		}


		subdevice = (u8)(vpd[kw_addr + 4]) << 8 | (u8)(vpd[kw_addr + 3]);
	} else {
		subdevice = pdev->subsystem_device;
	}

	if ((subdevice & 0xF000) != 0x3000)
		return NULL;

	if (ent->driver_data == csw_hw_dh89xxcc) {
		if ( (subdevice & 0x00F0) &
			 (0x1u << ((PCI_FUNC(pdev->devfn)-1)/2 + 4)) )
			return NULL;
	}
	else {
		if ( (subdevice & 0x00F0) &
			 (0x1u << (PCI_FUNC(pdev->devfn)/2 + 4)) )
			return NULL;
	}

	csw = kzalloc(sizeof(struct csw_device), GFP_KERNEL);
	csw->dev.parent = &pdev->dev;
	csw->p = &pdev->dev;
	strlcpy(csw->name, "csw_spi", sizeof(csw->name));
	csw->cwid = subdevice & 0x00FF;
	csw->class = CSW_CLASS_BYPASS;
	csw->type = subdevice & 0xFF00;
	strlcpy(csw->hwif, "pci", sizeof(csw->hwif));
	csw->hwtype = (u16) ent->driver_data;
	csw->addr = (pci_domain_nr(pdev->bus) << 20) | (pdev->bus->number << 12)
	             | (PCI_SLOT(pdev->devfn) << 4) | PCI_FUNC(pdev->devfn);
	if (csw->hwtype < csw_hw_onlan){
		csw->hw_addr = ioremap(pci_resource_start(pdev, 0),
		                       pci_resource_len(pdev, 0));

		if (!csw->hw_addr)
			goto err_ioremap;
	}

	// initial SPI interface if needed
	if (((csw->cwid & 0x000F) >> 1) == csw_amd_a50m)
		if (a50m_gpio_init(csw))
			goto err_dev;

	csw->ops.read  = csw_spi_read_microcon;
	csw->ops.write = csw_spi_write_microcon;

	ret = check_spi_dev(csw);
	if (ret)
		goto err_dev;

	ret = csw_register_device(csw);
	if (ret)
		goto err_dev;

	mutex_lock(&list_lock);
	list_add_tail(&csw->bus_list, &csw_spi_list);
	mutex_unlock(&list_lock);
	return csw;

err_dev:
	if (csw->hwtype < csw_hw_onlan)
		iounmap(csw->hw_addr);
err_ioremap:
	kfree(csw);
	return NULL;
}

#ifdef _TILERA_DEFINES_H_
static int csw_spi_add_gxio(int portn, _gxio_mpipe_link_phy_ext_cfg_t *port_info)
{
	int ret = 0;
	struct csw_device *csw;

	csw = kzalloc(sizeof(struct csw_device), GFP_KERNEL);
/* TODO: configure parent and port
	csw->dev.parent = &pdev->dev;
	csw->p = &pdev->dev;
*/

	// Register csw
	strlcpy(csw->name, "csw_spi", sizeof(csw->name));
	csw->cwid = GX36_CSW_ID;
	csw->class = CSW_CLASS_BYPASS;
	csw->type = GX36_CSW_BPTYPE;
	strlcpy(csw->hwif, "gpio", sizeof(csw->hwif));
	if (port_info == NULL) {
		csw->hwtype = csw_hw_tilegx;
		csw->addr = (portn * 4) << 8 | (portn / 2);
	} else {
		csw->hwtype = csw_hw_tilegx + (port_info->bp_spi_idx - 1);
		csw->addr =  port_info->instance << 12 | port_info->channel << 8 | (port_info->bp_spi_idx - 1) << 4 | (port_info->bp_seg_idx - 1);
	}
	csw->csw_mutex = &onboard_lock;

	csw->ops.read  = csw_spi_read_microcon;
	csw->ops.write = csw_spi_write_microcon;

	ret = check_spi_dev(csw);
	if (ret)
		goto err;

	ret = csw_register_device(csw);
	if (ret)
		goto err;

	mutex_lock(&list_lock);
	list_add_tail(&csw->bus_list, &csw_spi_list);
	mutex_unlock(&list_lock);
	return ret;

err:
	kfree(csw);
	return -ENODEV;
}
#endif

static int csw_gpio_probe(void)
{
	int ret = 0;

#ifdef _TILERA_DEFINES_H_
	int i = 0, devno = 0, portn = 0, old_flag = 1;
	char bp_idx_set = 0;
	unsigned char tile_xaui[TILEGX_MAX_XAUI][TILEGX_MAX_CHAN];
	_gxio_mpipe_link_phy_ext_cfg_t port_info[TILEGX_XAUI_PORTS];

	memset(tile_xaui, 0, sizeof(char) * TILEGX_MAX_XAUI * TILEGX_MAX_CHAN);

	for (i = 0; i < TILEGX_XAUI_PORTS; i++) {
		if ((ret = gxio_mpipe_link_get_phy_ext_cfg(i, &port_info[portn])) >= 0) {
			if (port_info[portn].has_phy_ext_cfg) {
				old_flag = 0;
				if ((port_info[portn].bp_spi_idx > 0) && (port_info[portn].bp_seg_idx > 0)) {
					if (tile_xaui[port_info[portn].instance][port_info[portn].channel] == 1) {
						continue;
					}
					bp_idx_set |= 1 << (port_info[portn].bp_spi_idx - 1);
					tile_xaui[port_info[portn].instance][port_info[portn].channel] = 1;
					portn++;
				}
			}
		}
	}

	if (old_flag) {	
		/*
		 * init code for old model without BIB bypass information
		 */
		ret = tilegx_gpio_init(0x01);
		if (ret) {
			goto out;
		}
		for (i = 0; i < 4; i++){
			ret = csw_spi_add_gxio(i, NULL);
			if (!ret) {
				devno++;
			}
		}
	} else {
		/*
		 * init code for model according to BIB bypass information
		 */
		ret = tilegx_gpio_init(bp_idx_set);
		if (ret) {
			goto out;
		}
		for (i = 0; i < portn; i++) {
			ret = csw_spi_add_gxio(i, &port_info[i]);
			if (!ret) {
				devno++;
			}
		}
	}

	if (devno) {
		ret = 0;
	} else {
		tilegx_gpio_exit();
		ret = -ENODEV;
	}

out:
#endif
	return ret;
}

static void csw_spi_del(struct csw_device *csw)
{
	list_del(&csw->bus_list);
	if (csw->hwtype < csw_hw_onlan)
		iounmap(csw->hw_addr);
#ifdef _TILERA_DEFINES_H_
	else if ((csw->hwtype & 0xFFF0) == csw_hw_tilegx)
		tilegx_gpio_exit();
#endif
	if (onboard_flag){
		iounmap(onboard_mmio);
		onboard_flag = 0;
	}
	csw_unregister_device(csw);
}

int __devinit csw_spi_dev_init(void)
{
	int retval = 0;

	retval = csw_pci_probe(csw_pci_tbl, csw_spi_add_pci);
	retval = csw_gpio_probe();

	return retval;
}

void csw_spi_dev_exit(void)
{
	struct csw_device *csw, *tmp;

	mutex_lock(&list_lock);
	list_for_each_entry_safe(csw, tmp, &csw_spi_list, bus_list)
		csw_spi_del(csw);
	mutex_unlock(&list_lock);
}

/* ----------------------------------------------
 * the functional interface to the SPI busses.
 * ----------------------------------------------
 */
/* SPI control type */
const static struct csw_spi_cpld spi_cpld = {
	.busy     = 0,
	.segcode  = 0,
	.clock    = BP_CPLD_CLK,
	.attempts = BP_CPLD_ATTEMPTS,
	.protocol = 0,
};

const static struct csw_spi_config spi_cfg = {
	.clk_usec   = BP_MCU_CLK,
	.byte_delay = BP_MCU_BYTE_DELAY,
};

const static struct csw_spi_ctrl spi_ctrl_fla_e1000 = {
	.spi_sck     = { INIC_FLA_SCK, E1000_FLA },
	.spi_ss      = { INIC_FLA_CE,  E1000_FLA },
	.spi_mosi    = { INIC_FLA_SI,  E1000_FLA },
	.spi_miso    = { INIC_FLA_SO,  E1000_FLA },
	.spi_rqst    = { INIC_FLA_REQ, E1000_FLA },
	.spi_gnt     = { INIC_FLA_GNT, E1000_FLA },
	.spi_swfw_sm = E1000_SWFW_FLA_SM,
};

const static struct csw_spi_ctrl spi_ctrl_fla_ixgbe = {
	.spi_sck     = { INIC_FLA_SCK, IXGBE_FLA },
	.spi_ss      = { INIC_FLA_CE,  IXGBE_FLA },
	.spi_mosi    = { INIC_FLA_SI,  IXGBE_FLA },
	.spi_miso    = { INIC_FLA_SO,  IXGBE_FLA },
	.spi_rqst    = { INIC_FLA_REQ, IXGBE_FLA },
	.spi_gnt     = { INIC_FLA_GNT, IXGBE_FLA },
	.spi_swfw_sm = IXGBE_GSSR_FLASH_SM,
};

const static struct csw_spi_ctrl spi_ctrl_eecd_e1000 = {
	.spi_sck     = { INIC_EECD_SK,  E1000_EECD },
	.spi_ss      = { INIC_EECD_CS,  E1000_EECD },
	.spi_mosi    = { INIC_EECD_DI,  E1000_EECD },
	.spi_miso    = { INIC_EECD_DO,  E1000_EECD },
	.spi_rqst    = { INIC_EECD_REQ, E1000_EECD },
	.spi_gnt     = { INIC_EECD_GNT, E1000_EECD },
	.spi_swfw_sm = E1000_SWFW_EEC_SM,
};

const static struct csw_spi_ctrl spi_ctrl_eecd_ixgbe = {
	.spi_sck     = { INIC_EECD_SK,  IXGBE_EECD },
	.spi_ss      = { INIC_EECD_CS,  IXGBE_EECD },
	.spi_mosi    = { INIC_EECD_DI,  IXGBE_EECD },
	.spi_miso    = { INIC_EECD_DO,  IXGBE_EECD },
	.spi_rqst    = { INIC_EECD_REQ, IXGBE_EECD },
	.spi_gnt     = { INIC_EECD_GNT, IXGBE_EECD },
	.spi_swfw_sm = IXGBE_GSSR_EEP_SM,
};

const static struct csw_spi_ctrl spi_ctrl_gpio_a50m = {
	.spi_sck     = {A50M_GPIO_OUT, A50M_SK_REG},
	.spi_ss      = {A50M_GPIO_OUT, A50M_SS_REG},
	.spi_mosi    = {A50M_GPIO_OUT, A50M_SI_REG},
	.spi_miso    = {A50M_GPIO_IN, A50M_SO_REG},
};

#ifdef _TILERA_DEFINES_H_
const static struct csw_spi_ctrl spi_ctrl_gpio_gx[] = {
	{
		.spi_sck     = {GX_GPIO_SK_0, GX_SPI_OUT_0},
		.spi_ss      = {GX_GPIO_SS_0, GX_SPI_OUT_0},
		.spi_mosi    = {GX_GPIO_SI_0, GX_SPI_OUT_0},
		.spi_miso    = {GX_GPIO_SO_0, GX_SPI_OUT_0},
	}, {
		.spi_sck     = {GX_GPIO_SK_1, GX_SPI_OUT_1},
		.spi_ss      = {GX_GPIO_SS_1, GX_SPI_OUT_1},
		.spi_mosi    = {GX_GPIO_SI_1, GX_SPI_OUT_1},
		.spi_miso    = {GX_GPIO_SO_1, GX_SPI_OUT_1},
	}
};
#endif

s32 csw_spi_acquire_intel(struct csw_spi_master *master);
void csw_spi_release_intel(struct csw_spi_master *master);

static s32 get_hw_semaphore_82573(struct csw_spi_master *master);
static void put_hw_semaphore_82573(struct csw_spi_master *master);
static s32 get_hw_semaphore_82575(struct csw_spi_master *master);
static void put_hw_semaphore_82575(struct csw_spi_master *master);
static s32 get_hw_semaphore_82599(struct csw_spi_master *master);
static void put_hw_semaphore_82599(struct csw_spi_master *master);
static s32 get_hw_semaphore_i210(struct csw_spi_master *master);
static void put_hw_semaphore_i210(struct csw_spi_master *master);
static s32 get_hw_semaphore_x540(struct csw_spi_master *master);
static void put_hw_semaphore_x540(struct csw_spi_master *master);
static s32 get_sw_ownership_82583(struct csw_spi_master *master);
static void put_sw_ownership_82583(struct csw_spi_master *master);

static s32 acquire_swfw_sync_82575(struct csw_spi_master *master, u16 mask);
static void release_swfw_sync_82575(struct csw_spi_master *master, u16 mask);
static s32 acquire_swfw_sync_82599(struct csw_spi_master *master, u16 mask);
static void release_swfw_sync_82599(struct csw_spi_master *master, u16 mask);
static s32 acquire_swfw_sync_i210(struct csw_spi_master *master, u16 mask);
static void release_swfw_sync_i210(struct csw_spi_master *master, u16 mask);
static s32 acquire_swfw_sync_x540(struct csw_spi_master *master, u16 mask);
static void release_swfw_sync_x540(struct csw_spi_master *master, u16 mask);

static s32 csw_spi_acquire_82583(struct csw_spi_master *master);
static void csw_spi_release_82583(struct csw_spi_master *master);
static s32 csw_spi_acquire_82575(struct csw_spi_master *master);
static void csw_spi_release_82575(struct csw_spi_master *master);
static s32 csw_spi_acquire_82599(struct csw_spi_master *master);
static void csw_spi_release_82599(struct csw_spi_master *master);
static s32 csw_nvm_acquire_i210(struct csw_spi_master *master);
static void csw_nvm_release_i210(struct csw_spi_master *master);
static s32 csw_spi_acquire_x540(struct csw_spi_master *master);
static void csw_spi_release_x540(struct csw_spi_master *master);
static s32 csw_spi_acquire_generic(struct csw_spi_master *master);
static void csw_spi_release_generic(struct csw_spi_master *master);

/* HW semaphore */
static s32 get_hw_semaphore_82573(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 extcnf_ctrl;
	s32 ret_val = 0;
	s32 i = 0;

	CW_SPI_DEBUG("get_hw_semaphore_82573\n");

	extcnf_ctrl = ops->read_reg(master->hw_addr, E1000_EXTCNF_CTRL);
	extcnf_ctrl |= E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP;
	do{
		ops->write_reg(master->hw_addr, E1000_EXTCNF_CTRL, extcnf_ctrl);
		extcnf_ctrl = ops->read_reg(master->hw_addr, E1000_EXTCNF_CTRL);

		if (extcnf_ctrl & E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP)
			break;

		extcnf_ctrl |= E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP;

		usleep_range(2000, 4000);
		i++;
	} while ( i < MDIO_OWNERSHIP_TIMEOUT);

	if (i == MDIO_OWNERSHIP_TIMEOUT){
		/* Release semaphore */
		put_hw_semaphore_82573(master);
		CW_SPI_DEBUG("Driver can't access the SPI\n");
		ret_val = -NIC_ERR;
		goto out;
	}

out:
	return ret_val;
}

static void put_hw_semaphore_82573(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 extcnf_ctrl;

	CW_SPI_DEBUG("put_hw_semaphore_82573\n");

	extcnf_ctrl = ops->read_reg(master->hw_addr, E1000_EXTCNF_CTRL);
	extcnf_ctrl &= ~E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP;
	ops->write_reg(master->hw_addr, E1000_EXTCNF_CTRL, extcnf_ctrl);
}

static s32 get_hw_semaphore_82575(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swsm;
	s32 ret_val = NIC_SUCCESS;
	s32 timeout = HW_SM_ATTEMPTS;
	s32 i = timeout;

	CW_SPI_DEBUG("get_hw_semaphore_82575\n");

	/* Get the SW semaphore */
	while (i){
		swsm = ops->read_reg(master->hw_addr, E1000_SWSM) & E1000_SWSM_SMBI;
		if (!swsm)
			break;
		udelay(50);
		i--;
	}

	if (!i){
		CW_SPI_DEBUG("Driver can't access device - SMBI bit is set.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

	/* Get the FW semaphore */
	for (i=0; i<timeout; i++){
		swsm = ops->read_reg(master->hw_addr, E1000_SWSM) | E1000_SWSM_SWESMBI;
		ops->write_reg(master->hw_addr, E1000_SWSM, swsm);

		/* Semaphore acquired if bit latched */
		if (ops->read_reg(master->hw_addr, E1000_SWSM) & E1000_SWSM_SWESMBI)
			break;

		udelay(50);
	}

	if (i == timeout){
		put_hw_semaphore_82575(master);
		CW_SPI_DEBUG("Driver can't access the SPI.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

out:
	return ret_val;
}

static void put_hw_semaphore_82575(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swsm;

	CW_SPI_DEBUG("put_hw_semaphore_82575\n");

	/* Clear SW/FW semaphore */
	swsm = ops->read_reg(master->hw_addr, E1000_SWSM);
	swsm &= ~(E1000_SWSM_SMBI | E1000_SWSM_SWESMBI);
	ops->write_reg(master->hw_addr, E1000_SWSM, swsm);
}

static s32 get_hw_semaphore_82599(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swsm;
	s32 ret_val = NIC_SUCCESS;
	s32 timeout = HW_SM_ATTEMPTS;
	s32 i = timeout;

	CW_SPI_DEBUG("get_hw_semaphore_82599\n");

	/* Get the SW semaphore */
	while (i){
		swsm = ops->read_reg(master->hw_addr, IXGBE_SWSM) & IXGBE_SWSM_SMBI;
		if (!swsm)
			break;
		udelay(50);
		i--;
	}

	if (!i){
		CW_SPI_DEBUG("Driver can't access device - SMBI bit is set.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

	/* Get the FW semaphore */
	for (i=0; i<timeout; i++){
		swsm = ops->read_reg(master->hw_addr, IXGBE_SWSM) | IXGBE_SWSM_SWESMBI;
		ops->write_reg(master->hw_addr, IXGBE_SWSM, swsm);

		/* Semaphore acquired if bit latched */
		if (ops->read_reg(master->hw_addr, IXGBE_SWSM) & IXGBE_SWSM_SWESMBI)
			break;

		udelay(50);
	}

	if (i == timeout){
		put_hw_semaphore_82599(master);
		CW_SPI_DEBUG("Driver can't access the SPI.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

out:
	return ret_val;
}

static void put_hw_semaphore_82599(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swsm;

	CW_SPI_DEBUG("put_hw_semaphore_82599\n");

	/* Clear SW/FW semaphore */
	swsm = ops->read_reg(master->hw_addr, IXGBE_SWSM);
	swsm &= ~(IXGBE_SWSM_SMBI | IXGBE_SWSM_SWESMBI);
	ops->write_reg(master->hw_addr, IXGBE_SWSM, swsm);
}

static s32 get_hw_semaphore_i210(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swsm;
	s32 ret_val = NIC_SUCCESS;
	s32 timeout = HW_SM_ATTEMPTS;
	s32 i = 0;

	CW_SPI_DEBUG("get_hw_semaphore_i210\n");

	/* Get the SW semaphore */
	while (i < timeout) {
		swsm = ops->read_reg(master->hw_addr, E1000_SWSM);
		if (!(swsm & E1000_SWSM_SMBI))
			break;

		udelay(50);
		i++;
	}

	if (i == timeout) {
		CW_SPI_DEBUG("Driver can't access device - SMBI bit is set.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

	/* Get the FW semaphore */
	for (i = 0; i < timeout; i++){
		swsm = ops->read_reg(master->hw_addr, E1000_SWSM) | E1000_SWSM_SWESMBI;
		ops->write_reg(master->hw_addr, E1000_SWSM, swsm);

		/* Semaphore acquired if bit latched */
		if (ops->read_reg(master->hw_addr, E1000_SWSM) & E1000_SWSM_SWESMBI)
			break;

		udelay(50);
	}

	if (i == timeout){
		put_hw_semaphore_i210(master);
		CW_SPI_DEBUG("Driver can't access the SPI.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

out:
	return ret_val;
}

static void put_hw_semaphore_i210(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swsm;

	CW_SPI_DEBUG("put_hw_semaphore_i210\n");

	/* Clear SW/FW semaphore */
	swsm = ops->read_reg(master->hw_addr, E1000_SWSM);
	swsm &= ~(E1000_SWSM_SMBI | E1000_SWSM_SWESMBI);
	ops->write_reg(master->hw_addr, E1000_SWSM, swsm);
}

static s32 get_hw_semaphore_x540(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	s32 status = IXGBE_ERR_EEPROM;
	u32 timeout = HW_SM_ATTEMPTS;
	u32 i = 0;
	u32 swsm;

	CW_SPI_DEBUG("get_hw_semaphore_x540\n");

    /* Get SMBI software semaphore between device drivers first */
	while (i < timeout) {
    /*
         * If the SMBI bit is 0 when we read it, then the bit will be
         * set and we have the semaphore
         */
		swsm = ops->read_reg(master->hw_addr, IXGBE_SWSM);
		if (!(swsm & IXGBE_SWSM_SMBI)) {
			status = 0;
			break;
		}
		udelay(50);
		i++;
	}

	if (i == timeout) {
		CW_SPI_DEBUG("Driver can't access device - SMBI bit is set.\n");
		status = -NIC_ERR;
		goto out;
	}

    /* Now get the semaphore between SW/FW through the REGSMP bit */
	if (status == 0) {
		for (i = 0; i < timeout; i++) {
			swsm = ops->read_reg(master->hw_addr, IXGBE_GSSR);
			if (!(swsm & IXGBE_SWFW_REGSMP))
				break;

			udelay(50);
		}
       /*
         * Release semaphores and return error if SW NVM semaphore * was not granted because we don't have access to the EEPROM
         */
		if (i >= timeout) {
			CW_SPI_DEBUG("REGSMP Software NVM semaphore not granted.\n");
			put_hw_semaphore_x540(master);
			status = IXGBE_ERR_EEPROM;
		}
	} else {
		CW_SPI_DEBUG("Software semaphore SMBI between device drivers not granted.\n");
	}

out:
    return status;
}

static void put_hw_semaphore_x540(struct csw_spi_master *master)
{
    struct csw_spi_operations *ops = &master->ops;
    u32 swsm;

    CW_SPI_DEBUG("put_hw_semaphore_x540\n");

    /* Release both semaphores by writing 0 to the bits REGSMP and SMBI */

    swsm = ops->read_reg(master->hw_addr, IXGBE_SWSM);
    swsm &= ~IXGBE_SWSM_SMBI;
    ops->write_reg(master->hw_addr, IXGBE_SWSM, swsm);

    swsm = ops->read_reg(master->hw_addr, IXGBE_GSSR);
    swsm &= ~IXGBE_SWFW_REGSMP;
    ops->write_reg(master->hw_addr, IXGBE_GSSR, swsm);

    intel_flush(master->hw_addr);
}

static s32 get_sw_ownership_82583(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 extcnf_ctrl;
	s32 ret_val = 0;
	s32 i = 0;

	CW_SPI_DEBUG("get_sw_ownership_82583\n");

	while (i < MDIO_OWNERSHIP_TIMEOUT){
		if (get_hw_semaphore_82575(master)){
			ret_val = -NIC_ERR;
			goto out;
		}

		/* Check if software has the ownership */
		extcnf_ctrl = ops->read_reg(master->hw_addr, E1000_EXTCNF_CTRL);
		if (!(extcnf_ctrl & E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP))
			break;

		/* Other software thread currently using resource */
		put_hw_semaphore_82575(master);
		mdelay(5);
		i++;
	}

	if (i == MDIO_OWNERSHIP_TIMEOUT){
		CW_SPI_DEBUG("Driver can't access resource, SW_OWNERSHIP timeout.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

	ret_val = get_hw_semaphore_82573(master);
	put_hw_semaphore_82575(master);

out:
	return ret_val;
}

static void put_sw_ownership_82583(struct csw_spi_master *master)
{
	CW_SPI_DEBUG("put_sw_ownership_82583\n");

	while (get_hw_semaphore_82575(master)!= NIC_SUCCESS);

	put_hw_semaphore_82573(master);
	put_hw_semaphore_82575(master);
}

/* SW/FW ownership */
static s32 acquire_swfw_sync_82575(struct csw_spi_master *master, u16 mask)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swfw_sync;
	u32 swmask = mask;
	u32 fwmask = mask << 16;
	s32 ret_val = NIC_SUCCESS;
	s32 i = 0, timeout = 200;

	CW_SPI_DEBUG("acquire_swfw_sync_82575\n");

	while (i < timeout){
		if (get_hw_semaphore_82575(master)){
			ret_val = -NIC_ERR;
			goto out;
		}

		/* Check if software or firmware has the ownership */
		swfw_sync = ops->read_reg(master->hw_addr, E1000_SW_FW_SYNC);
		if (!(swfw_sync & (swmask | fwmask)))
			break;

		/*
		 * Firmware currently using resource (fwmask)
		 * or other software thread using resource (swmask)
		 */
		put_hw_semaphore_82575(master);
		mdelay(5);
		i++;
	}

	if (i == timeout){
		CW_SPI_DEBUG("Driver can't access resource, SW_FW_SYNC timeout.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

	ops->write_reg(master->hw_addr, E1000_SW_FW_SYNC, swfw_sync | swmask);
	put_hw_semaphore_82575(master);

out:
	return ret_val;
}

static void release_swfw_sync_82575(struct csw_spi_master *master, u16 mask)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swfw_sync;

	CW_SPI_DEBUG("release_swfw_sync_82575\n");

	while (get_hw_semaphore_82575(master)!= NIC_SUCCESS);

	swfw_sync = ops->read_reg(master->hw_addr, E1000_SW_FW_SYNC) & ~mask;
	ops->write_reg(master->hw_addr, E1000_SW_FW_SYNC, swfw_sync);

	put_hw_semaphore_82575(master);
}

static s32 acquire_swfw_sync_82599(struct csw_spi_master *master, u16 mask)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 gssr;
	u32 swmask = mask;
	u32 fwmask = mask << 5;
	s32 ret_val = NIC_SUCCESS;
	s32 i = 0, timeout = 200;

	CW_SPI_DEBUG("acquire_swfw_sync_82599\n");

	while (i < timeout){
		if (get_hw_semaphore_82599(master)){
			ret_val = -NIC_ERR;
			goto out;
		}

		/* Check if software or firmware has the ownership */
		gssr = ops->read_reg(master->hw_addr, IXGBE_GSSR);
		if (!(gssr & (swmask | fwmask)))
			break;

		/*
		 * Firmware currently using resource (fwmask)
		 * or other software thread using resource (swmask)
		 */
		put_hw_semaphore_82599(master);
		mdelay(5);
		i++;
	}

	if (i == timeout){
		CW_SPI_DEBUG("Driver can't access resource, SW_FW_SYNC timeout.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

	ops->write_reg(master->hw_addr, IXGBE_GSSR, gssr | swmask);
	put_hw_semaphore_82599(master);

out:
	return ret_val;
}

static void release_swfw_sync_82599(struct csw_spi_master *master, u16 mask)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 gssr;

	CW_SPI_DEBUG("release_swfw_sync_82599\n");

	while (get_hw_semaphore_82599(master)!= NIC_SUCCESS);

	gssr = ops->read_reg(master->hw_addr, IXGBE_GSSR) & ~mask;
	ops->write_reg(master->hw_addr, IXGBE_GSSR, gssr);

	put_hw_semaphore_82599(master);
}

static s32 acquire_swfw_sync_i210(struct csw_spi_master *master, u16 mask)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swfw_sync;
	u32 swmask = mask;
	u32 fwmask = mask << 16;
	s32 ret_val = NIC_SUCCESS;
	s32 i = 0, timeout = 200;

	CW_SPI_DEBUG("acquire_swfw_sync_i210\n");

	while (i < timeout){
		if (get_hw_semaphore_i210(master)){
			ret_val = -NIC_ERR;
			goto out;
		}

		swfw_sync= ops->read_reg(master->hw_addr, E1000_SW_FW_SYNC);
		if (!(swfw_sync & (fwmask | swmask)))
			break;

		/*
		 * Firmware currently using resource (fwmask)
		 */
		put_hw_semaphore_i210(master);
		mdelay(5);
		i++;
	}

	if (i == timeout){
		CW_SPI_DEBUG("Driver can't access resource, SW_FW_SYNC timeout.\n");
		ret_val = -NIC_ERR;
		goto out;
	}

	ops->write_reg(master->hw_addr, E1000_SW_FW_SYNC, swfw_sync | swmask);
	put_hw_semaphore_i210(master);

out:
	return ret_val;
}

static void release_swfw_sync_i210(struct csw_spi_master *master, u16 mask)
{
	struct csw_spi_operations *ops = &master->ops;
	u32 swfw_sync;

	CW_SPI_DEBUG("release_swfw_sync_i210\n");

	while (get_hw_semaphore_i210(master)!= NIC_SUCCESS);

	swfw_sync = ops->read_reg(master->hw_addr, E1000_SW_FW_SYNC) & ~mask;
	ops->write_reg(master->hw_addr, E1000_SW_FW_SYNC, swfw_sync);

	put_hw_semaphore_i210(master);
}

static s32 acquire_swfw_sync_x540(struct csw_spi_master *master, u16 mask)
{
    struct csw_spi_operations *ops = &master->ops;
    u32 swfw_sync;
    u32 swmask = mask;
    u32 fwmask = mask << 5;
    u32 hwmask = 0;
    u32 timeout = 200;
    u32 i;
    s32 ret_val = 0;

    CW_SPI_DEBUG("acquire_swfw_sync_x540\n");

    if (swmask == IXGBE_GSSR_EEP_SM)
        hwmask = IXGBE_GSSR_FLASH_SM;

    /* SW only mask doesn't have FW bit pair */
    if (swmask == IXGBE_GSSR_SW_MNG_SM)
        fwmask = 0;

    for (i = 0; i < timeout; i++) {
        /*
         * SW NVM semaphore bit is used for access to all
         * SW_FW_SYNC bits (not just NVM)
         */
        if (get_hw_semaphore_x540(master)) {
            ret_val = IXGBE_ERR_SWFW_SYNC;
            goto out;
        }

        swfw_sync = ops->read_reg(master->hw_addr, IXGBE_GSSR);
        if (!(swfw_sync & (fwmask | swmask | hwmask))) {
            swfw_sync |= swmask;
            ops->write_reg(master->hw_addr, IXGBE_GSSR, swfw_sync);
            put_hw_semaphore_x540(master);
            msleep(5);
            goto out;
        } else {
            /*
             * Firmware currently using resource (fwmask), hardware currently
             * using resource (hwmask), or other software thread currently
             * using resource (swmask)
             */
            put_hw_semaphore_x540(master);
            msleep(5);
        }
    }

    /* Failed to get SW only semaphore */
    if (swmask == IXGBE_GSSR_SW_MNG_SM) {
        ret_val = IXGBE_ERR_SWFW_SYNC;
        goto out;
    }

    /* If the resource is not released by the FW/HW the SW can assume that
     * the FW/HW malfunctions. In that case the SW should sets the SW bit(s)
     * of the requested resource(s) while ignoring the corresponding FW/HW
     * bits in the SW_FW_SYNC register.
     */
    swfw_sync = ops->read_reg(master->hw_addr, IXGBE_GSSR);
    if (swfw_sync & (fwmask| hwmask)) {
        if (get_hw_semaphore_x540(master)) {
            ret_val = IXGBE_ERR_SWFW_SYNC;
            goto out;
        }
        swfw_sync |= swmask;
        ops->write_reg(master->hw_addr, IXGBE_GSSR, swfw_sync);
        put_hw_semaphore_x540(master);
        msleep(5);
    }

out:
    return ret_val;
}

static void release_swfw_sync_x540(struct csw_spi_master *master, u16 mask)
{
    struct csw_spi_operations *ops = &master->ops;
    u32 swfw_sync;
    u32 swmask = mask;

    CW_SPI_DEBUG("release_swfw_sync_x540\n");

    get_hw_semaphore_x540(master);

    swfw_sync = ops->read_reg(master->hw_addr, IXGBE_GSSR);
    swfw_sync &= ~swmask;
    ops->write_reg(master->hw_addr, IXGBE_GSSR, swfw_sync);

    put_hw_semaphore_x540(master);
    msleep(5);
}

/* SPI access acquire */
static s32 csw_spi_acquire_82583(struct csw_spi_master *master)
{
	s32 ret_val;

	CW_SPI_DEBUG("csw_spi_acquire_82583\n");

	ret_val = get_sw_ownership_82583(master);
	if (ret_val)
		goto out;

	ret_val = csw_spi_acquire_intel(master);
	if (ret_val)
		put_sw_ownership_82583(master);

out:
	return ret_val;
}

static void csw_spi_release_82583(struct csw_spi_master *master)
{
	CW_SPI_DEBUG("csw_spi_release_82583\n");

	csw_spi_release_intel(master);
	put_sw_ownership_82583(master);
}

static s32 csw_spi_acquire_82575(struct csw_spi_master *master)
{
	s32 ret_val;

	CW_SPI_DEBUG("csw_spi_acquire_82575\n");

	ret_val = acquire_swfw_sync_82575(master, master->ifspi.spi_swfw_sm);
	if (ret_val)
		goto out;

	ret_val = csw_spi_acquire_intel(master);
	if (ret_val)
		release_swfw_sync_82575(master, master->ifspi.spi_swfw_sm);

out:
	return ret_val;
}

static void csw_spi_release_82575(struct csw_spi_master *master)
{
	CW_SPI_DEBUG("csw_spi_release_82575\n");

	csw_spi_release_intel(master);
	release_swfw_sync_82575(master, master->ifspi.spi_swfw_sm);
}

static s32 csw_spi_acquire_82599(struct csw_spi_master *master)
{
	s32 ret_val;

	CW_SPI_DEBUG("csw_spi_acquire_82599\n");

	ret_val = acquire_swfw_sync_82599(master, master->ifspi.spi_swfw_sm);
	if (ret_val)
		goto out;

	ret_val = csw_spi_acquire_intel(master);
	if (ret_val)
		release_swfw_sync_82599(master, master->ifspi.spi_swfw_sm);

out:
	return ret_val;
}

static void csw_spi_release_82599(struct csw_spi_master *master)
{
	CW_SPI_DEBUG("csw_spi_release_82599\n");

	csw_spi_release_intel(master);
	release_swfw_sync_82599(master, master->ifspi.spi_swfw_sm);
}

static s32 csw_nvm_acquire_i210(struct csw_spi_master *master)
{
	s32 ret_val;

	CW_SPI_DEBUG("csw_nvm_acquire_i210\n");

	ret_val = acquire_swfw_sync_i210(master, master->ifspi.spi_swfw_sm);
	if (ret_val)
		goto out;

	ret_val = csw_spi_acquire_intel(master);
	if (ret_val)
		release_swfw_sync_i210(master, master->ifspi.spi_swfw_sm);

out:
	return ret_val;
}

static void csw_nvm_release_i210(struct csw_spi_master *master)
{
	CW_SPI_DEBUG("csw_nvm_release_i210\n");

	csw_spi_release_intel(master);
	release_swfw_sync_i210(master, master->ifspi.spi_swfw_sm);
}

static s32 csw_spi_acquire_x540(struct csw_spi_master *master)
{
    s32 ret_val;

    CW_SPI_DEBUG("csw_spi_acquire_x540\n");

    ret_val = acquire_swfw_sync_x540(master, master->ifspi.spi_swfw_sm);
    if (ret_val)
        goto out;

    ret_val = csw_spi_acquire_intel(master);
    if (ret_val)
        release_swfw_sync_x540(master, master->ifspi.spi_swfw_sm);

out:
    return ret_val;
}

static void csw_spi_release_x540(struct csw_spi_master *master)
{
    CW_SPI_DEBUG("csw_spi_release_x540\n");

    csw_spi_release_intel(master);
    release_swfw_sync_x540(master, master->ifspi.spi_swfw_sm);
}

static s32 csw_spi_acquire_generic(struct csw_spi_master *master)
{
	s32 ret_val = 0;
	s32 timeout = HW_SM_ATTEMPTS;

	CW_SPI_DEBUG("csw_spi_acquire_generic\n");
	// S/W semaphore
	while (timeout){
		if (!mutex_trylock(master->sem))
			break;
		udelay(50);
		timeout--;
	}

	if (!timeout){
		CW_SPI_DEBUG("Driver can't access device - SM is locked.\n");
		ret_val = -NIC_ERR;
		goto out;
	}


out:
	return ret_val;
}

static void csw_spi_release_generic(struct csw_spi_master *master)
{
	CW_SPI_DEBUG("csw_spi_rlease_generic\n");
	// S/W semaphore
	mutex_unlock(master->sem);
}

/* ----------------------------------------------
 * the SPI master control.
 * ----------------------------------------------
 */
/**
 *  csw_spi_raise_clk - Raise SPI clock
 *  @master: pointer to the BP structure
 *
 *  Enable/Raise the SPI clock bit.
 **/
static void csw_spi_raise_clk(struct csw_spi_master *master, u32 *spi)
{
	struct csw_spi_operations *ops = &master->ops;
	struct csw_spi_pin *spin = &master->ifspi.spi_sck;

	*spi = *spi | spin->mask;
	ops->write_reg(master->hw_addr, spin->reg, *spi);
	ops->wflush(master->hw_addr);
	CW_SPICLK_DEBUG("csw_spi_raise_clk:%#08x\n",
	                 ops->read_reg(master->hw_addr, spin->reg));
	udelay(master->cfg.clk_usec);
}

/**
 *  csw_spi_lower_clk - Lower SPI clock
 *  @master: pointer to the BP structure
 *
 *  Clear/Lower the SPI clock bit.
 **/
static void csw_spi_lower_clk(struct csw_spi_master *master, u32 *spi)
{
	struct csw_spi_operations *ops = &master->ops;
	struct csw_spi_pin *spin = &master->ifspi.spi_sck;

	*spi = *spi & ~spin->mask;
	ops->write_reg(master->hw_addr, spin->reg, *spi);
	ops->wflush(master->hw_addr);
	CW_SPICLK_DEBUG("csw_spi_lower_clk:%#08x\n",
	                 ops->read_reg(master->hw_addr, spin->reg));
	udelay(master->cfg.clk_usec);
}

/**
 *  csw_spi_shift_out_bits - Shift bits data out to the uP
 *  @master: pointer to the BP structure
 *  @data: data to send to the uP
 *  @count: bits to send to the uP
 **/
void csw_spi_shift_out_bits(struct csw_spi_master *master, u16 data, u16 count)
{
	struct csw_spi_operations *ops = &master->ops;
	struct csw_spi_ctrl *ifspi = &master->ifspi;
	u32 spi;
	u32 mask;
	u8 i = 0;

	CW_SPI_DEBUG("csw_spi_shift_out_bits\n");

	mask = 0x1u << (count - 1);

	do {
		spi = ops->read_reg(master->hw_addr, ifspi->spi_mosi.reg);
		spi &= ~ifspi->spi_mosi.mask;

		if (data & mask)
			spi |= ifspi->spi_mosi.mask;

		ops->write_reg(master->hw_addr, ifspi->spi_mosi.reg, spi);
		ops->wflush(master->hw_addr);

		udelay(master->cfg.clk_usec);

		csw_spi_raise_clk(master, &spi);
		csw_spi_lower_clk(master, &spi);

		mask >>= 1;

		if (((++i)%8) == 0){
			i = 0;
			udelay(master->cfg.byte_delay);
		}

	} while (mask);

	// Clean SI
	spi = ops->read_reg(master->hw_addr, ifspi->spi_mosi.reg);
	spi &= ~ifspi->spi_mosi.mask;
	ops->write_reg(master->hw_addr, ifspi->spi_mosi.reg, spi);
}

/**
 *  csw_spi_shift_in_bits - Shift bits data in from the uP
 *  @master: pointer to the BP structure
 *
 *  In order to read data from the uP, we need to shift byte in from the uP.
 *  Bits are "shifted in" by raising the clock input to the uP (setting the
 *  SCLK bit), and then reading the value of the data out "MISO" bit. During
 *  this "shifting in" process the data in "SI" bit should always be clear. (by
 *  SPEC, it seems no effect.)
 **/
u16 csw_spi_shift_in_bits(struct csw_spi_master *master, u16 count)
{
	struct csw_spi_operations *ops = &master->ops;
	struct csw_spi_ctrl *ifspi = &master->ifspi;
	u32 spi;
	u32 i;
	u16 data;

	CW_SPI_DEBUG("csw_spi_shift_in_bits\n");

	spi = ops->read_reg(master->hw_addr, ifspi->spi_miso.reg);
	spi &= ~ifspi->spi_miso.mask;
	ops->write_reg(master->hw_addr, ifspi->spi_miso.reg, spi);
	spi = ops->read_reg(master->hw_addr, ifspi->spi_mosi.reg);
	spi &= ~ifspi->spi_mosi.mask;
	ops->write_reg(master->hw_addr, ifspi->spi_mosi.reg, spi);
	data = 0;

	for (i = 0; i < count; i++) {
		data <<= 1;
		csw_spi_raise_clk(master, &spi);

		spi = ops->read_reg(master->hw_addr, ifspi->spi_miso.reg);
		if (spi & ifspi->spi_miso.mask)
			data |= 1;

		/* Clear SI */
		spi = ops->read_reg(master->hw_addr, ifspi->spi_mosi.reg);
		spi &= ~ifspi->spi_mosi.mask;
		ops->write_reg(master->hw_addr, ifspi->spi_mosi.reg, spi);

		csw_spi_lower_clk(master, &spi);

		if ((i%8) == 7)
			udelay(master->cfg.byte_delay);
	}


	return data;
}

/**
 *  csw_spi_send_inst - send uP instructions to the uP
 *  @master: pointer to the BP structure
 *  @opcode: the first command (for R/W)
 *  @cmd: the uP command
 *
 **/
static s32 csw_spi_send_inst(struct csw_spi_master *master, u8 opcode, u16 cmd)
{
	int i, cmd_len = BP_OPCODE_LEN - 1;
	u16 inst_mask = 0xFF00;
	u8 instruction = 0;
	s32 ret_val = NIC_SUCCESS;

	// Send opcode
	csw_spi_shift_out_bits(master, opcode, 8);
	// Send command
	if (opcode == BP_OPCODE_READ)
		cmd_len--;

	for (i = 0; i < cmd_len; i++) {
		instruction = ( cmd & inst_mask ) >> 8;
		csw_spi_shift_out_bits(master, instruction, 8);
		cmd <<= 8;
	}

	return ret_val;
}

/**
 *  csw_spi_acquire_intel - Intel Generic request for access to SPI
 *  @master: pointer to the BP structure
 *
 *  Set the sPI access request bit and wait for SPI access grant bit.
 *  Return successful if access grant bit set, else clear the request for
 *  SPI access and return -NIC_ERR.
 **/
s32 csw_spi_acquire_intel(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	struct csw_spi_ctrl *ifspi = &master->ifspi;
	u32 spi;
	s32 timeout = NIC_SPI_GRANT_ATTEMPTS;
	s32 ret_val = NIC_SUCCESS;

	CW_SPI_DEBUG("csw_spi_acquire_intel\n");

	spi = ops->read_reg(master->hw_addr, ifspi->spi_rqst.reg);
	spi |= ifspi->spi_rqst.mask;
	ops->write_reg(master->hw_addr, ifspi->spi_rqst.reg, spi);
	ops->wflush(master->hw_addr);

	spi = ops->read_reg(master->hw_addr, ifspi->spi_gnt.reg);
	while (timeout) {
		if (spi & ifspi->spi_gnt.mask)
			break;
		udelay(5);
		spi = ops->read_reg(master->hw_addr, ifspi->spi_gnt.reg);
		timeout--;
	}

	if (!timeout) {
		spi = ops->read_reg(master->hw_addr, ifspi->spi_rqst.reg);
		spi &= ~ifspi->spi_rqst.mask;
		ops->write_reg(master->hw_addr, ifspi->spi_rqst.reg, spi);
		CW_SPI_DEBUG("Could not acquire SPI grant\n");
		ret_val = -NIC_ERR;
	} else {
		spi = ops->read_reg(master->hw_addr, ifspi->spi_ss.reg);
		spi |= ifspi->spi_ss.mask;
		ops->write_reg(master->hw_addr, ifspi->spi_ss.reg, spi);
	}

	return ret_val;
}

static s32 csw_spi_request_cpld(struct csw_spi_master *master)
{
	struct csw_spi_cpld *cpld = &master->cpld;
	u16 spi_clock = master->cfg.clk_usec;

	/* Change clock cycle to CPLD */
	master->cfg.clk_usec = cpld->clock;
	/* Send segment code*/
	csw_spi_shift_out_bits(master, cpld->segcode, 2);
	/* Get busy bit */
	cpld->busy = csw_spi_shift_in_bits(master, 1);
	/* Set back MCU clock cycle */
	master->cfg.clk_usec = spi_clock;

	return (cpld->busy ? -EBUSY : 0);
}

/**
 *  csw_spi_stop - Terminate SPI command
 *  @master: pointer to the BP structure
 *
 *  Terminates the current command by inverting the SPI's chip select pin.
 **/
static void csw_spi_stop(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	struct csw_spi_ctrl *ifspi = &master->ifspi;
	u32 spi;

	CW_SPI_DEBUG("csw_spi_stop\n");

	spi = ops->read_reg(master->hw_addr, ifspi->spi_ss.reg);
	/* Pull CE high */
	spi |= ifspi->spi_ss.mask;
	ops->write_reg(master->hw_addr, ifspi->spi_ss.reg, spi);
	ops->wflush(master->hw_addr);
	/* Set back NVM SPI mode */
	if (master->cfg.sck)
		csw_spi_raise_clk(master, &spi);
	else
		csw_spi_lower_clk(master, &spi);
	CW_SPI_DEBUG("SPI stop: %x\n",ops->read_reg(master->hw_addr, ifspi->spi_ss.reg));
}

/**
 *  csw_spi_ready - Prepares SPI for read/write
 *  @master: pointer to the BP structure
 *
 *  Issue CPLD to switch to bypass channel.
 *  Then setups the SPI to specified MCU for reading and writing.
 **/
s32 csw_spi_ready(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	struct csw_spi_ctrl *ifspi = &master->ifspi;
	struct csw_spi_cpld *cpld = &master->cpld;
	u32 spi;
	u32 timeout = master->cpld.attempts;
	s32 ret_val = NIC_SUCCESS;

	CW_SPI_DEBUG("csw_spi_ready_microcon\n");

	spi = ops->read_reg(master->hw_addr, ifspi->spi_sck.reg);
	/* Record NVM SPI mode */
	if (spi & ifspi->spi_sck.mask)
		master->cfg.sck = 1;
	else
		master->cfg.sck = 0;

	while (timeout){
		/* New protocol with additional identification code 0x0A*/
		if (cpld->protocol == 1) {
			spi = ops->read_reg(master->hw_addr, ifspi->spi_ss.reg);
			spi |= ifspi->spi_ss.mask;
			ops->write_reg(master->hw_addr, ifspi->spi_ss.reg, spi);
			ops->wflush(master->hw_addr);
			csw_spi_shift_out_bits(master, 0x0A, 5);
		}

		/* Give a rising edge to CPLD */
		spi = ops->read_reg(master->hw_addr, ifspi->spi_mosi.reg);
		if (spi & ifspi->spi_mosi.mask){
			spi &= ~ifspi->spi_mosi.mask;
			ops->write_reg(master->hw_addr, ifspi->spi_mosi.reg, spi);
			ops->wflush(master->hw_addr);
		}
		spi |= ifspi->spi_mosi.mask;
		ops->write_reg(master->hw_addr, ifspi->spi_mosi.reg, spi);
		ops->wflush(master->hw_addr);

		/* Pull SS low previous to pull clock to make sure switch to bypass
		 * device which is SPI Mode 0 */
		spi = ops->read_reg(master->hw_addr, ifspi->spi_ss.reg);
		spi &= ~ifspi->spi_ss.mask;
		ops->write_reg(master->hw_addr, ifspi->spi_ss.reg, spi);
		ops->wflush(master->hw_addr);
		/* Clear SI and SCK*/
		spi = ops->read_reg(master->hw_addr, ifspi->spi_mosi.reg);
		spi &= ~ifspi->spi_mosi.mask;
		ops->write_reg(master->hw_addr, ifspi->spi_mosi.reg, spi);
		spi = ops->read_reg(master->hw_addr, ifspi->spi_sck.reg);
		spi &= ~ifspi->spi_sck.mask;
		ops->write_reg(master->hw_addr, ifspi->spi_sck.reg, spi);
		ops->wflush(master->hw_addr);

		ret_val = csw_spi_request_cpld(master);

		if (!ret_val){
			break;
		}

		csw_spi_stop(master);
		udelay(1);

		timeout--;
	}

	udelay(1);

	if (!timeout)
		CW_SPI_DEBUG("MCU is busy!!\n");
	return ret_val;
}

/**
 *  csw_spi_release_intel - Intel Release exclusive access to SPI
 *  @master: pointer to the BP structure
 *
 *  Stop any current commands to the uP and clear the SPI request bit.
 **/
void csw_spi_release_intel(struct csw_spi_master *master)
{
	struct csw_spi_operations *ops = &master->ops;
	struct csw_spi_ctrl *ifspi = &master->ifspi;
	u32 spi;

	CW_SPI_DEBUG("csw_spi_release_intel\n");

	spi = ops->read_reg(master->hw_addr, ifspi->spi_rqst.reg);
	spi &= ~ifspi->spi_rqst.mask;
	ops->write_reg(master->hw_addr, ifspi->spi_rqst.reg, spi);
	ops->wflush(master->hw_addr);
}

/**
 *  csw_spi_set_master - Set SPI master
 *  @master: pointer to the csw master structure
 *  @csw: pointer to the csw device
 *
 *  Set SPI master configuration based on csw device.
 **/
static int csw_spi_set_master(struct csw_spi_master *master,
                               struct csw_device *csw)
{
	int ret = 0;
	struct csw_spi_ctrl *ifspi = &master->ifspi;
	struct csw_spi_operations *ops = &master->ops;
	u16 iftype = 0xFFFF;

	/* Initial settings */
	master->cpld = spi_cpld;
	master->cfg  = spi_cfg;
	master->sem  = csw->csw_mutex;

	if (csw->hwtype == csw_hw_i210 || csw->hwtype == csw_hw_x540 || csw->hwtype == csw_hw_i354 || csw->hwtype == csw_hw_dh89xxcc) {
		master->cfg.clk_usec = 18;
	}

	if (csw->hwtype < csw_hw_onlan){
		master->hw_addr = csw->hw_addr;
		if (csw->hwtype == csw_hw_dh89xxcc)
			master->cpld.segcode = (u8)((csw->addr & 0xF)-1)/2;
		else
			master->cpld.segcode = (u8)(csw->addr & 0xF)/2;
		if (csw->hwtype == csw_hw_i354 || csw->hwtype == csw_hw_dh89xxcc) {
			master->cpld.protocol = 1;
		}
		ops->read_reg  = pci_reg_readl;
		ops->write_reg = pci_reg_writel;
		ops->wflush    = intel_flush;
		iftype = (csw->cwid & 0x000F) >> 1;
	} else if (csw->hwtype == csw_hw_rtl8168){
		master->hw_addr = csw->hw_addr;
		master->cpld.segcode = (u8)(csw->cwid & 0x1);
		ops->read_reg  = pci_reg_readb;
		ops->write_reg = pci_reg_writeb;
		ops->wflush    = amd_flush;
		iftype = (csw->cwid & 0x000F) >> 1;
	}
#ifdef _TILERA_DEFINES_H_
	else if ((csw->hwtype & 0xFFF0) == csw_hw_tilegx){
		master->cpld.segcode = (u8)(csw->addr & 0xF);
		ops->read_reg  = tilegx_gpio_read;
		ops->write_reg = tilegx_gpio_write;
		ops->wflush    = tilegx_gpio_flush;
		iftype = csw_gx36_gpio;
	}
#endif

	switch (iftype){
	case csw_intel_fla:
		if (csw->hwtype < csw_hw_i82599)
			*ifspi = spi_ctrl_fla_e1000;
		else
			*ifspi = spi_ctrl_fla_ixgbe;
		break;
	case csw_intel_eecd:
		if (csw->hwtype < csw_hw_i82599)
			*ifspi = spi_ctrl_eecd_e1000;
		else
			*ifspi = spi_ctrl_eecd_ixgbe;
		break;
	case csw_amd_a50m:
		*ifspi = spi_ctrl_gpio_a50m;
		break;
#ifdef _TILERA_DEFINES_H_
	case csw_gx36_gpio:
		*ifspi = spi_ctrl_gpio_gx[(csw->hwtype & 0xF)];
		break;
#endif
	default:
		ret = -ENODEV;
		goto out;
		break;
	}

	switch(csw->hwtype){
	case csw_hw_i82580:
	case csw_hw_i354:
	case csw_hw_dh89xxcc:
		ops->acquire = csw_spi_acquire_82575;
		ops->release = csw_spi_release_82575;
		break;
	case csw_hw_i82583:
		ops->acquire = csw_spi_acquire_82583;
		ops->release = csw_spi_release_82583;
		break;
	case csw_hw_i82599:
		ops->acquire = csw_spi_acquire_82599;
		ops->release = csw_spi_release_82599;
		break;
	case csw_hw_rtl8168:
	case csw_hw_tilegx:
	case (csw_hw_tilegx + 1):
		ops->acquire = csw_spi_acquire_generic; 
		ops->release = csw_spi_release_generic;
		break;
	case csw_hw_i210:
		ops->acquire = csw_nvm_acquire_i210;
		ops->release = csw_nvm_release_i210;
		ifspi->spi_swfw_sm = E1000_SWFW_EEC_SM;
		break;
	case csw_hw_x540:
		ops->acquire = csw_spi_acquire_x540;
		ops->release = csw_spi_release_x540;
		break;
	default:
		ret = -ENODEV;
		break;
	}

out:
	return ret;
}

/**
 *  csw_spi_read_microcon - Read from uP using SPI
 *  @master: pointer to the BP structure
 *  @cmd: instructions to the uP
 *  @data: byte read from the uP
 *
 *  Reads a 8 bit byte from the uP using SPI interface.
 **/
s32 csw_spi_read_microcon(struct csw_device *csw, u16 cmd, u8 *data)
{
	s32 ret_val = NIC_SUCCESS;
	struct csw_spi_master master;

	ret_val = csw_spi_set_master(&master, csw);
	if(ret_val)
		goto out;

	CW_SPI_DEBUG("csw_spi_read_microcon\n");

	// Request to access
	ret_val = master.ops.acquire(&master);
	if(ret_val)
		goto out;
	ret_val = csw_spi_ready(&master);
	if(ret_val)
		goto release;

	// Send instructions
	csw_spi_send_inst(&master, BP_OPCODE_READ, cmd);
	// Latch slave output
	*data = (u8) csw_spi_shift_in_bits(&master, 8);
	csw_spi_stop(&master);

release:
	master.ops.release(&master);
	udelay(READ_DELAY);

out:
	return ret_val;

}

/**
 *  csw_spi_write_microcon - Write to the uP using SPI
 *  @master: pointer to the BP structure
 *  @cmd: instructions to the uP
 *
 *  Writes data to uP at offset using SPI interface.
 **/
s32 csw_spi_write_microcon(struct csw_device *csw, u16 cmd, u8 data)
{
	s32 ret_val;
	struct csw_spi_master master;

	ret_val = csw_spi_set_master(&master, csw);
	if(ret_val)
		goto out;

	CW_SPI_DEBUG("csw_spi_write_microcon\n");

	// Request to access
	ret_val = master.ops.acquire(&master);
	if(ret_val)
		goto out;
	ret_val = csw_spi_ready(&master);
	if(ret_val)
		goto release;

	// Send instructions
	csw_spi_send_inst(&master, BP_OPCODE_WRITE, cmd | data);
	csw_spi_stop(&master);

release:
	master.ops.release(&master);
	udelay(WRITE_DELAY);

out:
	return ret_val;
}


