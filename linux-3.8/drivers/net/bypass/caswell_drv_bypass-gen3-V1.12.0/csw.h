/*******************************************************************************

  CASwell(R) Ethernet Bypass Linux driver
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

#ifndef _CSW_H_
#define _CSW_H_

#include <linux/types.h>
#ifdef __KERNEL__
#include "kcompat.h"
#include "cwdev.h"

#define DRV_VERSION          "1.12.0"
#define DRV_DATE             "Apr 15 2015"
#define CW_PATCH_VER_STR     "V" DRV_VERSION " (" DRV_DATE ")"

#define CSW_NAME_SIZE       10
#define CSW_MODULE_PREFIX   "csw:"
#define NET_INTERFACE_MAX   40

// CSW CLASS ID [3:2]
#define CSW_CLASS_BYPASS          0x0001   // bypass

enum csw_hw_type {
	csw_hw_i82580 = 0,
	csw_hw_i82583,	
	csw_hw_i210,
	csw_hw_i354,
	csw_hw_dh89xxcc,
	csw_hw_i82599,
	csw_hw_x540,
	csw_hw_onlan,              // boundary of on LAN type
	csw_hw_gx36ep,
	csw_hw_tilegx = 0x1000,    // 0x1000 ~ 0x100F for tilegx
	csw_hw_rtl8168 = 0x1010,
};

enum csw_if_type {
	csw_intel_fla = 0,
	csw_intel_eecd,
	csw_amd_a50m,
	csw_tilera_gxpci,
	csw_gx36_gpio = 0x1000,
};

extern struct bus_type csw_bus_type;
struct csw_driver;
struct csw_device;

#define CSW_ANY_ID (~0)

struct csw_device_id {
	__u32 cwid;                // CW device ID
	__u32 class;               // Type of CSW
	__u32 type;                // sub type of CSW device
};

struct csw_driver {
	struct list_head node;
	char *name;
	const struct csw_device_id *id_table;

	int id;
	unsigned int class;

	int  (*probe)(struct csw_device *, const struct csw_device_id *);
	void (*remove)(struct csw_device *);
	void (*shutdown)(struct csw_device *);
	int  (*suspend)(struct csw_device *, pm_message_t mesg);
	int  (*resume)(struct csw_device *);

	struct device_driver driver;
};

struct csw_operations {
	s32 (*read)(struct csw_device *, u16, u8*);
	s32 (*write)(struct csw_device *, u16, u8);
};

struct csw_device {
	struct list_head bus_list;

	unsigned short        cwid;    // CW device type ID
	unsigned short        class;
	unsigned short        type;
	char                  name[CSW_NAME_SIZE];
	char                  hwif[CSW_NAME_SIZE];  // H/W device interface
	unsigned int          addr;    // device address
	unsigned short        hwtype;  // H/W type
	u8 __iomem            *hw_addr;
	struct mutex          *csw_mutex;

	struct csw_operations ops;

	struct csw_driver     *driver;
	struct device         dev;
	struct device         *p;      // a device connected to this csw device.
	int irq;
};

#define CASWELL         0x15BB
#define RVB             0x1B2E
#define SVENDOR_ID      {CASWELL, RVB}
#define SVENDOR_ID_SIZE	2
#define SVENDOR_ANY_ID	{PCI_ANY_ID}

#define CSW_SID_VPD_KW			"V0"
#define READONLY_VPD_DESCRIPTOR	0x90
#define VPD_SIZE				128

struct csw_pci_device_id {
	u32 vendor;
	u32 device;
	u32 svid[SVENDOR_ID_SIZE];
	u32 subdev;
	u32 dclass;
	u32 class_mask;
	kernel_ulong_t driver_data;
};


#define to_csw_device(d) container_of(d, struct csw_device, dev)
#define to_csw_driver(d) container_of(d, struct csw_driver, driver)

#define CSWDEV_ALIGN           32
static inline void *cswdev_priv(const struct csw_device *dev)
{
	return (char *)dev + ALIGN(sizeof(struct csw_device), CSWDEV_ALIGN);
}

static inline void *csw_get_drvdata(struct csw_device *dev)
{
	return dev_get_drvdata(&dev->dev);
}

static inline void csw_set_drvdata(struct csw_device *dev, void *data)
{
	dev_set_drvdata(&dev->dev, data);
}

static inline const struct csw_device_id *
csw_match_one_device(const struct csw_device_id *id,
                     const struct csw_device *dev)
{
	if ((id->cwid == CSW_ANY_ID || id->cwid == dev->cwid) &&
	    (id->class == CSW_ANY_ID || id->class == dev->class) &&
	    (id->type == CSW_ANY_ID || id->type == dev->type))
		return id;
	return NULL;
}

extern int csw_register_device(struct csw_device *dev);
extern void csw_unregister_device(struct csw_device *dev);
#define csw_register_driver(d)   \
	__csw_register_driver(d, THIS_MODULE, KBUILD_MODNAME)
extern int __csw_register_driver(struct csw_driver *drv, struct module *owner,
                                 const char *mod_name);
extern void csw_unregister_driver(struct csw_driver *drv);
extern int csw_pci_probe(const struct csw_pci_device_id *pci_tbl,
                struct csw_device* (*csw_add_pci)(struct pci_dev*,
                                          const struct csw_pci_device_id *ent));
const struct csw_pci_device_id *csw_pci_match_one_device(const struct csw_pci_device_id *id,
                                                    const struct pci_dev *dev);
const struct csw_pci_device_id *csw_pci_match_id(const struct csw_pci_device_id *ids,
                                         struct pci_dev *dev);
extern void csw_dev_put(struct csw_device *dev);
extern struct csw_device *csw_get_subsys(unsigned int cwid, unsigned int class,
                       unsigned int type, struct csw_device *from);
extern struct csw_device *
csw_get_device(unsigned int cwid, unsigned int class, struct csw_device *from);
extern struct csw_device *
csw_get_class(unsigned int class, struct csw_device *from);

int csw_driver_init(void);
void csw_driver_exit(void);
int csw_spi_dev_init(void);
void csw_spi_dev_exit(void);


/*--------------------------------------------------------------------------*/
/**
 * For PCI csw devices
 **/

#define PCI_CWDEVICE(vendor, device)       \
	PCI_VENDOR_ID_##vendor, (device),      \
	SVENDOR_ID, PCI_ANY_ID, 0, 0

#define PCI_CWDEVICE_TILE(vendor, device)       \
	PCI_VENDOR_ID_##vendor, (device),      \
	SVENDOR_ANY_ID, PCI_ANY_ID, 0, 0

static inline u32 pci_reg_readl(u8 __iomem* hw_addr, u32 reg)
{
	return readl(hw_addr + reg);
}

static inline void pci_reg_writel(u8 __iomem* hw_addr, u32 reg, u32 value)
{
	writel(value, (hw_addr + reg));
}

static inline u32 pci_reg_readb(u8 __iomem* hw_addr, u32 reg)
{
	return readb(hw_addr + reg);
}

static inline void pci_reg_writeb(u8 __iomem* hw_addr, u32 reg, u32 value)
{
	writeb(value, (hw_addr + reg));
}

#endif  /* __KERNEL__ */
#endif  /* _CSW_H_ */
