/*******************************************************************************

  CASwell(R) Gen3 Bypass Linux driver
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
#include "bypass.h"
#include "gen3bp.h"

#ifdef CONFIG_FERRET_NETWORK_BYPASS
#include <linux/kthread.h>

#define CAR_GEN3_BP_MAX 		2

extern int select_bypass_driver[];
int num_of_caswell_bypass_device;
static struct bypass_device *gen3_bp[CAR_GEN3_BP_MAX];
#endif /* CONFIG_FERRET_NETWORK_BYPASS */

char gen3bp_driver_name[] = "gen3bp";
uint16_t BPSEGIDX = 0;
//static list_head dev_base_head;


#ifndef HW_VERIFICATION
#define FMT_HEX            "%#04x"
#define FMT_DEC            "%d"
#define FMT_STR            "%s"
#define FMT_VER            "V%d.%d"
static const char *bypass_mode[] = {"n", "b", "o", "MCU fails!!"};
#define bypass_mode(a)     bypass_mode[a]
#else
#define FMT_HEX            "%#02x"
#define FMT_DEC            FMT_HEX
#define FMT_STR            FMT_HEX
#define FMT_VER            "Maj:%#04x Min:%#04x"
#define bypass_mode(a)     a
#endif

static const char *LANTYPE[] = { "Copper", "Fiber" };

//static DEFINE_PCI_DEVICE_TABLE(gen3bp_pci_tbl) = {
static const struct csw_device_id gen3bp_csw_tbl[] = {
	{ CSW_ANY_ID, CSW_CLASS_BYPASS, BPGEN30 },
	{ CSW_ANY_ID, CSW_CLASS_BYPASS, BPGEN31 },
	{ 0, }
};
/*
MODULE_DEVICE_TABLE(pci, gen3bp_pci_tbl);
*/

/*
static int list_bpdevice(struct bypass_device *dev)
{
	list_add_tail(&dev->dev_list, &dev_base_head);
	return 0;
}

static void unlist_bpdevice(struct bypass_device *dev)
{
	list_del(&dev->dev_list);
}
*/

static ssize_t gen3bp_valid(uint16_t opcode, const char *buf, size_t len,
                   uint16_t *val)
{
	ssize_t ret = -EINVAL;
	uint16_t vmin = 0, vmax = 1;

	*val = (uint16_t) simple_strtoul(buf, NULL, 10);

	if ((*val >= vmin) && (*val <= vmax))
		ret = 0;

	return ret;
}

static ssize_t gen3bp_show(struct device *d, char *buf, uint16_t opcode)
{
	struct bypass_device *bp = to_bypass_dev(d);
	ssize_t ret = -EINVAL;
	char val[BPDEV_STR_LEN];

	ret = bp->ops.read(bp, opcode, val);

	return (ret ? ret : sprintf(buf, "%s\n", val));
}

static ssize_t gen3bp_store(struct device *d, const char *buf,
                uint16_t opcode, size_t len)
{
	struct bypass_device *bp = to_bypass_dev(d);
	ssize_t ret = -EINVAL;
	uint16_t val;

	ret = gen3bp_valid(opcode, buf, len, &val);
	ret = bp->ops.write(bp, opcode, val);

	return (ret ? ret : len);
}

#define GEN3BP_CTRL_R(name, read_op) \
static ssize_t name##_show(struct device *d, \
                struct device_attribute *attr, char *buf) \
{\
    return gen3bp_show(d, buf, read_op);\
}

#define GEN3BP_CTRL_W(name, write_op)\
static ssize_t name##_store(struct device *d, \
                struct device_attribute *attr, const char *buf, size_t len) \
{\
    return gen3bp_store(d, buf, write_op, len);\
}

#define GEN3BP_ENTRY_R(name, opcode) \
    GEN3BP_CTRL_R(name, opcode) \
