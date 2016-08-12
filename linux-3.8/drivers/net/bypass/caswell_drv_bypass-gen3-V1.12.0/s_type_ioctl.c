/*******************************************************************************

  CASwell(R) Bypass Linux driver
  Copyright(c) 2014 Scott Jhuang <scott.jhuang@cas-well.com>

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
#include "bypass.h"
#include "s_type_ioctl.h"

#define SUCCESS 0

extern struct bp_dev_info csw_dev_info[NET_INTERFACE_MAX];
extern struct list_head csw_spi_list;
extern int bp_dev_num;

static int get_dev_idx_bsf (int bus, int slot, int func)
{
	int addr = 0;

	addr = (bus << 12) | (slot << 4) | func;

	return addr;
}

static void str_low (char *str) 
{
	int i;

	for (i = 0; i < strlen(str); i++)
		if ((str[i] >= 65) && (str[i] <= 90))
			str[i] += 32;
}
static unsigned long str_to_hex(char *p) 
{
	unsigned long hex = 0;
	unsigned long length = strlen(p), shift  = 0;
	unsigned char dig = 0;

	str_low(p);
	length = strlen(p);

	if (length == 0)
		return 0;

	do {
		dig = p[--length];
		dig = dig<'a' ? (dig - '0') : (dig - 'a' + 0xa);
		hex |= (dig<<shift);
		shift += 4;
	}while (length);
	return hex;
}


static int get_dev_idx(int ifindex)
{
	int idx_dev = 0;
	struct net_device *dev;

#if (LINUX_VERSION_CODE >= 0x020618)
	for_each_netdev(&init_net, dev)
#elif (LINUX_VERSION_CODE >= 0x20616)
	for_each_netdev(dev)
#else
	for (dev = dev_base; dev; dev = dev->next)
#endif
	{
		if (ifindex == dev->ifindex) {

			struct ethtool_drvinfo drvinfo;
			char *buf;
			char res[10];
			int i=0;
			int bus,slot,func;
			memset(res,0,10);

			if (dev->ethtool_ops && dev->ethtool_ops->get_drvinfo) {
				memset(&drvinfo, 0, sizeof(drvinfo));
				dev->ethtool_ops->get_drvinfo(dev, &drvinfo);
			}
			buf = drvinfo.bus_info;

			while (*buf++!=':');
			for (i = 0; i < 10; i++, buf++) {
			if (*buf == ':')
					break;
				res[i] = *buf;
			}
			buf++;
			bus = str_to_hex(res);
			memset(res,0,10);

			for (i = 0; i < 10; i++, buf++) {
				if (*buf == '.')
					break;
				res[i] = *buf;
			}
			buf++;
			slot = str_to_hex(res);
			func = str_to_hex(buf);
			idx_dev = get_dev_idx_bsf(bus, slot, func);

			return idx_dev;
		}
	}

	return -1;
}

static int get_dev_ifindex(int bdf)
{
	struct device *busdev = NULL;
	struct pci_dev *pci_dev = NULL;
	struct net_device *net_dev = NULL;
	int addr = 0;

	FOR_EACH_NETDEV(net_dev)
	{
		busdev = GET_NETDEV_DEV(net_dev);
		if (!busdev)
			continue;

		if (is_pci_device(busdev)){
			pci_dev = to_pci_dev(busdev);
			addr = (pci_dev->bus->number << 12) | (PCI_SLOT(pci_dev->devfn) << 4) | PCI_FUNC(pci_dev->devfn);

			if (addr == bdf) {
				return net_dev->ifindex;
			}
		}
	}

	return -1;
}

void check_mode_change(struct bp_dev_info *bp_dev)
{
	int i = 0;
	uint8_t  mode = 0;
	uint8_t bypass = 1;
	uint8_t open = 2;

	for (i = 0; i < bp_dev_num; i++) {
		bp_dev[i].csw_dev->ops.read(bp_dev[i].csw_dev, BP_OPCODE_BP_STS, &mode);
		if ((mode == bypass) && (mode != bp_dev[i].current_mode)) {
			bp_dev[i].bypass_change = 1;
			if (!(i % 2))
				 bp_dev[i + 1].bypass_change = 1;
			else
				bp_dev[i - 1].bypass_change = 1;
		} else if ((mode == open) && (mode != bp_dev[i].current_mode)) {
			bp_dev[i].open_change = 1;
			if (!(i % 2))
				 bp_dev[i + 1].open_change = 1;
			else
				bp_dev[i - 1].open_change = 1;
		}
	}
}

void store_current_mode(struct bp_dev_info *bp_dev)
{
	int i = 0;

	for (i = 0; i < bp_dev_num; i++)
		bp_dev[i].csw_dev->ops.read(bp_dev[i].csw_dev, BP_OPCODE_BP_STS, &bp_dev[i].current_mode);
}

void wdt_auto_retrigger(unsigned long param)
{
	struct bp_dev_info *current_csw = NULL;

	current_csw = (struct bp_dev_info *)param;
	current_csw->csw_dev->ops.write(current_csw->csw_dev, BP_OPCODE_WDT_LOAD, 0);
	mod_timer(&current_csw->bp_timer, jiffies + (HZ * current_csw->wdt_reset_time) / 1000);
}

int bypass_wdt_auto_init(struct bp_dev_info *bp_dev)
{
	int ret = 0;

	init_timer(&bp_dev->bp_timer);
	bp_dev->bp_timer.function = &wdt_auto_retrigger;

	return ret;
}

void bypass_wdt_auto_exit(void)
{
	int i = 0;

	for (i = 0; i < bp_dev_num; i++) {
		if (&csw_dev_info[i].bp_timer) {
			del_timer(&csw_dev_info[i].bp_timer);
		}
	}
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
int device_ioctl(struct inode *inode, /* see include/linux/fs.h */
						struct file *file, /* ditto */
						unsigned int ioctl_num, /* number and param for ioctl */
						unsigned long ioctl_param)
