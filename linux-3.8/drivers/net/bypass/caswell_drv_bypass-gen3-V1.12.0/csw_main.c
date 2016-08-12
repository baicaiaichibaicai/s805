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
#include <linux/reboot.h>
#include "csw.h"
#include "bypass.h"
#include "gen3bp.h"
#include "csw_gxpci.h"
#ifdef S_TYPE
#include "s_type_ioctl.h"
#include <linux/miscdevice.h>

int bp_dev_num = 0;
struct bp_dev_info csw_dev_info[NET_INTERFACE_MAX];
#endif
char csw_drv_name[] = "csw";
static const char csw_drv_string[] =
                      "CASwell(R) Bypass Driver";
const char csw_drv_version[] = CW_PATCH_VER_STR;
static const char csw_copyright[] =
                      "Copyright (c) 2013 CASwell Inc.";

MODULE_AUTHOR("Zeno Lai, <zeno.lai@cas-well.com>");
MODULE_DESCRIPTION("CASwell(R) Ethernet Bypass Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

#ifdef CONFIG_FERRET 
#include <future/general.h>
#include <future/net/iface_array.h>
extern struct _zen_bypass_info zen_bypass_info;
int bp_gen3_no=0;
#endif

/*--------------------------------------------------------------------------*/
struct csw_device *csw_dev_get(struct csw_device *dev)
{
	if (dev)
		get_device(&dev->dev);
	return dev;
}

void csw_dev_put(struct csw_device *dev)
{
	if (dev)
		put_device(&dev->dev);
}

const struct csw_pci_device_id *csw_pci_match_one_device(const struct csw_pci_device_id *id,
                                                const struct pci_dev *dev)
{
	int i, ret = 1;

	if ((id->vendor == PCI_ANY_ID || id->vendor == dev->vendor) &&
		(id->device == PCI_ANY_ID || id->device == dev->device) &&
		(id->subdev == PCI_ANY_ID || id->subdev == dev->subsystem_device) &&
		!((id->dclass ^ dev->class) & id->class_mask)) {
		if (id->svid[0] == PCI_ANY_ID) {
			ret = 0;
		} else {
			for (i = 0; i < SVENDOR_ID_SIZE; i++) {
				if (id->svid[i] == dev->subsystem_vendor) {
					ret = 0;
					break;
				}
			}
		}
	}

	if (ret) {
		goto out;
	}

	return id;
out:
	return NULL;
}

const struct csw_pci_device_id *csw_pci_match_id(const struct csw_pci_device_id *ids,
                                        struct pci_dev *dev)
{
	while (ids) {
		if (ids->vendor == 0)
			break;
		if (csw_pci_match_one_device(ids, dev)) {
			return ids;
		}
			ids++;
	}

	return NULL;
}

int csw_pci_probe(const struct csw_pci_device_id *pci_tbl,
                        struct csw_device* (*csw_add_pci)(struct pci_dev*,
                                             const struct csw_pci_device_id *ent))
{
	int retval = 0;
	int dnum = 0;
	struct pci_dev *pdev = NULL;
	const struct csw_pci_device_id *pid = NULL;
	struct csw_device *csw = NULL;
#ifdef CONFIG_FERRET
	struct net_device *netdev=NULL;
	char bp_no[3];
#endif

	/* Probe PCI device which is CSW device */
	while ((pdev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, pdev))!= NULL){
		if (!(pid = csw_pci_match_id(pci_tbl, pdev)))
			continue;

		/* Initial csw device */
		csw = (*csw_add_pci)(pdev, pid);
		if (csw) {
#ifdef S_TYPE
			csw_dev_info[dnum].pdev = pdev;
#endif
			dnum++;
#ifdef CONFIG_FERRET 
			if((!strncmp(bootserial,"WZN601A",7)))
			{
				netdev=pci_get_drvdata(pdev);
				strncpy(bp_no,netdev->name +3,strlen(netdev->name)-3);
				bp_no[strlen(netdev->name)-3]='\0';
				sscanf(bp_no,"%d",&zen_bypass_info.allbp[dnum-1]);
				bp_gen3_no=dnum;
			}
#endif
		}
#ifdef S_TYPE
		bp_dev_num = dnum;
#endif
	}

	if (!dnum)
		retval = -ENODEV;

	return retval;
}

static int match_csw_dev_by_id(struct device *dev, void *data)
{
	struct csw_device *csw = to_csw_device(dev);
	struct csw_device_id *id = data;

	if (csw_match_one_device(id, csw))
		return 1;
	return 0;
}

static struct csw_device *csw_get_dev_by_id(const struct csw_device_id *id,
                                            struct csw_device *from)
{
	struct device *dev;
	struct device *dev_start = NULL;
	struct csw_device *csw = NULL;

	WARN_ON(in_interrupt());
	if (from)
		dev_start = &from->dev;
	dev = bus_find_device(&csw_bus_type, dev_start, (void *)id,
	                      match_csw_dev_by_id);
	if (dev)
		csw = to_csw_device(dev);
	if (from)
		csw_dev_put(from);