static DEVICE_ATTR(name, S_IRUGO, name##_show, NULL)
#define GEN3BP_ENTRY_W(name, write_op)\
    GEN3BP_CTRL_W(name, write_op)\
static DEVICE_ATTR(name, S_IWUSR, NULL, name##_store)
#define GEN3BP_ENTRY_RW(name, read_op, write_op)\
    GEN3BP_CTRL_R(name, read_op)\
    GEN3BP_CTRL_W(name, write_op)\
static DEVICE_ATTR(name, S_IRUGO | S_IWUSR, name##_show, name##_store)

GEN3BP_ENTRY_RW(bpe, GEN3BP_GET_BPE, GEN3BP_SET_BPE);
GEN3BP_ENTRY_RW(nextboot, GEN3BP_GET_NEXTBOOT, GEN3BP_SET_NEXTBOOT);
GEN3BP_ENTRY_RW(func, GEN3BP_GET_FUNC, GEN3BP_SET_FUNC);
GEN3BP_ENTRY_R(countdown, GEN3BP_WDT_COUNTDOWN);


static struct attribute *bpgen30_attrs[] = {
	&dev_attr_bpe.attr,
	&dev_attr_nextboot.attr,
	&dev_attr_func.attr,
	&dev_attr_countdown.attr,
	NULL
};

static struct attribute *bpgen31_attrs[] = {
	&dev_attr_nextboot.attr,
	&dev_attr_func.attr,
	&dev_attr_countdown.attr,
	NULL
};

static struct attribute_group bpgen30_group = {
	.attrs = bpgen30_attrs,
};

static struct attribute_group bpgen31_group = {
	.attrs = bpgen31_attrs,
};

#ifdef HW_VERIFICATION
GEN3BP_ENTRY_RW(relay, GEN3BP_GET_RELAY, GEN3BP_SET_RELAY);
GEN3BP_ENTRY_R(hwena, GEN3BP_HW_ENA);
GEN3BP_ENTRY_W(reset, GEN3BP_RESET);

static struct attribute *hwvalid_attrs[] = {
	&dev_attr_relay.attr,
	&dev_attr_hwena.attr,
	&dev_attr_reset.attr,
	NULL
};

static struct attribute_group hwvalid_group = {
	.attrs = hwvalid_attrs,
};
#endif

static s32 gen3bp_check_func(struct csw_device *csw)
{
	s32 ret = -EINVAL;
	u8 bp_ena = 0;

	ret = csw->ops.read(csw, BP_OPCODE_HW_ENABLE, &bp_ena);
	if (bp_ena == 1)
		ret = csw->ops.read(csw, BP_OPCODE_SW_ENABLE, &bp_ena);
	if (!ret)
		ret = bp_ena;

	return ret;
}

static s32 gen3bp_read(struct bypass_device *bp, u16 command, char *val)
{
	s32 ret = -EINVAL;
	u8 data, data1;
	struct csw_device *csw = to_csw_device(bp->dev.parent);
	struct csw_operations *ops = &csw->ops;

	switch (command){
	case BP_GET_STS:
		ret = ops->read(csw, BP_OPCODE_BP_STS, &data);
		sprintf(val, FMT_STR, bypass_mode(data));
		break;
	case BP_WDT_RS:
		ret = ops->read(csw, BP_OPCODE_WDT_RS, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case BP_PERIOD:
		ret = ops->read(csw, BP_OPCODE_PERIOD, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case BP_WDT_STS:
		ret = ops->read(csw, BP_OPCODE_WDT_STS, &data);
#ifndef HW_VERIFICATION
		data ^= 0x1u;
#endif
		sprintf(val, FMT_DEC, data);
		break;
	case BP_BPID:
		ret = ops->read(csw, BP_OPCODE_BPID, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case BP_FWVER:
		ret = ops->read(csw, BP_OPCODE_UPVER_MAJOR, &data);
		ret = ops->read(csw, BP_OPCODE_UPVER_MINOR, &data1);
		sprintf(val, FMT_VER, data, data1);
		break;
	case BP_ONBOARD:
		ret = ops->read(csw, BP_OPCODE_ONBOARD, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case BP_LAN_TYPE:
		ret = ops->read(csw, BP_OPCODE_LAN_TYPE, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case GEN3BP_GET_BPE:
		ret = ops->read(csw, BP_OPCODE_BPE, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case GEN3BP_GET_NEXTBOOT:
		ret = ops->read(csw, BP_OPCODE_NEXTBOOT, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case GEN3BP_GET_FUNC:
#ifndef HW_VERIFICATION
		ret = ops->read(csw, BP_OPCODE_HW_ENABLE, &data);
		if (data)
#endif
			ret = ops->read(csw, BP_OPCODE_SW_ENABLE, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case GEN3BP_WDT_COUNTDOWN:
		ret = ops->read(csw, BP_OPCODE_WDT_COUNTDOWN, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case GEN3BP_GET_RELAY:
		ret = ops->read(csw, BP_OPCODE_RLYE_ENABLE, &data);
		sprintf(val, FMT_DEC, data);
		break;
	case GEN3BP_HW_ENA:
		ret = ops->read(csw, BP_OPCODE_HW_ENABLE, &data);
		sprintf(val, FMT_DEC, data);
		break;
	}

	return ret;
}

static s32 gen3bp_write(struct bypass_device *bp, u16 command, u16 val)
{
	s32 ret = -EINVAL;
	struct csw_device *csw = to_csw_device(bp->dev.parent);
	struct csw_operations *ops = &csw->ops;

	switch (command){
	case BP_SET_NORM:
		// Disable WDT first
		ret = ops->write(csw, BP_OPCODE_WDT_DISABLE, 0);
		msleep(RELAY_DELAY);
		ret = ops->write(csw, BP_OPCODE_BP_SET_NORM, 0);
		msleep(RELAY_DELAY);
		break;
	case BP_SET_NON:
		if (bp->info.gen == (BPGEN31 >> 8))
			if (val == 1)
				goto out;
		// Disable WDT first
		ret = ops->write(csw, BP_OPCODE_WDT_DISABLE, 0);
		msleep(RELAY_DELAY);
		// Set BPE if needed
		if ((bp->info.gen == (BPGEN30 >> 8)) && (val < 3)){
			ret = gen3bp_check_func(csw);
			if (ret != 1)
				goto out;
			val--;
			ret = ops->write(csw, BP_OPCODE_BPE, val);
			msleep(RELAY_DELAY);
		}
		ret = ops->write(csw, BP_OPCODE_BP_SET_NON, 0);
		msleep(RELAY_DELAY);
		break;
	case BP_WDT_LOAD:
		ret = ops->write(csw, BP_OPCODE_WDT_LOAD, 0);
		msleep(RELAY_DELAY);
		break;
	case BP_WDT_DISABLE:
		ret = ops->write(csw, BP_OPCODE_WDT_DISABLE, 0);
		msleep(RELAY_DELAY);
		break;
	case BP_PERIOD:
		ret = ops->write(csw, BP_OPCODE_PERIOD, val);
		break;
	case BP_WDT_STS:
		ret = ops->write(csw, BP_OPCODE_WDT_STS, 0);
		break;
	case GEN3BP_SET_BPE:
		ret = ops->write(csw, BP_OPCODE_BPE, val);
		msleep(RELAY_DELAY);
		break;
	case GEN3BP_SET_NEXTBOOT:
		ret = ops->write(csw, BP_OPCODE_NEXTBOOT, val);
		msleep(EEPROM_DELAY);
		break;
	case GEN3BP_SET_FUNC:
		ret = ops->write(csw, BP_OPCODE_SW_ENABLE, val);
		msleep(EEPROM_DELAY);
		break;
	case GEN3BP_SET_RELAY:
		ret = ops->write(csw, BP_OPCODE_RLYE_ENABLE, val);
		msleep(EEPROM_DELAY);
		break;
	case GEN3BP_RESET:
		ret = ops->write(csw, BP_OPCODE_INTERN_RST, 0);
		msleep(RESET_DELAY);
		break;
	}

out:
	return ret;
}

static void gen3bp_set_ports(struct bypass_device *bp, struct csw_device *csw)
{
	struct csw_device *pair = NULL;
	s32 ret;
	u8 idx = 0xFF;

	if (csw->p)
		bp->port0 = csw->p;
	else
		bp->port0 = NULL;

	while((pair = csw_get_device(csw->cwid, csw->class, pair))!=NULL){
		ret = pair->ops.read(pair, BP_OPCODE_BPID, &idx);
		if (!ret)
			if (bp->info.seg_idx == idx && csw != pair)
			break;
	}

	if (bp->info.seg_idx == idx && csw != pair)
		bp->port1 = pair->p;
	else
		bp->port1 = NULL;
}

static int gen3bp_probe(struct csw_device *device,
                         const struct csw_device_id *id )
{
	int ret = -ENODEV;
	int i = 0;
	u8  seg_idx;
	u8  lan;
	struct csw_operations *ops = &device->ops;
	struct bypass_device *bp;

	ret = ops->read(device, BP_OPCODE_BPID, &seg_idx);
	if (ret || (seg_idx != 0xFF))
		goto out;
	ret = ops->write(device, BP_OPCODE_BPID, BPSEGIDX);
	if (ret)
		goto out;

	bp = kzalloc(sizeof(struct bypass_device), GFP_KERNEL);
	SET_BPDEV_DEV(bp, &device->dev);
	csw_set_drvdata(device, bp);
	bp->info.seg_idx = BPSEGIDX;
	// Set port0 and port1
	gen3bp_set_ports(bp, device);

	// Set bypass information
	sprintf(bp->name, "g3bp%d", BPSEGIDX);
	bp->info.gen = device->type >> 8;
	bp->info.bp_type = device->cwid;
	bp->info.twdmax  = TWDMAX;

	ret = ops->read(device, BP_OPCODE_UPVER_MINOR, &bp->info.upver_minor);
	if (ret) goto err_info;
	ret = ops->read(device, BP_OPCODE_UPVER_MAJOR, &bp->info.upver_major);
	if (ret) goto err_info;
	sprintf(bp->info.fwver, "%d.%d", bp->info.upver_major,
	                                 bp->info.upver_minor);
	ret = ops->read(device, BP_OPCODE_ONBOARD, &bp->info.onboard);
	if (ret) goto err_info;
	ret = ops->read(device, BP_OPCODE_LAN_TYPE, &lan);
	if (ret) goto err_info;
	sprintf(bp->info.lan_type, "%s", LANTYPE[lan]);

	// Set bypass attribut group
	switch(device->type){
	case BPGEN30:
		bp->sysfs_groups[i++] = &bpgen30_group;
		break;
	case BPGEN31:
		bp->sysfs_groups[i++] = &bpgen31_group;
		break;
	}

#ifdef HW_VERIFICATION
	bp->sysfs_groups[i] = &hwvalid_group;
#endif

	ret = bypass_register_device(bp);
	if (ret)
		goto err_info;

	bp->ops.read = gen3bp_read;
	bp->ops.write = gen3bp_write;
	BPSEGIDX++;
#ifdef CONFIG_FERRET_NETWORK_BYPASS
	gen3_bp[num_of_caswell_bypass_device] = bp;
	select_bypass_driver[num_of_caswell_bypass_device] = 2;
	num_of_caswell_bypass_device++;
#endif /* CONFIG_FERRET_NETWORK_BYPASS */

	return 0;

err_info:
	csw_set_drvdata(device, NULL);
	kfree(bp);
	ops->write(device, BP_OPCODE_BPID, 0xFF);
out:
	return ret;
}

static void gen3bp_remove(struct csw_device *device)
{
	struct bypass_device *bp = csw_get_drvdata(device);

	if (bp){
		device->ops.write(device, BP_OPCODE_BPID, 0xFF);
		bypass_unregister_device(bp);
	}
}

static struct csw_driver gen3bp_driver = {
	.name         = gen3bp_driver_name,
	.probe        = gen3bp_probe,
	.remove       = gen3bp_remove,
	.id_table     = gen3bp_csw_tbl,
};

int __devinit gen3bp_init(void)
{
	int retval;

	retval = csw_register_driver(&gen3bp_driver);

	return retval;	
}

void __devexit gen3bp_exit(void)
{
	csw_unregister_driver(&gen3bp_driver);
}

/* Prepare for reboot notifier */
void gen3bp_reboot_exit(void)
{
	csw_unregister_driver(&gen3bp_driver);
}

#ifdef CONFIG_FERRET_NETWORK_BYPASS
extern int g_wd_timeout;
extern int zen_thread_bypass(void *dummy);

void caswell_gen3_bypass_enable(int port, int cmd)
{
	if (gen3_bp[port]) {
		gen3_bp[port]->ops.write(gen3_bp[port], BP_SET_NON, 1);
		gen3_bp[port]->ops.write(gen3_bp[port], GEN3BP_SET_BPE, 1);
		gen3_bp[port]->ops.write(gen3_bp[port], GEN3BP_SET_NEXTBOOT, 1);
	}
}

void caswell_gen3_bypass_disable(int port)
{
	if (gen3_bp[port]) {
		gen3_bp[port]->ops.write(gen3_bp[port], BP_SET_NORM, 0);
		gen3_bp[port]->ops.write(gen3_bp[port], GEN3BP_SET_BPE, 0);
		gen3_bp[port]->ops.write(gen3_bp[port], GEN3BP_SET_NEXTBOOT, 0);
	}
}

void caswell_gen3_bypass_wd_reset(int port)
{
	if (gen3_bp[port]) {
		gen3_bp[port]->ops.write(gen3_bp[port], BP_WDT_LOAD, 1);
	}
}

void caswell_gen3_bypass_wd_timeout(int port, int timeout)
{
	if (gen3_bp[port])
		gen3_bp[port]->ops.write(gen3_bp[port], BP_PERIOD, timeout);
}

void caswell_gen3_bypass_init(int port)
{
	static struct task_struct *bypass_task;

	if (gen3_bp[port]) {
		gen3_bp[port]->ops.write(gen3_bp[port], BP_SET_NORM, 0);
		gen3_bp[port]->ops.write(gen3_bp[port], GEN3BP_SET_BPE, 1);
		gen3_bp[port]->ops.write(gen3_bp[port], GEN3BP_SET_NEXTBOOT, 1);
		gen3_bp[port]->ops.write(gen3_bp[port], BP_PERIOD, g_wd_timeout);
		gen3_bp[port]->ops.write(gen3_bp[port], BP_WDT_LOAD, 1);
	}

	bypass_task = kthread_run(zen_thread_bypass, NULL, "bypass");
	if(IS_ERR(bypass_task))
		printk("bypass watchdog start error.\n");
}
#endif /* CONFIG_FERRET_NETWORK_BYPASS */
