#include <linux/module.h>
#include <linux/phy.h>
#include <linux/proc_fs.h>
#include "m88e6176_reg.h"


static int m88e6176_phy_read_status(struct phy_device *phydev)
{
	int adv;
	int lpa;
	int lpagb = 0;
	int status;

	mv6176_phy_read(phydev, phydev->addr, MII_BMSR, &status);

	if (status < 0)
		return status;

	if ((status & BMSR_LSTATUS) == 0) 
		phydev->link = 0;
	else 
		phydev->link = 1;

	if (AUTONEG_ENABLE == phydev->autoneg) {
		if (phydev->supported & (SUPPORTED_1000baseT_Half
					| SUPPORTED_1000baseT_Full)) {
			mv6176_phy_read(phydev, phydev->addr, MII_STAT1000, &lpagb);

			if (lpagb < 0)
				return lpagb;

			mv6176_phy_read(phydev, phydev->addr, MII_CTRL1000, &adv);

			if (adv < 0)
				return adv;

			lpagb &= adv << 2;
		}

		mv6176_phy_read(phydev, phydev->addr, MII_LPA, &lpa);

		if (lpa < 0)
			return lpa;

		mv6176_phy_read(phydev, phydev->addr, MII_ADVERTISE, &adv);

		if (adv < 0)
			return adv;

		lpa &= adv;

		phydev->speed = SPEED_10;
		phydev->duplex = DUPLEX_HALF;
		phydev->pause = phydev->asym_pause = 0;

		if (lpagb & (LPA_1000FULL | LPA_1000HALF)) {
			phydev->speed = SPEED_1000;

			if (lpagb & LPA_1000FULL)
				phydev->duplex = DUPLEX_FULL;
		} else if (lpa & (LPA_100FULL | LPA_100HALF)) {
			phydev->speed = SPEED_100;
			
			if (lpa & LPA_100FULL)
				phydev->duplex = DUPLEX_FULL;
		} else
			if (lpa & LPA_10FULL)
				phydev->duplex = DUPLEX_FULL;

		if (phydev->duplex == DUPLEX_FULL){
			phydev->pause = lpa & LPA_PAUSE_CAP ? 1 : 0;
			phydev->asym_pause = lpa & LPA_PAUSE_ASYM ? 1 : 0;
		}
	} else {
		int bmcr;
		mv6176_phy_read(phydev, phydev->addr, MII_BMCR, &bmcr);
		if (bmcr < 0)
			return bmcr;

		if (bmcr & BMCR_FULLDPLX)
			phydev->duplex = DUPLEX_FULL;
		else
			phydev->duplex = DUPLEX_HALF;

		if (bmcr & BMCR_SPEED1000)
			phydev->speed = SPEED_1000;
		else if (bmcr & BMCR_SPEED100)
			phydev->speed = SPEED_100;
		else
			phydev->speed = SPEED_10;

		phydev->pause = phydev->asym_pause = 0;
	}
	return 0;
}

static int m88e6176_read_status(struct phy_device *phydev)
{
	int link = 0, speed = 0, duplex = 0;
	int i, err;
	unsigned long old_hub_link_bit;
	unsigned char hub_port_map[8];
	*((unsigned long *)hub_port_map) = 0x0001020304000000ull;

	old_hub_link_bit = phydev->drv->hub_link_bit;
	phydev->drv->hub_link_bit = 0;
	for(i=0;i<5;i++) {
		phydev->addr = hub_port_map[i];
		err = m88e6176_phy_read_status(phydev);
		if (err) break;
		if (!link && phydev->link) {
			link = 1;
			speed = phydev->speed;
			duplex = phydev->duplex;
		}
		if((old_hub_link_bit&(0x01<<i)) && !phydev->link) 
			phydev->drv->hub_link_change_bit |= (0x01<<hub_port_map[i]);  
		phydev->drv->hub_link_bit |= (phydev->link<<i);
	}
	phydev->link = link;
	phydev->duplex = duplex;
	phydev->speed = speed;
	phydev->addr = MV6176_MGMT_PORT;

	if (phydev->drv->hub_link_change_bit) 
		phydev->drv->hub_link_change_bit = 0;

	return err;
}

static int m88e6176_ethtool_sset(struct phy_device *phydev, struct ethtool_cmd *ecmd)
{
	return 0;
}

static int m88e6176_ethtool_gset(struct phy_device *phydev, struct ethtool_cmd *ecmd)
{
	return 0;
}

static int m88e6176_config_init(struct phy_device *phydev)
{
	return 0;
}

