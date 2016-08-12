/*
    pca9555_bypass.c - 16-bit I/O port with interrupt and reset
    with special microcon connected. For net port bypass controller
    use. The driver is modified from pca9539.c

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/miscdevice.h>
#include <linux/hwmon-sysfs.h>
#include <linux/time.h>
#include "bypass_def.h"
#include "bypass_control.h"
#include "bypass_action.h"
#include "bypass_i2cmux.h"
#include "bypass_smbus.h"

#define __INCLUDE_PLATEFORM_DATA__
#include "bypass_platform.h"

#define BYPASS_MAX_SYSFS_PRE_SEG   11

extern  bp_interface bp_int[NET_INTERFACE_MAX];

extern struct semaphore i2cmux_sem;

static struct attribute_group bypass_sysfs_groups[PCI_ADDCARD_MAX_SLOTS];

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static struct sensor_device_attribute *sda_pool[PCI_ADDCARD_MAX_SLOTS][BYPASS_MAX_SYSFS_PRE_SEG * 2];
#else
static struct class_device_attribute *sda_pool[PCI_ADDCARD_MAX_SLOTS][BYPASS_MAX_SYSFS_PRE_SEG * 2];
#endif

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t bypass_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t bypass_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = index >> 4 & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_status(client, 0);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t bypass_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t bypass_store(struct class_device *dev, const char *buf, size_t count, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	unsigned long val = simple_strtoul(buf, NULL, 0);
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_status(client, 0, val);

	up(&i2cmux_sem);

	return (ret ? ret : count);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t nextboot_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t nextboot_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_nextboot(client, 0);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t nextboot_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t nextboot_store(struct class_device *dev, const char *buf, size_t count, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	unsigned long val = simple_strtoul(buf, NULL, 0);
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_nextboot(client, 0, val);

	up(&i2cmux_sem);
    
	return (ret ? ret : count);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t bpe_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t bpe_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_bpe(client, 0);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t bpe_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t bpe_store(struct class_device *dev, const char *buf, size_t count, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	unsigned long val = simple_strtoul(buf, NULL, 0);
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_bpe(client, 0, val);

	up(&i2cmux_sem);

	return (ret ? ret : count);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t timeout_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t timeout_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_timeout_bit(client, 0);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t wdt_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t wdt_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_wdt(client, 0);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t wdt_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t wdt_store(struct class_device *dev, const char *buf, size_t count, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	unsigned long val = simple_strtoul(buf, NULL, 0);
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_wdt(client, 0, val);

	up(&i2cmux_sem);

	return (ret ? ret : count);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t period_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t period_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_period(client, 0);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t period_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t period_store(struct class_device *dev, const char *buf, size_t count, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	unsigned long val = simple_strtoul(buf, NULL, 0);
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_period(client, 0, val);

	up(&i2cmux_sem);

	return (ret ? ret : count);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t bp_type_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
#else
static ssize_t bp_type_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
#endif
	bp_interface *bp_interface;

	if (find_slot_bp_interface(&bp_interface, slot_no))
		return -ENXIO;

	return sprintf(buf, "%s\n", bp_interface->modelname);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t auto_trigger_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t auto_trigger_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_auto_trigger (client, 0);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t auto_trigger_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t auto_trigger_store(struct class_device *dev, const char *buf, size_t count, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	unsigned long val = simple_strtoul(buf, NULL, 0);
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_auto_trigger (client, 0, val);

	up(&i2cmux_sem);

	return (ret ? ret : count);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t firmware_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
#else
static ssize_t firmware_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_fw_ver(client, 0);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);

}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t tx_disable_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = ((psa->index & 0xf) < 2) ? 0 : 1;
#else
static ssize_t tx_disable_show(struct class_device *dev, char *buf, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
	ret = bypass_get_txdisable(client, (psa->index & 0xf) % 2);
#else
	ret = bypass_get_txdisable(client, (index >> 8) & 0xf);
#endif

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
static ssize_t tx_disable_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = ((psa->index & 0xf) < 2) ? 0 : 1;
#else
static ssize_t tx_disable_store(struct class_device *dev, const char *buf, size_t count, int index)
{
	int slot_no = (index >> 4) & 0xf;
	int seg_no = index & 0xf;
#endif
	struct i2c_client *client;
	unsigned long val = simple_strtoul(buf, NULL, 0);
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
	ret = bypass_set_txdisable(client, (psa->index & 0xf) % 2, val);
#else
	ret = bypass_set_txdisable(client, (index >> 8) & 0xf, val);
#endif

	up(&i2cmux_sem);

	return (ret ? ret : count);
}

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )

#define SDA_OPS(show, store) show, store
#define SDA_ATTR &sda->dev_attr.attr

struct sensor_device_attribute *create_sda(char *name, umode_t mode, 
											ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf),
											ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count), 
											int index){
	struct sensor_device_attribute *sda;

	sda = kmalloc(sizeof(struct sensor_device_attribute), GFP_KERNEL);
	sda->dev_attr.attr.name = kmalloc(sizeof(char) * 32, GFP_KERNEL);
	sprintf((char *)sda->dev_attr.attr.name, "%s", name);
	sda->dev_attr.attr.mode = mode;
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) )
	sda->dev_atty.attr.owner = THIS_MODULE;
#endif
	sda->dev_attr.show = show;
	sda->dev_attr.store = store;
	sda->index = index;

	return sda;
}
#else
struct class_func_t {
	ssize_t (*show)(struct class_device *, char * buf);
	ssize_t (*store)(struct class_device *, const char * buf, size_t count);
};

#define CLASS_CTRL_W(name, index)\
static ssize_t name##_store_##index(struct class_device *dev, const char *buf, size_t count) \
{\
    return name##_store(dev, buf, count, 0x##index);\
}

#define CLASS_CTRL_R(name, index)\
static ssize_t name##_show_##index(struct class_device *dev, char *buf) \
{\
    return name##_show(dev, buf, 0x##index);\
}

#define CLASS_ENTRY_R(name, index)\
    CLASS_CTRL_R(name, index)
#define CLASS_ENTRY_W(name, index)\
    CLASS_CTRL_W(name, index)
#define CLASS_ENTRY_RW(name, index)\
    CLASS_CTRL_R(name, index)\
    CLASS_CTRL_W(name, index)

#define CLASS_BP_ENTRY(slot, index)\
	CLASS_ENTRY_RW(bypass, slot##index)\
	CLASS_ENTRY_RW(nextboot, slot##index)\
	CLASS_ENTRY_RW(bpe, slot##index)\
	CLASS_ENTRY_RW(wdt, slot##index)\
	CLASS_ENTRY_RW(auto_trigger, slot##index)\
	CLASS_ENTRY_RW(period, slot##index)\
	CLASS_ENTRY_R(timeout, slot##index)\
	CLASS_ENTRY_R(bp_type, slot##index)\
	CLASS_ENTRY_R(firmware, slot##index)\
	CLASS_ENTRY_RW(tx_disable, 0##slot##index)\
	CLASS_ENTRY_RW(tx_disable, 1##slot##index)

#define CLASS_BP_SLOT_ENTRY(slot)\
	CLASS_BP_ENTRY(slot, 0)\
	CLASS_BP_ENTRY(slot, 1)

CLASS_BP_SLOT_ENTRY(0);
CLASS_BP_SLOT_ENTRY(1);
CLASS_BP_SLOT_ENTRY(2);
CLASS_BP_SLOT_ENTRY(3);
CLASS_BP_SLOT_ENTRY(4);
CLASS_BP_SLOT_ENTRY(5);
CLASS_BP_SLOT_ENTRY(6);
CLASS_BP_SLOT_ENTRY(7);

#define CLASS_FUNC_ENTRY_R(name, index)\
	{ .show = name##_show_##index,\
	  .store = NULL\
	}

#define CLASS_FUNC_ENTRY_RW(name, index)\
	{ .show = name##_show_##index,\
	  .store = name##_store_##index\
	}

#define CLASS_FUNC_BP_ENTRY(slot, index)\
	CLASS_FUNC_ENTRY_RW(bypass, slot##index),\
	CLASS_FUNC_ENTRY_RW(nextboot, slot##index),\
	CLASS_FUNC_ENTRY_RW(bpe, slot##index),\
	CLASS_FUNC_ENTRY_RW(wdt, slot##index),\
	CLASS_FUNC_ENTRY_RW(auto_trigger, slot##index),\
	CLASS_FUNC_ENTRY_RW(period, slot##index),\
	CLASS_FUNC_ENTRY_R(timeout, slot##index),\
	CLASS_FUNC_ENTRY_R(bp_type, slot##index),\
	CLASS_FUNC_ENTRY_R(firmware, slot##index),\
	CLASS_FUNC_ENTRY_RW(tx_disable, 0##slot##index),\
	CLASS_FUNC_ENTRY_RW(tx_disable, 1##slot##index)

#define CLASS_FUNC_BP_SLOT_ENTRY(slot)\
	CLASS_FUNC_BP_ENTRY(slot, 0),\
	CLASS_FUNC_BP_ENTRY(slot, 1)

static struct class_func_t class_func[8][22] = {
	{ CLASS_FUNC_BP_SLOT_ENTRY(0) },
	{ CLASS_FUNC_BP_SLOT_ENTRY(1) },
	{ CLASS_FUNC_BP_SLOT_ENTRY(2) },
	{ CLASS_FUNC_BP_SLOT_ENTRY(3) },
	{ CLASS_FUNC_BP_SLOT_ENTRY(4) },
	{ CLASS_FUNC_BP_SLOT_ENTRY(5) },
	{ CLASS_FUNC_BP_SLOT_ENTRY(6) },
	{ CLASS_FUNC_BP_SLOT_ENTRY(7) },
};

#define SDA_OPS(show, store) NULL, NULL
#define SDA_ATTR &sda->attr

static int get_bypass_op_index(char *name, int index)
{
	if (!strncmp(name, "bypass", 6)) {
		return 0 + (index * 11);
	} else if (!strncmp(name, "nextboot", 8)) {
		return 1 + (index * 11);
	} else if (!strncmp(name, "bpe", 3)) {
		return 2 + (index * 11);
	} else if (!strncmp(name, "wdt", 3)) {
		return 3 + (index * 11);
	} else if (!strncmp(name, "auto_trigger", 12)) {
		return 4 + (index * 11);
	} else if (!strncmp(name, "period", 6)) {
		return 5 + (index * 11);
	} else if (!strncmp(name, "timeout", 7)) {
		return 6 + (index * 11);
	} else if (!strncmp(name, "bp_type", 7)) {
		return 7 + (index * 11);
	} else if (!strncmp(name, "firmware", 8)) {
		return 8 + (index * 11);
	} else if (!strncmp(name, "tx_disable", 10)) {
		return 9 + (index % 2) + ((index / 2) * 11);
	}

	return -1;
}

struct class_device_attribute *create_sda(char *name, umode_t mode, 
											ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf),
											size_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count),
											int index){
	struct class_device_attribute *sda;
	int slot, seg, idx;

	slot = (index >> 4) & 0xF;
	seg = index & 0xF;

	sda = kmalloc(sizeof(struct class_device_attribute), GFP_KERNEL);
	sda->attr.name = kmalloc(sizeof(char) * 32, GFP_KERNEL);
	sprintf((char *)sda->attr.name, "%s", name);
	sda->attr.mode = mode;
	sda->attr.owner = THIS_MODULE;

	idx = get_bypass_op_index(name, seg);
	if (idx < 0) {
		return NULL;
	}

	sda->show = class_func[slot][idx].show;
	sda->store = class_func[slot][idx].store;

	return sda;
}
#endif

void bypass_add_attr0(int slot_no, int idx){
	int index = 0;
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
	struct sensor_device_attribute *sda;
#else
	struct class_device_attribute *sda;
#endif

	sda = create_sda("bypass0", S_IRUGO | S_IWUSR, SDA_OPS(bypass_show, bypass_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("nextboot0", S_IRUGO | S_IWUSR, SDA_OPS(nextboot_show, nextboot_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("bpe0", S_IRUGO | S_IWUSR, SDA_OPS(bpe_show, bpe_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("wdt0", S_IRUGO | S_IWUSR, SDA_OPS(wdt_show, wdt_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("auto_trigger0", S_IRUGO | S_IWUSR, SDA_OPS(auto_trigger_show, auto_trigger_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("period0", S_IRUGO | S_IWUSR, SDA_OPS(period_show, period_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("timeout0", S_IRUGO, SDA_OPS(timeout_show, NULL), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("bp_type0", S_IRUGO, SDA_OPS(bp_type_show, NULL), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("firmware0", S_IRUGO, SDA_OPS(firmware_show, NULL), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("tx_disable0_0", S_IRUGO | S_IWUSR, SDA_OPS(tx_disable_show, tx_disable_store), slot_no << 4 | (idx * 2));
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("tx_disable0_1", S_IRUGO | S_IWUSR, SDA_OPS(tx_disable_show, tx_disable_store), slot_no << 4 | ((idx * 2) + 1));
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	bypass_sysfs_groups[slot_no].attrs[index++] = NULL;
}

void bypass_add_attr1(int slot_no, int  idx){
	int index = BYPASS_MAX_SYSFS_PRE_SEG;
#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
	struct sensor_device_attribute *sda;
#else
	struct class_device_attribute *sda;
#endif

	sda = create_sda("bypass1", S_IRUGO | S_IWUSR, SDA_OPS(bypass_show, bypass_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("nextboot1", S_IRUGO | S_IWUSR, SDA_OPS(nextboot_show, nextboot_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("bpe1", S_IRUGO | S_IWUSR, SDA_OPS(bpe_show, bpe_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("wdt1", S_IRUGO | S_IWUSR, SDA_OPS(wdt_show, wdt_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("auto_trigger1", S_IRUGO | S_IWUSR, SDA_OPS(auto_trigger_show, auto_trigger_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("period1", S_IRUGO | S_IWUSR, SDA_OPS(period_show, period_store), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("timeout1", S_IRUGO, SDA_OPS(timeout_show, NULL), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("bp_type1", S_IRUGO, SDA_OPS(bp_type_show, NULL), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("firmware1", S_IRUGO, SDA_OPS(firmware_show, NULL), slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("tx_disable1_0", S_IRUGO | S_IWUSR, SDA_OPS(tx_disable_show, tx_disable_store), slot_no << 4 | (idx * 2));
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	sda = create_sda("tx_disable1_1", S_IRUGO | S_IWUSR, SDA_OPS(tx_disable_show, tx_disable_store), slot_no << 4 | ((idx * 2) + 1));
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = SDA_ATTR;

	bypass_sysfs_groups[slot_no].attrs[index++] = NULL;
}

void initial_bypass_sysfs_group(int slot_no){
	if (bypass_sysfs_groups[slot_no].name == NULL){
		bypass_sysfs_groups[slot_no].name = kmalloc(sizeof(char) * 8, GFP_KERNEL);
		sprintf((char *)bypass_sysfs_groups[slot_no].name, "slot%d", slot_no);
		bypass_sysfs_groups[slot_no].attrs = kmalloc(sizeof(struct attribute **) * (BYPASS_MAX_SYSFS_PRE_SEG * 2 + 1), GFP_KERNEL);
		bypass_sysfs_groups[slot_no].attrs[0] = NULL;
	}
}

int bypass_unsetup_fs(struct miscdevice *mdev, int slot_no) {
	int i;

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
	sysfs_remove_group(&mdev->this_device->kobj, &bypass_sysfs_groups[slot_no]);
#else
	sysfs_remove_group(&mdev->class->kobj, &bypass_sysfs_groups[slot_no]);
#endif

	for (i = 0; i < (BYPASS_MAX_SYSFS_PRE_SEG * 2); i++){
		kfree(sda_pool[slot_no][i]);
	}

	kfree(bypass_sysfs_groups[slot_no].attrs);
	return 0;
}

int bypass_setup_fs(struct miscdevice *mdev, int slot_no, int type) {
	int err = 0;

	initial_bypass_sysfs_group(slot_no);

	if (type==DEFATTR) {
		//err = sysfs_create_group(&mdev->dev.kobj, &pca9555_defattr_group);
	} else if (type==BYPASS0ATTR) {
		bypass_add_attr0(slot_no, 0);
	} else if (type==BYPASS1ATTR) {
		if (bypass_sysfs_groups[slot_no].attrs[0] == NULL)
			bypass_add_attr0(slot_no, 1);
		else
			bypass_add_attr1(slot_no, 1);
	}

	if(err!=0)
		return 1; 
	else
		return 0;
}

int bypass_register_fs(struct miscdevice *mdev, int slot_no){

#if ( LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18) )
	return sysfs_create_group(&mdev->this_device->kobj, &bypass_sysfs_groups[slot_no]);
#else
	return sysfs_create_group(&mdev->class->kobj, &bypass_sysfs_groups[slot_no]);
#endif
}

#ifdef CONFIG_FERRET_NETWORK_BYPASS
int caswell_set_bypass(int slot_no, int seg_no, int cmd)
{
	struct i2c_client *client;
	int ret;

	mdelay(50);
	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;

	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no,
			get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_status(client, 0, cmd);

	up(&i2cmux_sem);

	return ret;
}

int caswell_set_nextboot(int slot_no, int seg_no, int cmd)
{
	struct i2c_client *client;
	int ret;

	/* bypass timing issue 50ms */
	mdelay(50);
	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;

	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no,
			get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_nextboot(client, 0, cmd);

	up(&i2cmux_sem);

	return ret;
}

int caswell_set_bpe(int slot_no, int seg_no, int cmd)
{
	struct i2c_client *client;
	int ret;

	/* bypass timing issue 50ms */
	mdelay(50);
	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;

	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no,
			get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_bpe(client, 0, cmd);

	up(&i2cmux_sem);

	return ret;
}

int caswell_set_wd_reset(int slot_no, int seg_no, int cmd)
{
	struct i2c_client *client;
	int ret;

	/* bypass timing issue 50ms */
	mdelay(50);
	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;

	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no,
			get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_wdt(client, 0, cmd);

	up(&i2cmux_sem);

	return ret;
}

int caswell_set_wd_timeout(int slot_no, int seg_no, int cmd)
{
	struct i2c_client *client;
	int ret;

	/* bypass timing issue 50ms */
	mdelay(50);
	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;

	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no,
			get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_period(client, 0, cmd);

	up(&i2cmux_sem);

	return ret;
}
#endif /* CONFIG_FERRET_NETWORK_BYPASS */