#else
long device_ioctl(struct file *file, /* ditto */
						unsigned int ioctl_num, /* number and param for ioctl */
						unsigned long ioctl_param)
#endif
{
	int i = 0;
	struct bpctl_cmd bpctl_cmd;
	struct csw_device *csw_bp_dev = NULL;
	struct csw_device *csw_node, *tmp;
	int dev_idx = 0;
	int dev_number = 0;
	void __user *argp = (void __user *)ioctl_param;
	int ret=0;
	u8 data;

	memset(&bpctl_cmd,0,sizeof(bpctl_cmd));

	// Initial device information for IOCTL
	if (csw_dev_info[0].csw_dev == NULL) {
		i = 0;
		list_for_each_entry_safe(csw_node, tmp, &csw_spi_list, bus_list) {
			csw_dev_info[i].csw_dev = csw_node;
			csw_dev_info[i].bus = (csw_node->addr & 0xf000) >> 12;
			csw_dev_info[i].slot = (csw_node->addr & 0x0ff0) >> 4;
			csw_dev_info[i].func = (csw_node->addr & 0x000f);
			csw_dev_info[i].dev_number = i;
			bypass_wdt_auto_init(&csw_dev_info[i]);
			ret = get_dev_ifindex (csw_node->addr);
			if (ret > 0)
				csw_dev_info[i].ifindex = get_dev_ifindex (csw_node->addr);

			csw_dev_info[i].csw_dev->ops.read(csw_dev_info[i].csw_dev, BP_OPCODE_BP_STS, &csw_dev_info[i].current_mode);
			i++;
		}
	}

	check_mode_change(csw_dev_info);

	// Re-scan device
	if (ioctl_num == IOCTL_TX_MSG(IF_SCAN)) {
		i = 0;
		list_for_each_entry_safe(csw_node, tmp, &csw_spi_list, bus_list) {
			csw_dev_info[i].csw_dev = csw_node;
			csw_dev_info[i].bus = (csw_node->addr & 0xf000) >> 12;
			csw_dev_info[i].slot = (csw_node->addr & 0x0ff0) >> 4;
			csw_dev_info[i].func = (csw_node->addr & 0x000f);
			csw_dev_info[i].dev_number = i;
			ret = get_dev_ifindex (csw_node->addr);
			if (ret > 0)
				csw_dev_info[i].ifindex = get_dev_ifindex (csw_node->addr);

			i++;

		}
		bpctl_cmd.status = 0;
		ret = SUCCESS;
		goto bp_exit;
	}
	if (copy_from_user(&bpctl_cmd, argp, sizeof(struct bpctl_cmd))) {
		ret = -EFAULT; 
		goto bp_exit;
	}
	// Get bypass device quantity
	if (ioctl_num == IOCTL_TX_MSG(GET_DEV_NUM)) {
		bpctl_cmd.out_param[0] = bp_dev_num;
		if (copy_to_user(argp, (void *)&bpctl_cmd, sizeof(struct bpctl_cmd))) {
			ret = -EFAULT;
			goto bp_exit;
		}
		ret = SUCCESS;
		goto bp_exit;

	}

	// Device index : BDF
	if ((bpctl_cmd.in_param[5])||
		(bpctl_cmd.in_param[6])||
		(bpctl_cmd.in_param[7]))
		dev_idx = get_dev_idx_bsf(bpctl_cmd.in_param[5],
									bpctl_cmd.in_param[6],
									bpctl_cmd.in_param[7]);
	// Device index : all
	else if (bpctl_cmd.in_param[1]==0) {
		i = 0;
		list_for_each_entry_safe(csw_node, tmp, &csw_spi_list, bus_list)
		{
			if (i == bpctl_cmd.in_param[0]) {
				dev_idx = csw_node->addr;
				break;
			}
			i++;
		}
	}
	// Device index : ethXXX
	else dev_idx = get_dev_idx(bpctl_cmd.in_param[1]);

	for (i = 0; i < bp_dev_num; i++) {
		if (csw_dev_info[i].csw_dev->addr == dev_idx) {
			csw_bp_dev = csw_dev_info[i].csw_dev;
			dev_number = csw_dev_info[i].dev_number;

			bpctl_cmd.out_param[0] = csw_dev_info[i].bus;
			bpctl_cmd.out_param[1] = csw_dev_info[i].slot;
			bpctl_cmd.out_param[2] = csw_dev_info[i].func;
			bpctl_cmd.out_param[3] = csw_dev_info[i].ifindex;

			break;
		}
	}

	if (dev_number < 0 || dev_number > bp_dev_num) {
		ret= -EOPNOTSUPP;
		goto bp_exit;
	}

	if (csw_bp_dev == NULL) {
		ret = -EOPNOTSUPP;
		goto bp_exit;
	}

	switch (ioctl_num) {
	// Poweroff mode: Bypass
	case IOCTL_TX_MSG(SET_BYPASS_PWOFF):
		if (bpctl_cmd.in_param[2] == 0) {
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 0);
			msleep(RELAY_DELAY);
		} else {
			if (csw_dev_info[dev_number].dis_bypass) {
				bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 0);
				break;
			}
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 1);
			msleep(RELAY_DELAY);
		}
		break;
	case IOCTL_TX_MSG(GET_BYPASS_PWOFF):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BPE, &data);
		if (ret) 
			goto bp_exit;
		else if (data == 1)
			bpctl_cmd.status = 1;
		else if (data == 0)
			bpctl_cmd.status = 0;
		break;

	// Powerup mode: Bypass
	case IOCTL_TX_MSG(SET_BYPASS_PWUP):
		if (bpctl_cmd.in_param[2] == 0) {
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_NEXTBOOT, 0);
			msleep(RELAY_DELAY);
		} else {
			if (csw_dev_info[dev_number].dis_bypass) {
				bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_NEXTBOOT, 0);
				break;
			}
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_NEXTBOOT, 1);
			msleep(RELAY_DELAY);
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 1);
			msleep(RELAY_DELAY);
		}
		break;
	case IOCTL_TX_MSG(GET_BYPASS_PWUP):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_NEXTBOOT, &data);
		if (ret) {
			goto bp_exit;
		} else if (data == 1) {
			ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BPE, &data);
			if (ret)
				goto bp_exit;
			if (data == 0)
				bpctl_cmd.status = 0;
			else if (data == 1)
				bpctl_cmd.status = 1;
		} else if (data == 0) {
			bpctl_cmd.status = 0;
		}
		break;

	// Set WDT enable/disable
	case IOCTL_TX_MSG(SET_BYPASS_WD):
		csw_dev_info[dev_number].period_value = (bpctl_cmd.in_param[2]/1000);
		ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_PERIOD, csw_dev_info[dev_number].period_value);
		if (ret) 
			goto bp_exit;
		else if (bpctl_cmd.in_param[2] > 1000) {
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_LOAD, 0);
			if (ret)
				goto bp_exit;
			bpctl_cmd.status = (csw_dev_info[dev_number].period_value*1000);
		} else
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_DISABLE, 0);

		break;
	case IOCTL_TX_MSG(GET_BYPASS_WD):
		bpctl_cmd.status = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_WDT_RS, &data);
		if (ret) {
			ret = 0;
			bpctl_cmd.data[0] = -1;
			goto bp_exit;
		} else if (data == 1) {
			bpctl_cmd.data[0] = (csw_dev_info[dev_number].period_value*1000);
		} else if (data == 0) {
			bpctl_cmd.data[0] = 0;
		}
		bpctl_cmd.status = 0;
		break;

	// Get WDT countdown value
	case IOCTL_TX_MSG(GET_WD_EXPIRE_TIME):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_WDT_RS, &data); // [data] 0: disable, 1: WDT running
		if (ret) {
			bpctl_cmd.status = -1;
			goto bp_exit;
		} else if (data == 0) {
			csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_WDT_STS, &data);// [data] 0: time out, 1: normal
			if (!data) {
				bpctl_cmd.data[0] = -1;
				bpctl_cmd.status = 0;
				break;
			}
			bpctl_cmd.data[0] = 0;
		} else if (data == 1) {
			ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_WDT_COUNTDOWN, &data);
			if (ret) {
				bpctl_cmd.status = -1;
				goto bp_exit;
			} else {
				bpctl_cmd.data[0] = (data*1000);
			}
		}

		bpctl_cmd.status = 0;
		break;

	// Re-trigger WDT
	case IOCTL_TX_MSG(RESET_BYPASS_WD_TIMER):
		ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_LOAD, 0);
		if (ret)
			goto bp_exit;
		else {
			ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_WDT_RS, &data);
			if (ret)
				goto bp_exit;
			else if (data == 0)
				bpctl_cmd.status = 0;
			else
				bpctl_cmd.status = 1;
		}
		break;
	case IOCTL_TX_MSG(GET_WD_SET_CAPS):
		bpctl_cmd.status = -1;
		break;

	// Software bypass function enable/disable
	case IOCTL_TX_MSG(SET_STD_NIC):
		if (bpctl_cmd.in_param[2] == 0)
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_SW_ENABLE, 1);
		else
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_SW_ENABLE, 0);
		break;
	case IOCTL_TX_MSG(GET_STD_NIC):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_SW_ENABLE, &data);
		if (ret) {
			bpctl_cmd.status = -1;
			goto bp_exit;
		} else if (data == 0) {
			bpctl_cmd.status = 1;
		} else {
			bpctl_cmd.status = 0;
		}
		break;

	case IOCTL_TX_MSG(SET_TAP):
		bpctl_cmd.status = -1;
		break;

	case IOCTL_TX_MSG(GET_TAP):
		bpctl_cmd.status = -1;
		break;

	case IOCTL_TX_MSG(GET_TAP_CHANGE):
		bpctl_cmd.status = -1;
		break;

	case IOCTL_TX_MSG(SET_DIS_TAP):
		bpctl_cmd.status = -1;
		break;

	case IOCTL_TX_MSG(GET_DIS_TAP):
		bpctl_cmd.status = -1;
		break;

	case IOCTL_TX_MSG(SET_TAP_PWUP):
		bpctl_cmd.status = -1;
		break;

	case IOCTL_TX_MSG(GET_TAP_PWUP):
		bpctl_cmd.status = -1;
		break;

	// Set WDT expire mode :
	case IOCTL_TX_MSG(SET_WD_EXP_MODE):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_WDT_STS, &data);
		if (data == 1) {
			ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BPE, &data);
			if (data == 0)
				csw_dev_info[dev_number].open_change = 1;
			else if (data == 1)
				csw_dev_info[dev_number].bypass_change = 1;

			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_STS, 0);
		}

		if ((bpctl_cmd.in_param[2] == 0) && (csw_dev_info[dev_number].dis_bypass == 0)) {
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 1);
		} else if ((bpctl_cmd.in_param[2] == 0) && (csw_dev_info[dev_number].dis_bypass == 1)) {
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 0);
		} else if (bpctl_cmd.in_param[2] ==1) {
			bpctl_cmd.status = -1;
			ret = 0;
			break;
		} else if ((bpctl_cmd.in_param[2] == 2) && (csw_dev_info[dev_number].dis_open == 0)) {
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 0);
		} else if ((bpctl_cmd.in_param[2] == 2) && (csw_dev_info[dev_number].dis_open == 1)) {
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 1);
		}
		break;
	case IOCTL_TX_MSG(GET_WD_EXP_MODE):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BPE, &data);
		if (ret)
			goto bp_exit;
		else if (data == 0)
			bpctl_cmd.status = 2;
		else if (data == 1)
			bpctl_cmd.status = 0;
		break;

	// Disable Bypass Mode
	case IOCTL_TX_MSG(SET_DIS_BYPASS):
		if (bpctl_cmd.in_param[2] == 0) {
			csw_dev_info[dev_number].dis_bypass = 0;
			if (dev_number%2)
				csw_dev_info[dev_number-1].dis_bypass = 0;
			else
				csw_dev_info[dev_number+1].dis_bypass = 0;
		} else { 
			csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BP_STS, &data);
			if (data == 1) {
				ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_DISABLE, 0);
				msleep(RELAY_DELAY);
				ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BP_SET_NORM, 0);
				msleep(RELAY_DELAY);
			}
			csw_dev_info[dev_number].dis_bypass = 1;

			if (dev_number%2)
				csw_dev_info[dev_number-1].dis_bypass = 1;
			else
				csw_dev_info[dev_number+1].dis_bypass = 1;
		}
		bpctl_cmd.status = 0;
		break;
	case IOCTL_TX_MSG(GET_DIS_BYPASS):
		if (!csw_dev_info[dev_number].dis_bypass)
			bpctl_cmd.status = 0;
		else
			bpctl_cmd.status = 1;
		break;

	// Get bypass change history
	case IOCTL_TX_MSG(GET_BYPASS_CHANGE):
		csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BP_STS, &data);
		if ((data == 1) || (csw_dev_info[dev_number].bypass_change)) {
			csw_dev_info[dev_number].bypass_change = 1;
			bpctl_cmd.status = 1;
		} else {
			bpctl_cmd.status = 0;
		}
		break;

	// Switch Mode : Bypass
	case IOCTL_TX_MSG(SET_BYPASS):
		if (bpctl_cmd.in_param[2] == 0) {
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_DISABLE, 0);
			msleep(RELAY_DELAY);
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BP_SET_NORM, 0);
			msleep(RELAY_DELAY);
		} else if ((bpctl_cmd.in_param[2] == 1) && (csw_dev_info[dev_number].dis_bypass == 0)){
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_DISABLE, 0);
			msleep(RELAY_DELAY);
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 1);
			msleep(RELAY_DELAY);
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BP_SET_NON, 0);
			msleep(RELAY_DELAY);
			if (!ret)
				csw_dev_info[dev_number].bypass_change = 1;
		}

		if (ret)
			bpctl_cmd.status = -1;
		else
			bpctl_cmd.status = 0;
		break;
	case IOCTL_TX_MSG(GET_BYPASS):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BP_STS, &data);
		if (ret)
			bpctl_cmd.status = -1;
		else if (data == 1)
			bpctl_cmd.status = 1;
		else if (data == 0)
			bpctl_cmd.status = 0;
		break;

	case IOCTL_TX_MSG(GET_BYPASS_CAPS):
		bpctl_cmd.status = -1;
		ret = -EOPNOTSUPP;
		break;

	// To get target port's partner of same segment
	case IOCTL_TX_MSG(GET_BYPASS_SLAVE):
		if (dev_number%2)
			bpctl_cmd.status = 0;
		else {
			bpctl_cmd.status = 1;
			dev_number++;
			csw_bp_dev = csw_dev_info[dev_number].csw_dev;
			bpctl_cmd.out_param[4] = (csw_bp_dev->addr & 0xf000) >> 12;
			bpctl_cmd.out_param[5] = (csw_bp_dev->addr & 0x0ff0) >> 4;
			bpctl_cmd.out_param[6] = (csw_bp_dev->addr & 0x000f);
			bpctl_cmd.out_param[7] = csw_dev_info[dev_number].ifindex;
		}
		break;

	// Determine target port is bypass device or not
	case IOCTL_TX_MSG(IS_BYPASS): // Done
		if (csw_bp_dev != NULL)
			bpctl_cmd.status = 1;
		else {
			ret = -EOPNOTSUPP;
			goto bp_exit;
		}
		break;

	case IOCTL_TX_MSG(SET_TX):
		bpctl_cmd.status = -1;
		break;
	case IOCTL_TX_MSG(GET_TX):
		bpctl_cmd.status = -1;
		break;

	// Set WDT auto re-trigger timer
	case IOCTL_TX_MSG(SET_WD_AUTORESET):
		// WDT autoreset min time is 500 msec.
		if (bpctl_cmd.in_param[2] >= 500 && bpctl_cmd.in_param[2] <=255000) {
			// Determine target port index is all/ethXXX/BDF
			if (((bpctl_cmd.in_param[1] == 0) && ((csw_dev_info[dev_number].dev_number % 2) == 0)) || bpctl_cmd.in_param[1]
				|| bpctl_cmd.in_param[5] || bpctl_cmd.in_param[6] || bpctl_cmd.in_param[7]) {
				// Determine the partner of same segment which has been running a timer
				if ((csw_dev_info[dev_number].dev_number % 2) && timer_pending(&csw_dev_info[dev_number-1].bp_timer)) {
					// del partner's timer, then add a new timer later.
					del_timer_sync(&csw_dev_info[dev_number-1].bp_timer);
				} else if (((csw_dev_info[dev_number].dev_number % 2) == 0) && timer_pending(&csw_dev_info[dev_number+1].bp_timer)){
					del_timer_sync(&csw_dev_info[dev_number+1].bp_timer);
				}

				csw_dev_info[dev_number].wdt_reset_time = bpctl_cmd.in_param[2];
				csw_dev_info[dev_number].bp_timer.data = (unsigned long)&csw_dev_info[dev_number];

				csw_dev_info[dev_number].csw_dev->ops.write(csw_dev_info[dev_number].csw_dev, BP_OPCODE_WDT_LOAD, 0);
				mod_timer(&csw_dev_info[dev_number].bp_timer, jiffies + (HZ*bpctl_cmd.in_param[2])/1000);
			}
		} else if (bpctl_cmd.in_param[2] == 0) {
			del_timer_sync(&csw_dev_info[dev_number].bp_timer);
		}

		bpctl_cmd.status = 0;
		break;
	case IOCTL_TX_MSG(GET_WD_AUTORESET):
		if (timer_pending(&csw_dev_info[dev_number].bp_timer))
			bpctl_cmd.status = csw_dev_info[dev_number].wdt_reset_time;
		else
			bpctl_cmd.status = 0;
		break;

	// Switch moe : Open
	case IOCTL_TX_MSG(SET_DISC):
		if (bpctl_cmd.in_param[2] == 0) {
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_DISABLE, 0);
			msleep(RELAY_DELAY);
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BP_SET_NORM, 0);
			msleep(RELAY_DELAY);
		} else if((bpctl_cmd.in_param[2] == 1) && (csw_dev_info[dev_number].dis_open == 0)) {
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_DISABLE, 0);
			msleep(RELAY_DELAY);
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 0);
			msleep(RELAY_DELAY);
			ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BP_SET_NON, 0);
			msleep(RELAY_DELAY);
			if (!ret)
				csw_dev_info[dev_number].open_change = 1;
		}

		if (ret)
			bpctl_cmd.status = -1;
		else
			bpctl_cmd.status = 0;
		break;
	case IOCTL_TX_MSG(GET_DISC):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BP_STS, &data);
		if (ret)
			bpctl_cmd.status = -1;
		else if (data == 2)
			bpctl_cmd.status = 1;
		else if (data == 0)
			bpctl_cmd.status = 0;
		break;

	// Get open mode change history
	case IOCTL_TX_MSG(GET_DISC_CHANGE):
		csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BP_STS, &data);
		if ((data == 2) || (csw_dev_info[dev_number].open_change)) {
			csw_dev_info[dev_number].open_change = 1;
			bpctl_cmd.status = 1;
		} else {
			bpctl_cmd.status = 0;
		}
		break;

	// Disable open mode
	case IOCTL_TX_MSG(SET_DIS_DISC):
		if (bpctl_cmd.in_param[2] == 0) {
			csw_dev_info[dev_number].dis_open = 0;
			if (dev_number%2)
				csw_dev_info[dev_number-1].dis_open = 0;
			else
				csw_dev_info[dev_number+1].dis_open = 0;
		} else { 
			csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BP_STS, &data);
			if (data == 2) {
				ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_WDT_DISABLE, 0);
				msleep(RELAY_DELAY);
				ret = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BP_SET_NORM, 0);
				msleep(RELAY_DELAY);
			}
			csw_dev_info[dev_number].dis_open = 1;
			if (dev_number%2)
				csw_dev_info[dev_number-1].dis_open = 1;
			else
				csw_dev_info[dev_number+1].dis_open = 1;
		}
		bpctl_cmd.status = 0;
		break;
	case IOCTL_TX_MSG(GET_DIS_DISC):
		if (!csw_dev_info[dev_number].dis_open)
			bpctl_cmd.status = 0;
		else
			bpctl_cmd.status = 1;
		break;

	// Powerup mode: Open
	case IOCTL_TX_MSG(SET_DISC_PWUP):
		if (bpctl_cmd.in_param[2] == 0) {
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_NEXTBOOT, 0);
		} else {
			if (csw_dev_info[dev_number].dis_open) {
				bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_NEXTBOOT, 0);
				break;
			}
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_NEXTBOOT, 1);
			msleep(RELAY_DELAY);
			bpctl_cmd.status = csw_bp_dev->ops.write(csw_bp_dev, BP_OPCODE_BPE, 0);
			msleep(RELAY_DELAY);
		}
		break;
	case IOCTL_TX_MSG(GET_DISC_PWUP):
		ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_NEXTBOOT, &data);
		if (ret) {
			goto bp_exit;
		} else if (data == 1) {
			ret = csw_bp_dev->ops.read(csw_bp_dev, BP_OPCODE_BPE, &data);
			if (ret)
				goto bp_exit;
			if (data == 0)
				bpctl_cmd.status = 1;
			else if (data == 1)
				bpctl_cmd.status = 0;
		} else if (data == 0) {
			bpctl_cmd.status = 0;
		}
		break;

	case IOCTL_TX_MSG(GET_BYPASS_INFO):
		bpctl_cmd.status = -1;
		break;

	case IOCTL_TX_MSG(SET_TPL):
		bpctl_cmd.status = -1;
		break;
	case IOCTL_TX_MSG(GET_TPL):
		bpctl_cmd.status = -1;
		break;