	return csw;	
}

struct csw_device *csw_get_subsys(unsigned int cwid, unsigned int class,
                       unsigned int type, struct csw_device *from)
{
	struct csw_device *csw;
	struct csw_device_id *id;

	id = kzalloc(sizeof(*id), GFP_KERNEL);
	if (!id)
		return NULL;
	id->cwid = cwid;
	id->class = class;
	id->type = type;

	csw = csw_get_dev_by_id(id, from);
	kfree(id);

	return csw;
}

struct csw_device *
csw_get_device(unsigned int cwid, unsigned int class, struct csw_device *from)
{
	return csw_get_subsys(cwid, class, CSW_ANY_ID, from);
}

struct csw_device *
csw_get_class(unsigned int class, struct csw_device *from)
{
	return csw_get_subsys(CSW_ANY_ID, class, CSW_ANY_ID, from);
}

/*--------------------------------------------------------------------------*/
/**
 * For csw bus
 **/

static const struct csw_device_id *csw_match_id(
            const struct csw_device_id *id, const struct csw_device *device)
{
	int i;

	for (i=0; id[i].type; i++){
		if (csw_match_one_device(&id[i], device))
			return &id[i];
	}
	return NULL;
}

static int csw_device_match(struct device *dev, struct device_driver *drv)
{
	struct csw_device *device = to_csw_device(dev);
	struct csw_driver *driver = to_csw_driver(drv);

	/* match on an id table if there is one */
	if (driver->id_table)
		return csw_match_id(driver->id_table, device) != NULL;

	return 0;
}

//#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24) )
#if 1
#define csw_device_uevent    NULL
#else
static int csw_device_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	struct csw_device *device = to_csw_device(dev);

	if (add_uevent_var(env, "MODALIAS=%s%s",
	           CSW_MODULE_PREFIX, device->name))
		return -ENOMEM;
	dev_dbg(dev, "uevent\n");
	return 0;
}
#endif

static int csw_device_probe(struct device *dev)
{
	struct csw_device *device = to_csw_device(dev);
	struct csw_driver *driver = to_csw_driver(dev->driver);
	const struct csw_device_id *id;
	int status = 0;

	csw_dev_get(device);

	status = -ENODEV;
	if (!device->driver && driver->probe && driver->id_table) {
		id = csw_match_id(driver->id_table, device);
		if (id) {
			status = driver->probe(device, id);
		}
		if (status >= 0) {
			device->driver = driver;
			status = 0;
		}
	}

	if (status) {
		csw_dev_put(device);
	}

	return status;
}

static int csw_device_remove(struct device *dev)
{
	struct csw_device *device = to_csw_device(dev);
	struct csw_driver *driver = device->driver;

	if (driver) {
		if (driver->remove) {
			driver->remove(device);
		}
		device->driver = NULL;
	}

	csw_dev_put(device);

	return 0;
}

static void csw_device_shutdown(struct device *dev)
{
	struct csw_driver *driver;

	if (!dev->driver)
		return;
	driver = to_csw_driver(dev->driver);
	if (driver->shutdown)
		driver->shutdown(to_csw_device(dev));
}

static int csw_device_suspend(struct device *dev, pm_message_t mesg)
{
	struct csw_driver *driver;

	if (!dev->driver)
		return 0;
	driver = to_csw_driver(dev->driver);
	if (!driver->suspend)
		return 0;
	return driver->suspend(to_csw_device(dev), mesg);
}

static int csw_device_resume(struct device *dev)
{
	struct csw_driver *driver;

	if (!dev->driver)
		return 0;
	driver = to_csw_driver(dev->driver);
	if (!driver->resume)
		return 0;
	return driver->resume(to_csw_device(dev));
}

static void csw_device_release(struct device *dev)
{
	kfree(to_csw_device(dev));
}

static ssize_t
show_name(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct csw_device *csw = to_csw_device(dev);
	return sprintf(buf, "%s\n", csw->name);
}

static ssize_t
show_modalias(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct csw_device *csw = to_csw_device(dev);
	return sprintf(buf, "%s%s\n", CSW_MODULE_PREFIX, csw->name);
}

static ssize_t
show_cwid(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct csw_device *csw = to_csw_device(dev);
	return sprintf(buf, "%0#x\n", csw->cwid);
}

static ssize_t
show_hwif(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct csw_device *csw = to_csw_device(dev);
	return sprintf(buf, "%s\n", csw->hwif);
}

static ssize_t
show_addr(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct csw_device *csw = to_csw_device(dev);
	return sprintf(buf, "%0#x\n", csw->addr);
}

static struct device_attribute csw_dev_attrs[] = {
	__ATTR(name, S_IRUGO, show_name, NULL),
	__ATTR(modalias, S_IRUGO, show_modalias, NULL),
	__ATTR(cwid, S_IRUGO, show_cwid, NULL),
	__ATTR(hwif, S_IRUGO, show_hwif, NULL),
	__ATTR(address, S_IRUGO, show_addr, NULL),
	__ATTR_NULL,
};