static int m88e6176_config_autoneg(struct phy_device *phydev)
{
	int port = -1;
	int oldpage;
	int val = 0;
	
	port = phydev->addr & 0xf;

	mv6176_phy_read(phydev, port, 0x0, &val);
	val |= 0x8000;
	mv6176_phy_write(phydev, port, 0x0, val);

	mv6176_phy_read(phydev, port, 0x0, &val);
	val |= 0x200;
	mv6176_phy_write(phydev, port, 0x0, val);

	mv6176_phy_read(phydev, port, 0x0, &val);
	val |= 0x800;
	mv6176_phy_write(phydev, port, 0x0, val);

	mv6176_phy_read(phydev, port, 0x0, &val);
	val &= ~(0x800);
	mv6176_phy_write(phydev, port, 0x0, val);

	mv6176_phy_read(phydev, port, 0x16, &oldpage);
	mv6176_phy_write(phydev, port, 0x16, 3);

	mv6176_phy_read(phydev, port, 0x13, &val);
	val &= ~(0x8000);
	val &= ~(0x4000);
	mv6176_phy_write(phydev, port, 0x13, val);

	mv6176_phy_read(phydev, port, 0x11, &val);
	val &= ~(0xc0);
	val &= ~(0x30);
	val &= ~(0xff00);
	mv6176_phy_write(phydev, port, 0x11, val);

	mv6176_phy_read(phydev, port, 0x10, &val);
	val &= ~(0xe0);
	val &= ~(0x8);
	val |= 0x7;
	mv6176_phy_write(phydev, port, 0x10, val);

	mv6176_phy_write(phydev, port, 0x16, oldpage);
	return 0;
}


void mv6176_phy_read(struct phy_device *phydev, ushort port, ushort reg, int *data)
{
	struct mii_bus *bus = phydev->bus;
	mv_smix_cmd_t mv_cmd;
	ushort val = 0;

	if (port < 0 || port > 0x4) 
		return ;
	if (reg > 0x1f) 
		return ;

	mv_cmd.u16 = 0;

	mv_cmd.s.busy = 1;
	mv_cmd.s.mode = 1;
	mv_cmd.s.opcode = 2;
	mv_cmd.s.devaddr = port;
	mv_cmd.s.regaddr = reg;
	bus->write(bus, SWC_GLOBAL_ADDR_2, SMI_PHY_CMD_REG, mv_cmd.u16);
	do {
		val = bus->read(bus, SWC_GLOBAL_ADDR_2, SMI_PHY_CMD_REG);
	} while (val & MV6176_SMI_BUSY_BIT);

	*data = bus->read(bus, SWC_GLOBAL_ADDR_2, SMI_PHY_DATA_REG);

	return ;
}

void mv6176_phy_write(struct phy_device *phydev, ushort port, ushort reg, ushort data)
{
	struct mii_bus *bus = phydev->bus;
	mv_smix_cmd_t mv_cmd;
	ushort val = 0;

	if (port < 0 || port > 0x4) 
		return ;
	if (reg > MV6176_MAX_SMI_ADDR) 
		return ;

	mv_cmd.u16 = 0;
	bus->write(bus, SWC_GLOBAL_ADDR_2, SMI_PHY_DATA_REG, data);
	mv_cmd.s.busy = 1;
	mv_cmd.s.mode = 1;
	mv_cmd.s.opcode = 1;
	mv_cmd.s.devaddr = port;
	mv_cmd.s.regaddr = reg;
	bus->write(bus, SWC_GLOBAL_ADDR_2, SMI_PHY_CMD_REG, mv_cmd.u16);
	do {
		val = bus->read(bus, SWC_GLOBAL_ADDR_2, SMI_PHY_CMD_REG);
	} while (val & MV6176_SMI_BUSY_BIT);

	return ;
}

static int m88e6176_probe(struct phy_device *phydev)
{
	int val = 0;
	if (phydev->addr == MV6176_MGMT_PORT) {
		val = phy_read(phydev, 0x0);
		val &= ~(0x01<<12);
		phy_write(phydev, 0x0, val);

		val = phy_read(phydev, 0x1);
		val |= 0xfe;
		phy_write(phydev, 0x1, val);
	}
	return 0;
}

void m88e6176_reg_show(void)
{
	return ;
}

static int m88e6176_ack_interrupt(struct phy_device *phydev)
{
	return 0;
}

static int m88e6176_config_intr(struct phy_device *phydev)
{
	return 0;
}

static struct phy_driver m88e6176_driver = {
		.phy_id = MARVELL_PHY_ID_88E6176,
		.phy_id_mask = 0xfffffff0,
		.name = "Marvell 88E6176",
		.features = PHY_GBIT_FEATURES,
		.flags = PHY_HAS_MAGICANEG | PHY_HAS_INTERRUPT,
		.config_init = &m88e6176_config_init,
		.probe = &m88e6176_probe,
		.config_aneg = &m88e6176_config_autoneg,
		.read_status = &m88e6176_read_status,
		.ack_interrupt = &m88e6176_ack_interrupt,
		.config_intr = &m88e6176_config_intr,
		.ethtool_sset = &m88e6176_ethtool_sset,
		.ethtool_gset = &m88e6176_ethtool_gset,
		.driver = { .owner = THIS_MODULE },
};

static int __init m88e6176_init(void)
{
	return phy_driver_register(&m88e6176_driver);
}

static void __exit m88e6176_exit(void)
{
	phy_driver_unregister(&m88e6176_driver);
}
module_init(m88e6176_init);
module_exit(m88e6176_exit);

