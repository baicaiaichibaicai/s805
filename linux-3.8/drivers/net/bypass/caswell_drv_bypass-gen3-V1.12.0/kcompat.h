/*******************************************************************************

  CASwell(R) Gen3 Bypass Linux driver
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

#ifndef _KCOMPAT_H_
#define _KCOMPAT_H_

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#else
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif
#include <linux/pci.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/init.h>
#include <linux/device.h>           // for struct device
#include <linux/mempolicy.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/sched.h>            // for completion
#include <linux/cpu.h>
#include <linux/mutex.h>

#include <linux/ctype.h>
#include <linux/capability.h>
#include <linux/fs.h>
#include <linux/kernel.h>

#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>

#ifndef readq
static inline u64 readq(void __iomem *reg)
{
    return ((u64) readl(reg)) | (((u64) readl(reg + 4UL)) << 32);
}
 
static inline void writeq(u64 val, void __iomem *reg)
{
    writel(val & 0xffffffff, reg);
    writel(val >> 32, reg + 0x4UL);
}
#endif


/******************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,9))
#define __iomem
#endif // < 2.6.9

/******************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,16) )
#define DEFINE_MUTEX(x)     DECLARE_MUTEX(x)
#undef mutex
#define mutex               semaphore
#define mutex_init(x)       init_MUTEX(x)
#define mutex_trylock(x)    down_trylock(x)
#define mutex_unlock(x)     up(x)
#endif // < 2.6.16

/******************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19) )
#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#endif
#endif // < 2.6.19

/******************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32) )
#define TYPE_PREFIX
#else
#define TYPE_PREFIX     const
#endif // < 2.6.32

/******************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36) )
#undef usleep_range
#define usleep_range(min, max)  msleep(DIV_ROUND_UP(min, 1000))
#endif // < 2.6.36

/******************************************************************************/
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26) )
#define set_dev_name(d, fmt, args...)   \
	snprintf(d->dev.bus_id, BUS_ID_SIZE, fmt, ##args)
#else
#define set_dev_name(d, fmt, args...)   \
	dev_set_name(&(d)->dev, fmt, ##args)
#endif // < 2.6.26

/******************************************************************************/
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0) )
#define __devinit
#define __devexit
#endif // >= 3.8.0

/******************************************************************************/
#if (LINUX_VERSION_CODE < 0x02061D)
#ifndef pci_read_vpd
static inline ssize_t
pci_read_vpd(struct pci_dev *dev, loff_t pos, size_t count, u8 *buf)
{
	int i, vpd_cap;

	vpd_cap = pci_find_capability(dev, PCI_CAP_ID_VPD);
	if (!vpd_cap) {
		return -ENODEV;
	}

	for (i = 0; i < count; i += 4) {
		u32 tmp, j = 0;
		__le32 v;
		u16 tmp16;

		pci_write_config_word(dev, vpd_cap + PCI_VPD_ADDR, i);

		while (j++ < 100) {
			pci_read_config_word(dev, vpd_cap +
								PCI_VPD_ADDR, &tmp16);
			if (tmp16 & 0x8000) {
				break;
			}
			msleep(1);
		}

		if (!(tmp16 & 0x8000)) {
			break;
		}

		pci_read_config_dword(dev, vpd_cap + PCI_VPD_DATA, &tmp);
		v = cpu_to_le32(tmp);
		memcpy(&buf[i], &v, sizeof(v));
	}

	return i;
}
#endif
#endif

/******************************************************************************/
#if ( LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,32) )

#ifndef PCI_VPD_LRDT
#define PCI_VPD_LRDT                    0x80
#define PCI_VPD_LRDT_TAG_SIZE           3
#define PCI_VPD_SRDT_TAG_SIZE           1
#define PCI_VPD_STIN_END                0x78    /* End */
#define PCI_VPD_SRDT_END                PCI_VPD_STIN_END
#define PCI_VPD_SRDT_LEN_MASK           0x07
#define PCI_VPD_INFO_FLD_HDR_SIZE       3

static inline u16 pci_vpd_lrdt_size(const u8 *lrdt)
{
	return (u16)lrdt[1] + ((u16)lrdt[2] << 8);
}

static inline u8 pci_vpd_srdt_size(const u8 *srdt)
{
	return (*srdt) & PCI_VPD_SRDT_LEN_MASK;
}

static inline u8 pci_vpd_info_field_size(const u8 *info_field)
{
	return info_field[2];
}

static inline int pci_vpd_find_tag(const u8 *buf, unsigned int off, unsigned int len, u8 rdt)
{
	int i;

	for (i = off; i < len; ) {
		u8 val = buf[i];

		if (val & PCI_VPD_LRDT) {
			/* Don't return success of the tag isn't complete */
			if (i + PCI_VPD_LRDT_TAG_SIZE > len)
				break;

			if (val == rdt)
				return i;

			i += PCI_VPD_LRDT_TAG_SIZE +
				pci_vpd_lrdt_size(&buf[i]);
		} else {
			u8 tag = val & ~PCI_VPD_SRDT_LEN_MASK;

			if (tag == rdt)
				return i;

			if (tag == PCI_VPD_SRDT_END)
				break;

			i += PCI_VPD_SRDT_TAG_SIZE +
				pci_vpd_srdt_size(&buf[i]);
		}
	}

	return -ENOENT;
}

static inline int pci_vpd_find_info_keyword(const u8 *buf, unsigned int off,
				unsigned int len, const char *kw)
{
	int i;

	for (i = off; i + PCI_VPD_INFO_FLD_HDR_SIZE <= off + len;) {
		if (buf[i + 0] == kw[0] && buf[i + 1] == kw[1])
			return i;

		i += PCI_VPD_INFO_FLD_HDR_SIZE +
			pci_vpd_info_field_size(&buf[i]);
	}

	return -ENOENT;
}
#endif // ifndef PCI_VPD_LRDT
#endif // <=2.6.32
#endif
