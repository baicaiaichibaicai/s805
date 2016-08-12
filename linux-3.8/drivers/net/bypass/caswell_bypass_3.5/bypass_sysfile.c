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

static struct sensor_device_attribute *sda_pool[PCI_ADDCARD_MAX_SLOTS][BYPASS_MAX_SYSFS_PRE_SEG * 2];

static ssize_t bypass_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t bypass_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t nextboot_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t nextboot_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t bpe_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t bpe_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t wdt_timeout_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t wdt_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t wdt_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t period_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t period_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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


static ssize_t bp_type_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	bp_interface *bp_interface;

	if (find_slot_bp_interface(&bp_interface, slot_no))
		return -ENXIO;

	return sprintf(buf, "%s\n", bp_interface->modelname);
}

static ssize_t auto_trigger_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t auto_trigger_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t fw_ver_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = psa->index & 0xf;
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

static ssize_t tx_disable_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = ((psa->index & 0xf) < 2) ? 0 : 1;
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_get_txdisable(client, (psa->index & 0xf) % 2);

	up(&i2cmux_sem);

	return ( ret >= 0 ? sprintf(buf, "%d\n", ret) : ret);
}

static ssize_t tx_disable_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	int slot_no = (psa->index >> 4) & 0xf;
	int seg_no = ((psa->index & 0xf) < 2) ? 0 : 1;
	struct i2c_client *client;
	unsigned long val = simple_strtoul(buf, NULL, 0);
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;
	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no, get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_txdisable(client, (psa->index & 0xf) % 2, val);

	up(&i2cmux_sem);

	return (ret ? ret : count);
}

struct sensor_device_attribute *create_sda(char *name, umode_t mode, 
											ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf),
											ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count), 
											int index){
	struct sensor_device_attribute *sda;

	sda = kmalloc(sizeof(struct sensor_device_attribute), GFP_KERNEL);
	sda->dev_attr.attr.name = kmalloc(sizeof(char) * 32, GFP_KERNEL);
	sprintf((char *)sda->dev_attr.attr.name, "%s", name);
	sda->dev_attr.attr.mode = mode;
	sda->dev_attr.show = show;
	sda->dev_attr.store = store;
	sda->index = index;

	return sda;
}

void bypass_add_attr0(int slot_no, int idx){
	int index = 0;
	struct sensor_device_attribute *sda;

	sda = create_sda("bypass0", S_IRUGO | S_IWUSR, bypass_show, bypass_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("nextboot0", S_IRUGO | S_IWUSR, nextboot_show, nextboot_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("bpe0", S_IRUGO | S_IWUSR, bpe_show, bpe_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("wdt0", S_IRUGO | S_IWUSR, wdt_show, wdt_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("auto_trigger0", S_IRUGO | S_IWUSR, auto_trigger_show, auto_trigger_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("period0", S_IRUGO | S_IWUSR, period_show, period_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("timeout0", S_IRUGO, wdt_timeout_show, NULL, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("bp_type0", S_IRUGO, bp_type_show, NULL, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("firmware0", S_IRUGO, fw_ver_show, NULL, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("tx_disable0_0", S_IRUGO | S_IWUSR, tx_disable_show, tx_disable_store, slot_no << 4 | (idx * 2));
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("tx_disable0_1", S_IRUGO | S_IWUSR, tx_disable_show, tx_disable_store, slot_no << 4 | ((idx * 2) + 1));
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	bypass_sysfs_groups[slot_no].attrs[index++] = NULL;
}

void bypass_add_attr1(int slot_no, int  idx){
	int index = BYPASS_MAX_SYSFS_PRE_SEG;
	struct sensor_device_attribute *sda;


	sda = create_sda("bypass1", S_IRUGO | S_IWUSR, bypass_show, bypass_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("nextboot1", S_IRUGO | S_IWUSR, nextboot_show, nextboot_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("bpe1", S_IRUGO | S_IWUSR, bpe_show, bpe_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("wdt1", S_IRUGO | S_IWUSR, wdt_show, wdt_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("auto_trigger1", S_IRUGO | S_IWUSR, auto_trigger_show, auto_trigger_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("period1", S_IRUGO | S_IWUSR, period_show, period_store, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("timeout1", S_IRUGO, wdt_timeout_show, NULL, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("bp_type1", S_IRUGO, bp_type_show, NULL, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("firmware1", S_IRUGO, fw_ver_show, NULL, slot_no << 4 | idx);
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("tx_disable1_0", S_IRUGO | S_IWUSR, tx_disable_show, tx_disable_store, slot_no << 4 | (idx * 2));
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

	sda = create_sda("tx_disable1_1", S_IRUGO | S_IWUSR, tx_disable_show, tx_disable_store, slot_no << 4 | ((idx * 2) + 1));
	sda_pool[slot_no][index] = sda;
	bypass_sysfs_groups[slot_no].attrs[index++] = &sda->dev_attr.attr;

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

int nfw_bypass_store(int slot_no, int seg_no, int cmd)
{
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
	    return -ERESTARTSYS;

	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no,
		get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_status(client, 0, cmd);

	up(&i2cmux_sem);

	return ret;
}

int nfw_nextboot_store(int slot_no, int seg_no, int cmd)
{
	struct i2c_client *client;
	int ret;

	udelay(10000);
	udelay(10000);
	if (down_interruptible(&i2cmux_sem)) 
		return -ERESTARTSYS;

	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no,
		get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_nextboot(client, 0, cmd);

	up(&i2cmux_sem);

	return ret;
}

int nfw_bpe_store(int slot_no, int seg_no, int cmd)
{
	struct i2c_client *client;
	int ret;

	if (down_interruptible(&i2cmux_sem))
		return -ERESTARTSYS;

	bypass_switch_i2cmux(slot_no);

	client = bypass_get_i2c_client_by_addr(slot_no,
		get_bp_seg_ctrl_addr(slot_no, seg_no));

	ret = bypass_set_bpe(client, 0, cmd);

	up(&i2cmux_sem);

	return ret;
}