//#ifdef PMC_FIX_FLAG
	case IOCTL_TX_MSG(SET_BP_WAIT_AT_PWUP):
		bpctl_cmd.status = -1;
		ret = -EOPNOTSUPP;
		break;
	case IOCTL_TX_MSG(GET_BP_WAIT_AT_PWUP):
		bpctl_cmd.status = -1;
		ret = -EOPNOTSUPP;
		break;

	case IOCTL_TX_MSG(SET_BP_HW_RESET):
		bpctl_cmd.status = -1;
		break;
	case IOCTL_TX_MSG(GET_BP_HW_RESET):
		bpctl_cmd.status = -1;
		break;
//#endif
#ifdef BP_SELF_TEST
	case IOCTL_TX_MSG(SET_BP_SELF_TEST):
		bpctl_cmd.status = -1;
		break;
	case IOCTL_TX_MSG(GET_BP_SELF_TEST):
		bpctl_cmd.status = -1;
		break;

#endif
#if 0
	case IOCTL_TX_MSG(SET_DISC_PORT) :
		bpctl_cmd.status= set_disc_port_fn(dev_idx, bpctl_cmd.in_param[2]);
		break;

	case IOCTL_TX_MSG(GET_DISC_PORT) :
		bpctl_cmd.status= get_disc_port_fn(dev_idx);
		break;

	case IOCTL_TX_MSG(SET_DISC_PORT_PWUP) :
		bpctl_cmd.status= set_disc_port_pwup_fn(dev_idx, bpctl_cmd.in_param[2]);
		break;

	case IOCTL_TX_MSG(GET_DISC_PORT_PWUP) :
		bpctl_cmd.status= get_disc_port_pwup_fn(dev_idx);
		break;
#endif

	default:
		ret = -EOPNOTSUPP;
		goto bp_exit;
	}
	if (copy_to_user(argp, (void *)&bpctl_cmd, sizeof(struct bpctl_cmd)))
		ret = -EFAULT;
	ret = SUCCESS;
	bp_exit:
		store_current_mode(csw_dev_info);

	return ret;
}