struct bus_type csw_bus_type = {
	.name       = "csw",
	.dev_attrs  = csw_dev_attrs,
	.match      = csw_device_match,
	.uevent     = csw_device_uevent,
	.probe      = csw_device_probe,
	.remove     = csw_device_remove,
	.shutdown   = csw_device_shutdown,
	.suspend    = csw_device_suspend,
	.resume     = csw_device_resume,
};

/*--------------------------------------------------------------------------*/
/**
 * For csw device
 **/

/**
 * csw_register_device - register CSW device with controller
 * @dev: initialized device.
 **/
int csw_register_device(struct csw_device *device)
{
	int status;

	device->dev.bus = &csw_bus_type;
	device->dev.release = &csw_device_release;
	set_dev_name(device, "%s:%09x", device->hwif, device->addr);
	status = device_register(&device->dev);

	return status;
}

void csw_unregister_device(struct csw_device *device)
{
	device_unregister(&device->dev);
}

/*--------------------------------------------------------------------------*/
/**
 * For csw driver
 **/

int __csw_register_driver(struct csw_driver *drv, struct module *owner,
                        const char *mod_name)
{
	int error;

	drv->driver.name = drv->name;
	drv->driver.bus = &csw_bus_type;
	drv->driver.owner = owner;
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23) )
	drv->driver.mod_name = mod_name;
#endif // End of ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23) )

	error = driver_register(&drv->driver);

	return error;
}

void csw_unregister_driver(struct csw_driver *drv)
{
	driver_unregister(&drv->driver);
}
#ifdef S_TYPE
struct file_operations csw_fops = {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
	.ioctl = device_ioctl,
#else
	.unlocked_ioctl = device_ioctl,
#endif
};

static struct miscdevice csw_ioctl = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &csw_fops,
};

struct cdev *bypass3_dev;
#endif

/*--------------------------------------------------------------------------*/
/**
 * reboot notifier
 **/
static int csw_reboot_notify_call(struct notifier_block *this, unsigned long event, void *unused)
{
	if (event == SYS_DOWN || event == SYS_HALT) {
		gen3bp_reboot_exit();
	}

	return NOTIFY_DONE;
}

static struct notifier_block csw_reboot_notifier = {
	.notifier_call = csw_reboot_notify_call,
};

/*--------------------------------------------------------------------------*/
/**
 * csw_init_module - Driver Registration Routine
 *
 * csw_init_module is the first routine called when the driver is
 * loaded. All it does is to create csw bus and register device
 * with the csw subsystem.
 **/
static int __init csw_init_module(void)
{
	int ret = 0;

	ret = bus_register(&csw_bus_type);
	if (ret)
		goto err_bus;

	csw_spi_dev_init();
	csw_tilecard_init();

	ret = bypass_class_init();
	if (ret)
		goto err_bclass;
	ret = gen3bp_init();
	if (ret)
		goto err_gen3bp;
	
#ifdef S_TYPE
	misc_register(&csw_ioctl);
#endif

	/* register reboot notifier */
	register_reboot_notifier(&csw_reboot_notifier);

	printk(KERN_INFO "%s - %s\n", csw_drv_string, csw_drv_version);
	printk(KERN_INFO "%s\n", csw_copyright);

	return 0;

err_gen3bp:
	bypass_class_exit();
err_bclass:
	csw_tilecard_exit();
	csw_spi_dev_exit();
	bus_unregister(&csw_bus_type);
#ifdef S_TYPE
	misc_deregister(&csw_ioctl);
#endif
err_bus:
	return ret;
}

/**
 * csw_exit_module - Driver Exit Cleanup Routine
 *
 * csw_exit_module is called just before the driver is removed
 * from memory.
 **/
static void __exit csw_exit_module(void)
{
#ifdef S_TYPE
	bypass_wdt_auto_exit();
	misc_deregister(&csw_ioctl);
#endif
	gen3bp_exit();
	bypass_class_exit();
	csw_tilecard_exit();
	csw_spi_dev_exit();

	/* unregister reboot notifier */
	unregister_reboot_notifier(&csw_reboot_notifier);

	bus_unregister(&csw_bus_type);
}

#ifdef CONFIG_FERRET_NETWORK_BYPASS
late_initcall(csw_init_module);
#else
module_init(csw_init_module);
#endif /* CONFIG_FERRET_NETWORK_BYPASS */
module_exit(csw_exit_module);

#ifdef S_TYPE
EXPORT_SYMBOL(bp_dev_num);
#endif
EXPORT_SYMBOL(csw_bus_type);
EXPORT_SYMBOL(csw_dev_put);
EXPORT_SYMBOL(csw_pci_probe);
EXPORT_SYMBOL(csw_get_subsys);
EXPORT_SYMBOL(csw_get_device);
EXPORT_SYMBOL(csw_get_class);
EXPORT_SYMBOL(csw_register_device);
EXPORT_SYMBOL(csw_unregister_device);
EXPORT_SYMBOL(__csw_register_driver);
EXPORT_SYMBOL(csw_unregister_driver);
