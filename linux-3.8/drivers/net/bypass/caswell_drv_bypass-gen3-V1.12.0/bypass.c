/*******************************************************************************

  CASwell(R) Bypass Linux driver
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

#include "kcompat.h"
#include "bypass.h"

static const char fmt_hex[] = "%#04x\n";
static const char fmt_dec[] = "%d\n";
static const char fmt_str[] = "%s\n";
static const char fmt_gen[] = "%d.%d\n";
static const char fmt_ver[] = "V%s\n";

static uint16_t bpmode_trans(char mode)
{
	uint16_t ret_val = 0xff;

	switch(mode){
	case 'n':           // normal mode
		ret_val = 0;
		break;
	case 'o':           // open mode
		ret_val = 1;
		break;
	case 'b':           // bypass mode
		ret_val = 2;
		break;
	case 'e':           // expired mode (non-normal)
		ret_val = 3;
		break;
	}

	return ret_val;
}

// Bypass control nodes
static ssize_t bpdev_show(struct device *d, char *buf, uint16_t opcode)
{
	struct bypass_device *dev = to_bypass_dev(d);
	ssize_t ret = -EINVAL;
	char val[BPDEV_STR_LEN];

	ret = dev->ops.read(dev, opcode, val);

	return ( ret ? ret : sprintf(buf, fmt_str, val) );
}

static ssize_t bpdev_store(struct device *d, const char *buf,
                uint16_t opcode, size_t len)
{
	struct bypass_device *dev = to_bypass_dev(d);
	ssize_t ret = -EINVAL;
	uint16_t val;
	uint16_t vmin = 0, vmax = 1;

	val = (uint16_t) simple_strtoul(buf, NULL, 10);

	// Check input parameter
	switch(opcode){
	case BP_WDT_LOAD:
		if (val == 0)
			opcode = BP_WDT_DISABLE;
		break;
	case BP_PERIOD:
		vmin = 1;
		vmax = dev->info.twdmax;
		break;
	case BP_WDT_STS:
		vmin = 0;
		vmax = 0;
		break;
	case BP_SET_NORM:
		if ( (len-1) == 1){
			val = bpmode_trans(tolower((char)buf[0]));
			if (val)
				opcode = BP_SET_NON;
		}
		else
			goto out;

		vmax = 3;
		break;
	}

	if ((val < vmin) || (val > vmax))
		goto out;

	ret = dev->ops.write(dev, opcode, val);

out:
	return ( ret ? ret : len );
}

#define BYPASS_CTRL_R(name, read_op) \
static ssize_t show_##name(struct device *d, \
                struct device_attribute *attr, char *buf) \
{\
	return bpdev_show(d, buf, read_op);\
}

#define BYPASS_CTRL_W(name, write_op)\
static ssize_t store_##name(struct device *d, \
                struct device_attribute *attr, const char *buf, size_t len) \
{\
	return bpdev_store(d, buf, write_op, len);\
}

#define BYPASS_ENTRY_R(name, opcode) \
	BYPASS_CTRL_R(name, opcode)
#define BYPASS_ENTRY_RW(name, read_op, write_op)\
	BYPASS_CTRL_R(name, read_op)\
	BYPASS_CTRL_W(name, write_op)

BYPASS_ENTRY_RW(bypass, BP_GET_STS, BP_SET_NORM);
BYPASS_ENTRY_RW(wdt, BP_WDT_RS, BP_WDT_LOAD);
BYPASS_ENTRY_RW(period, BP_PERIOD, BP_PERIOD);
BYPASS_ENTRY_RW(timeout, BP_WDT_STS, BP_WDT_STS);


static ssize_t bpinfo_show(struct device *d, char *buf, uint16_t opcode,
                ssize_t (*format)(const struct bypass_device *, char *))
{
	struct bypass_device *dev = to_bypass_dev(d);

#ifdef HW_VERIFICATION
	ssize_t ret = -EINVAL;
	char val[BPDEV_STR_LEN];
	if (opcode){
		ret = dev->ops.read(dev, opcode, val);
		return ( ret ? ret : sprintf(buf, fmt_str, val) );
	}else
#endif
		return (*format)(dev, buf);
}

#define BYPASS_INFO_R(name, opcode, format_string, args...)    \
static ssize_t name##_format(const struct bypass_device *dev, char *buf) \
{                                                            \
	return sprintf(buf, format_string, ##args);              \
}                                                            \
static ssize_t show_##name(struct device *d,              \
                   struct device_attribute *attr, char *buf) \
{                                                            \
	return bpinfo_show(d, buf, opcode, name##_format);    \
}

BYPASS_INFO_R(gen, 0, fmt_gen, (dev->info.gen & 0xF0)>>4, dev->info.gen & 0xF);
BYPASS_INFO_R(bptype, 0, fmt_hex, dev->info.bp_type);
BYPASS_INFO_R(bpindex, BP_BPID, fmt_dec, dev->info.seg_idx);
BYPASS_INFO_R(firmware, BP_FWVER, fmt_ver, dev->info.fwver);
BYPASS_INFO_R(onboard, BP_ONBOARD, fmt_dec, dev->info.onboard);
BYPASS_INFO_R(lantype, BP_LAN_TYPE, fmt_str, dev->info.lan_type);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0) )
DEVICE_ATTR(gen, S_IRUGO, show_gen, NULL);
DEVICE_ATTR(bptype, S_IRUGO, show_bptype, NULL);
DEVICE_ATTR(bpindex, S_IRUGO, show_bpindex, NULL);
DEVICE_ATTR(firmware, S_IRUGO, show_firmware, NULL);
DEVICE_ATTR(onboard, S_IRUGO, show_onboard, NULL);
DEVICE_ATTR(lantype, S_IRUGO, show_lantype, NULL);
DEVICE_ATTR(bypass, S_IRUGO | S_IWUSR, show_bypass, store_bypass);
DEVICE_ATTR(wdt, S_IRUGO | S_IWUSR, show_wdt, store_wdt);
DEVICE_ATTR(period, S_IRUGO | S_IWUSR, show_period, store_period);
DEVICE_ATTR(timeout, S_IRUGO | S_IWUSR, show_timeout, store_timeout);
static struct attribute *bypass_class_attrs[] = {
	&dev_attr_gen.attr,
	&dev_attr_bptype.attr,
	&dev_attr_bpindex.attr,
	&dev_attr_firmware.attr,
	&dev_attr_onboard.attr,
	&dev_attr_lantype.attr,
	&dev_attr_bypass.attr,
	&dev_attr_wdt.attr,
	&dev_attr_period.attr,
	&dev_attr_timeout.attr,
	NULL,
};
ATTRIBUTE_GROUPS(bypass_class);
#else
static struct device_attribute bypass_class_attributes[] = {
	__ATTR(gen, S_IRUGO, show_gen, NULL),
	__ATTR(bptype, S_IRUGO, show_bptype, NULL),
	__ATTR(bpindex, S_IRUGO, show_bpindex, NULL),
	__ATTR(firmware, S_IRUGO, show_firmware, NULL),
	__ATTR(onboard, S_IRUGO, show_onboard, NULL),
	__ATTR(lantype, S_IRUGO, show_lantype, NULL),
	__ATTR(bypass, S_IRUGO | S_IWUSR, show_bypass, store_bypass),
	__ATTR(wdt, S_IRUGO | S_IWUSR, show_wdt, store_wdt),
	__ATTR(period, S_IRUGO | S_IWUSR, show_period, store_period),
	__ATTR(timeout, S_IRUGO | S_IWUSR, show_timeout, store_timeout),
	{}
};
#endif	// End of (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0) )

static void bypass_release(struct device *d)
{
	kfree(to_bypass_dev(d));
}

struct class bypass_class = {
	.owner = THIS_MODULE,
	.name = "bypass",
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23) )
	.dev_release = bypass_release,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0) )
	.dev_groups = bypass_class_groups,
#else
	.dev_attrs = bypass_class_attributes,
#endif	// End of (LINUX_VERSION_CODE >= KERNEL_VERSION(3,12,0) )
#endif // End of (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23) )
};

int bypass_add_ports(struct bypass_device *bp)
{
	struct device *dev = &(bp->dev);
	int error = 0;

	if (bp->port0){
		error = sysfs_create_link(&dev->kobj, &bp->port0->kobj, "port0");
		if (error)
			goto err_port0;
	}
	if (bp->port1){
		error = sysfs_create_link(&dev->kobj, &bp->port1->kobj, "port1");
		if (error)
			goto err_port1;
	}

	return 0;

err_port1:
	if (bp->port0)
		sysfs_remove_link(&dev->kobj, "port0");
err_port0:
	return error;
}

void bypass_del_ports(struct bypass_device *bp)
{
	struct device *dev = &(bp->dev);

	if (bp->port0)
		sysfs_remove_link(&dev->kobj, "port0");
	if (bp->port1)
		sysfs_remove_link(&dev->kobj, "port1");
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) )
static int cas_device_add_attrs(struct device * dev,struct device_attribute *dev_attrs)
{
        int error = 0;
        int i;

        if (dev_attrs) {
                for (i = 0; attr_name(dev_attrs[i]); i++) {
                        error = device_create_file(dev,&dev_attrs[i]);
                        if (error)
                                goto Err;
                }
        }
 Done:
        return error;
 Err:
        while (--i >= 0)
                device_remove_file(dev,&dev_attrs[i]);
        goto Done;
}


static void cas_device_remove_attrs(struct device * dev,struct device_attribute *dev_attrs)
{
        int i;

        if (dev_attrs) {
                for (i = 0; attr_name(dev_attrs[i]); i++)
                        device_remove_file(dev,&dev_attrs[i]);
        }
}

static int device_add_groups(struct device *dev,
                             struct attribute_group **groups)
{
        int error = 0;
        int i;

        if (groups) {
                for (i = 0; groups[i]; i++) {
                        error = sysfs_create_group(&dev->kobj, groups[i]);
                        if (error) {
                                while (--i >= 0)
                                        sysfs_remove_group(&dev->kobj, groups[i]);
                                break;
                        }
                }
        }
        return error;
}

static void device_remove_groups(struct device *dev,
                                 struct attribute_group **groups)
{
        int i;

        if (groups)
                for (i = 0; groups[i]; i++)
                        sysfs_remove_group(&dev->kobj, groups[i]);
}
#endif // End of (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) )

void bypass_unregister_device(struct bypass_device *bp)
{
	struct device *dev = &(bp->dev);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) )
	TYPE_PREFIX struct attribute_group **groups = bp->sysfs_groups;
	cas_device_remove_attrs(dev,bypass_class_attributes);
	device_remove_groups(dev,groups);
#endif // End of (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) )
	bypass_del_ports(bp);
	device_unregister(dev);
}

int bypass_register_device(struct bypass_device *bp)
{
	int status = 0;
	struct device *dev = &(bp->dev);
	TYPE_PREFIX struct attribute_group **groups = bp->sysfs_groups;;

	dev->class = &bypass_class;
	dev->platform_data = bp;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23) )
	dev->groups = groups;
#else
	dev->release = bypass_release;
#endif //End of (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,23) )

	set_dev_name(bp, "%s", bp->name);

	status = device_register(dev);
	if (status)
		goto err_register;

	status = bypass_add_ports(bp);
	if (status)
		device_unregister(dev);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) )
	status = cas_device_add_attrs(dev,bypass_class_attributes);
	if (status)
		cas_device_remove_attrs(dev,bypass_class_attributes);

	status = device_add_groups(dev,groups);
	if(status)
		device_remove_groups(dev,groups);
#endif //End of (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23) )
err_register:
	return status;
}

int __devinit bypass_class_init(void)
{
	return class_register(&bypass_class);
}

void bypass_class_exit(void)
{
	class_unregister(&bypass_class);
}

EXPORT_SYMBOL(bypass_class);
EXPORT_SYMBOL(bypass_register_device);
EXPORT_SYMBOL(bypass_unregister_device);

