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
#include <linux/hwmon-sysfs.h>
#include <linux/delay.h>
#include "bypass_control.h"
//#include "bypass_def.h"
#define INCLUDE_PLATEFORM_MARCO
#include "bypass_platform.h"

extern  bp_interface bp_int[NET_INTERFACE_MAX];
#ifdef CONFIG_FUTURE
extern int bypass_map[];
static struct attribute *bypass_0_attributes[];
struct i2c_client *bypass_client[10];
extern int add_bypass_device(int dev);
#endif
/* following are the sysfs callback functions */
static ssize_t pca9555_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	return sprintf(buf, "%d\n", i2c_smbus_read_byte_data(client,
							     psa->index));
}

static ssize_t pca9555_store(struct device *dev, struct device_attribute *attr,
			     const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 0);
	if (val > 0xff)
		return -EINVAL;
	i2c_smbus_write_byte_data(client, psa->index, val);
	return count;
}

/* Define the device attributes */

#define PCA9555_ENTRY_RO(name, cmd_idx) \
	static SENSOR_DEVICE_ATTR(name, S_IRUGO, pca9555_show, NULL, cmd_idx)

#define PCA9555_ENTRY_RW(name, cmd_idx) \
	static SENSOR_DEVICE_ATTR(name, S_IRUGO | S_IWUSR, pca9555_show, \
				  pca9555_store, cmd_idx)

PCA9555_ENTRY_RO(input0, PCA9555_INPUT_0);
PCA9555_ENTRY_RO(input1, PCA9555_INPUT_1);
PCA9555_ENTRY_RW(output0, PCA9555_OUTPUT_0);
PCA9555_ENTRY_RW(output1, PCA9555_OUTPUT_1);
PCA9555_ENTRY_RW(invert0, PCA9555_INVERT_0);
PCA9555_ENTRY_RW(invert1, PCA9555_INVERT_1);
PCA9555_ENTRY_RW(direction0, PCA9555_DIRECTION_0);
PCA9555_ENTRY_RW(direction1, PCA9555_DIRECTION_1);

static struct attribute *pca9555_attributes[] = {
	&sensor_dev_attr_input0.dev_attr.attr,
	&sensor_dev_attr_input1.dev_attr.attr,
	&sensor_dev_attr_output0.dev_attr.attr,
	&sensor_dev_attr_output1.dev_attr.attr,
	&sensor_dev_attr_invert0.dev_attr.attr,
	&sensor_dev_attr_invert1.dev_attr.attr,
	&sensor_dev_attr_direction0.dev_attr.attr,
	&sensor_dev_attr_direction1.dev_attr.attr,
	NULL
};

static struct attribute_group pca9555_defattr_group = {
	.attrs = pca9555_attributes,
};

static ssize_t bypass_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	int ret=0, bpe=0;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	bypass_reg_set_high_low(client, &bp_seg->get_bypass, bp_seg->reg_reposit, bp_seg->delay_ms);


	//Read the sts0 and sts1 pin which now means bypass status. 0 is normal, x is none normal
	if ((bp_seg->type == BP_COPPER_PIC) ||(bp_seg->type == BP_COPPER_CPLDV2))
		ret = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) & (bp_seg->bp_sts_bit0))? 1:0;
	else if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004)
		ret = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) & (bp_seg->bp_sts_bit1|bp_seg->bp_sts_bit0))? 1:0;

	//ret = 0 means normal status
	if (!ret ) {
		return sprintf(buf, "%d\n", ret);
	} else {
		if(bp_seg->generation==1)//generation 1 BP not detect BPE
			return sprintf(buf, "%d\n", ret);
		else {
			//This is none normal status, we need to check BPE status
			bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
			if (bp_seg->type == BP_COPPER_CPLDV2)
				bpe = copper_cpld_eeprom_read_bpe(bp_seg->eeprom_addr, bp_seg->index);
			else
				bpe = (i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in) & bp_seg->bp_sts_bit0)? 1:0;
			return sprintf(buf, "%d\n", ret+bpe);
		}
	}
	return sprintf(buf, "%d\n", (-1));

}
#ifdef CONFIG_FUTURE
static ssize_t _bypass_store(struct i2c_client *client, const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[0];
#else
static ssize_t bypass_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
#endif
	unsigned long val = simple_strtoul(buf, NULL, 0);
	bp_seg *bp_seg;
	int tmpval;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	if (val > 0xff)
		return -EINVAL;

	switch(val) {
	case SET_NORMAL:
		bypass_reg_set_high_low(client, &bp_seg->set_normal, bp_seg->reg_reposit, bp_seg->delay_ms);
		break;

	case SET_OPEN:
		//This is CPLD type, we need to set runtime bpe value
		if (bp_seg->type == BP_COPPER_CPLDV2) {
			tmpval = i2c_smbus_read_byte_data(client , bp_seg->set_bpe_open.reg_in);
			i2c_smbus_write_byte_data(client, bp_seg->set_bpe_open.reg_out,
				(tmpval&bp_seg->set_bpe_open.cmd));
			//Write to 9559 eeprom for sync
			copper_cpld_eeprom_write_bpe(bp_seg->eeprom_addr, bp_seg->index, 0x0);
		} else {
			bypass_reg_set_high_low(client, &bp_seg->set_bpe_open, bp_seg->reg_reposit, bp_seg->delay_ms);
		}

		bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit, bp_seg->delay_ms);
		break;

	case SET_BP:
		if (bp_seg->type == BP_COPPER_CPLDV2) {
			tmpval = i2c_smbus_read_byte_data(client , bp_seg->set_bpe_bypass.reg_in);
			i2c_smbus_write_byte_data(client, bp_seg->set_bpe_bypass.reg_out,
				(tmpval|bp_seg->set_bpe_bypass.cmd));
			copper_cpld_eeprom_write_bpe(bp_seg->eeprom_addr, bp_seg->index, 0x1);
		} else {
			bypass_reg_set_high_low(client, &bp_seg->set_bpe_bypass, bp_seg->reg_reposit, bp_seg->delay_ms);
		}

		bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit, bp_seg->delay_ms);
		break;

	default :
		break;
	}
        /*The following msleep is used to wait relay to finish moving*/
	msleep(bp_seg->delay_ms);
	return count;
}

#ifdef CONFIG_FUTURE
static ssize_t bypass_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	return _bypass_store(client, buf, count);
}



ssize_t ext_bypass_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];

	if(!client){
	    printk("%s err port:%d buf:%s count:%d\n", __FUNCTION__, port, buf, count);
	    return 0;
	}

	msleep(100);	
	return _bypass_store(client, buf, count);
}
#endif

static ssize_t nextboot_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	int nextboot;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	//generation 1 BP not support nextboot
	if(bp_seg->generation==1)
		return sprintf(buf, "N/A\n" );

	//eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom
	if (bp_seg->eeprom_addr) {
		return sprintf(buf, "%d\n", copper_cpld_eeprom_read_nb(bp_seg->eeprom_addr, 
			bp_seg->index));
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
	}

	nextboot = (i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in)& bp_seg->bp_sts_bit1)? 1:0;
	//If NextBoot=1 => none normal, NextBoot=0 => normal
	return sprintf(buf, "%d\n", nextboot);
}
#ifdef CONFIG_FUTURE
static ssize_t _nextboot_store(struct i2c_client *client, const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[1];
#else
static ssize_t nextboot_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
#endif
	unsigned long val = simple_strtoul(buf, NULL, 0);
	bp_seg *bp_seg;

	if (val > 0xff)
		return -EINVAL;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	//generation 1 BP not support nextboot
	if(bp_seg->generation==1)
		return -EINVAL;
    
	//eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom
	if (bp_seg->eeprom_addr) {
		copper_cpld_eeprom_write_nb(bp_seg->eeprom_addr, bp_seg->index, val);
	} else {
		switch(val) {
		case SET_NB_NORMAL:
			bypass_reg_set_high_low(client, &bp_seg->set_nb_normal, bp_seg->reg_reposit, bp_seg->delay_ms);
			break;

		case SET_NB_NO_NORMAL:
			bypass_reg_set_high_low(client, &bp_seg->set_nb_nonormal, bp_seg->reg_reposit, bp_seg->delay_ms);
			break;

		default :
			break;
		}
	}

	return count;
}

#ifdef CONFIG_FUTURE
static ssize_t nextboot_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);

	return _nextboot_store(client, buf, count);
}

ssize_t ext_nextboot_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];

	if(!client){
	    printk("%s err port:%d buf:%s count:%d\n", __FUNCTION__, port, buf, count);
	    return 0;
	}

	msleep(100);	
	return  _nextboot_store(client, buf, count);
}
#endif

static ssize_t bpe_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	int bpe;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	//generation 1 BP not support bpe
	if(bp_seg->generation==1)
		return sprintf(buf, "N/A\n" );
    
	//eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom
	if (bp_seg->eeprom_addr) {
		return sprintf(buf, "%d\n", copper_cpld_eeprom_read_bpe(bp_seg->eeprom_addr, bp_seg->index));
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
	}
	
	bpe = (i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in) & bp_seg->bp_sts_bit0)? 1:0;
	//If BPE=1 => bypass, BPE=0 => open
	return sprintf(buf, "%d\n", bpe);
}

#ifdef CONFIG_FUTURE
static ssize_t _bpe_store(struct i2c_client *client, const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[2];
#else
static ssize_t bpe_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
#endif
	unsigned long val = simple_strtoul(buf, NULL, 0);
	bp_seg *bp_seg;
        int tmpval,ret=0;

	if (val > 0xff)
		return -EINVAL;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	//generation 1 BP not support bpe
	if(bp_seg->generation==1)
		return -EINVAL;
		
	//eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom
	if (bp_seg->eeprom_addr) {
                switch(val) {
                case SET_BPE_OPEN:
                        tmpval = i2c_smbus_read_byte_data(client , bp_seg->set_bpe_open.reg_in);
                        i2c_smbus_write_byte_data(client, bp_seg->set_bpe_open.reg_out,
                                (tmpval&bp_seg->set_bpe_open.cmd));
                break;
                case SET_BPE_BP:
                        tmpval = i2c_smbus_read_byte_data(client , bp_seg->set_bpe_bypass.reg_in);
                        i2c_smbus_write_byte_data(client, bp_seg->set_bpe_bypass.reg_out,
                                (tmpval|bp_seg->set_bpe_bypass.cmd));
                break;
                }
		copper_cpld_eeprom_write_bpe(bp_seg->eeprom_addr, bp_seg->index, val);  
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_bypass, bp_seg->reg_reposit, bp_seg->delay_ms);
		switch(val) {
		case SET_BPE_OPEN:
			bypass_reg_set_high_low(client, &bp_seg->set_bpe_open, bp_seg->reg_reposit, bp_seg->delay_ms);
            		if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004){
				ret = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) & (bp_seg->bp_sts_bit1|bp_seg->bp_sts_bit0))? 1:0;
				if (ret )
					bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit, bp_seg->delay_ms);
            		}
			break;

		case SET_BPE_BP:
			bypass_reg_set_high_low(client, &bp_seg->set_bpe_bypass, bp_seg->reg_reposit, bp_seg->delay_ms);
            		if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004){
				ret = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) & (bp_seg->bp_sts_bit1|bp_seg->bp_sts_bit0))? 1:0;
				if (ret )
					bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit, bp_seg->delay_ms);
            		}
			break;

		default:
			break;
		}
	}

	return count;
}

#ifdef CONFIG_FUTURE
static ssize_t bpe_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);

	return  _bpe_store(client, buf, count);
}

ssize_t ext_bpe_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];
	
	if(!client){
	    printk("%s err port:%d buf:%s count:%d\n", __FUNCTION__, port, buf, count);
	    return 0;
	}

	msleep(100);	
	return  _bpe_store(client, buf, count);
}
#endif

static ssize_t wdt_timeout_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	struct i2c_client bp_client;
	int timeout;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->wdt_addr)
		bp_client.addr = bp_seg->wdt_addr;

	bypass_reg_set_high_low(client, &bp_seg->get_wdt, bp_seg->reg_reposit, bp_seg->delay_ms);

	//There are low active at fiber PIC and copper CPLD bypass types
	if ((bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004)||(bp_seg->type == BP_COPPER_CPLDV2))
		timeout = (i2c_smbus_read_byte_data(&bp_client, bp_seg->get_wdt.reg_in) & bp_seg->wdt_tmout_bit)? 0:1;
	else
		timeout = (i2c_smbus_read_byte_data(&bp_client, bp_seg->get_wdt.reg_in) & bp_seg->wdt_tmout_bit)? 1:0;
	return sprintf(buf, "%d\n", timeout);

}

static ssize_t wdt_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	struct i2c_client bp_client;
	int wdt_sts;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->wdt_addr)
		bp_client.addr = bp_seg->wdt_addr;

	if(bp_seg->type == BP_COPPER_CPLDV2) {
		//We don't support CPLD wdt status read now
		wdt_sts = 0xff;
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_wdt, bp_seg->reg_reposit, bp_seg->delay_ms);
		wdt_sts = (i2c_smbus_read_byte_data(&bp_client, bp_seg->get_wdt.reg_in) & bp_seg->wdt_en_bit)? 1:0;
	}

	return sprintf(buf, "%d\n", wdt_sts);
}

#ifdef CONFIG_FUTURE
static ssize_t _wdt_store(struct i2c_client *client, const char *buf, size_t count)
{
    	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[3];
#else
static ssize_t wdt_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
#endif
	unsigned long val = simple_strtoul(buf, NULL, 0);
	struct i2c_client bp_client;
	bp_seg *bp_seg;

	if (val > 0xff)
		return -EINVAL;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->wdt_addr)
		bp_client.addr = bp_seg->wdt_addr;

	switch(val) {
	case WDT_DISABLE:
		bypass_reg_set_high_low(&bp_client, &bp_seg->wdt_disable, bp_seg->reg_reposit,
			bp_seg->delay_ms);
		break;

	case WDT_ARM_REFRESH:
		bypass_reg_set_high_low(&bp_client, &bp_seg->wdt_refresh, bp_seg->reg_reposit,
			bp_seg->delay_ms);
		break;

	case WDT_CLEAR:
		bypass_reg_set_high_low(&bp_client, &bp_seg->wdt_clear, bp_seg->reg_reposit,
			bp_seg->delay_ms);
		break;

	default :
		break;
	}

	return count;
}

#ifdef CONFIG_FUTURE
static ssize_t wdt_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);

	return _wdt_store(client, buf, count);
}


ssize_t ext_wdt_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];
	
	if(!client){
	    printk("%s err port:%d buf:%s count:%d\n", __FUNCTION__, port, buf, count);
	    return 0;
	}

	msleep(100);	
	return _wdt_store(client, buf, count);
}
#endif

static u8 copper_pic_wdt_period_tab[] =COPPER_PIC_WDT_PERIOD;

static ssize_t period_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	struct i2c_client bp_client;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->period_addr)
		bp_client.addr = bp_seg->period_addr;

	if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004)
		return sprintf(buf, "%d\n", i2c_smbus_read_byte_data(&bp_client, bp_seg->set_period.reg_in));
	else
		return sprintf(buf, "%d\n", bp_seg->wdt_period);
}

#ifdef CONFIG_FUTURE
static ssize_t _period_store(struct i2c_client *client, const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[4];
#else
static ssize_t period_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
#endif
	unsigned long tmpval, val = simple_strtoul(buf, NULL, 0);
	struct i2c_client bp_client;
	bp_seg *bp_seg;

	/* Decode input */
	if ((val > 0xff) || (val <= 0))
		return -EINVAL;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->period_addr)
		bp_client.addr = bp_seg->period_addr;

	if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004) {
		//Set period is just need to set second unit, only 1~63 sec would be allowed
		if (val> FIBER_ABN482_WDT_PERIOD_MAX)
			val = FIBER_ABN482_WDT_PERIOD_MAX;
		i2c_smbus_write_byte_data(&bp_client, bp_seg->set_period.reg_out, val);
	} else if (bp_seg->type == BP_COPPER_CPLDV2) {
		if (val> COPPER_CPLD_V2P0_WDT_PERIOD_MAX)
			val = COPPER_CPLD_V2P0_WDT_PERIOD_MAX;
		tmpval = i2c_smbus_read_byte_data(&bp_client, bp_seg->set_period.reg_in);
		tmpval &= 0x3;
		tmpval |= val<<2;
		i2c_smbus_write_byte_data(&bp_client, bp_seg->set_period.reg_out, tmpval);
		memset(&bp_seg->wdt_period, val, sizeof(u8));
	} else {
		if (val> COPPER_PIC_WDT_PERIOD_MAX)
			val = COPPER_PIC_WDT_PERIOD_MAX;
		i2c_smbus_write_byte_data(&bp_client, bp_seg->set_period.reg_out, COPPER_PIC_SET_CMD_BITS_HIGH);
		msleep(bp_seg->delay_ms);
		i2c_smbus_write_byte_data(&bp_client, bp_seg->set_period.reg_out, copper_pic_wdt_period_tab[val]);
		memset(&bp_seg->wdt_period, val, sizeof(u8));
	}

	return count;
}

#ifdef CONFIG_FUTURE
static ssize_t period_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);

	return _period_store(client, buf, count);
}

ssize_t ext_period_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];

	if(!client){
	    printk("%s err port:%d buf:%s count:%d\n", __FUNCTION__, port, buf, count);
	    return 0;
	}
	msleep(100);	
	return _period_store(client, buf, count);
}
#endif

static ssize_t bp_type_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	bp_interface *bp_interface;
	if (find_client_bp_interface(&bp_interface, client, psa->index))
		return -EINVAL;
    
	return sprintf(buf, "%s\n", bp_interface->modelname);
}

#ifdef X86_TARGET
static ssize_t tx_disable_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	int ret=0;
	unsigned long cmd;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, 0))
		return -EINVAL;
	
	if (bp_seg->name != BP_CARD_ABN482)
		return -EACCES;
	    

	cmd = (psa->index ? FIBER_ABN482_TX_DISABLE1 : FIBER_ABN482_TX_DISABLE0);
	ret = ((i2c_smbus_read_byte_data(client, PCA9555_OUTPUT_1) & cmd) ? 1:0);

	return sprintf(buf, "%d\n", ret);

}

static ssize_t tx_disable_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	unsigned long val = simple_strtoul(buf, NULL, 0);
	unsigned long cmd;
	bp_seg *bp_seg;
	int tmpval;

	if (find_client_bp_seg(&bp_seg, client, 0))
		return -EINVAL;

	if (bp_seg->name != BP_CARD_ABN482)
		return -EACCES;

	cmd = (psa->index ? FIBER_ABN482_TX_DISABLE1 : FIBER_ABN482_TX_DISABLE0 );
	
	tmpval = i2c_smbus_read_byte_data(client , PCA9555_OUTPUT_1);

	if ( val == 1)
	    i2c_smbus_write_byte_data(client, PCA9555_OUTPUT_1, (tmpval | cmd));
	else if ( val == 0 )
	    i2c_smbus_write_byte_data(client, PCA9555_OUTPUT_1, (tmpval & ~cmd));
	else
		return -EINVAL;

	return count;
}
#endif /*X86_TARGET*/

#define BP_ENTRY_RW(name ,cmd_idx)\
        static SENSOR_DEVICE_ATTR(name, S_IRUGO | S_IWUSR, bypass_show, \
                                  bypass_store, cmd_idx )
#define NEXTBOOT_ENTRY_RW(name ,cmd_idx)\
        static SENSOR_DEVICE_ATTR(name, S_IRUGO | S_IWUSR, nextboot_show, \
                                  nextboot_store, cmd_idx )
#define BPE_ENTRY_RW(name ,cmd_idx)\
        static SENSOR_DEVICE_ATTR(name, S_IRUGO | S_IWUSR, bpe_show, \
                                  bpe_store, cmd_idx )
#define PERIOD_ENTRY_RW(name ,cmd_idx)\
        static SENSOR_DEVICE_ATTR(name, S_IRUGO | S_IWUSR, period_show, \
                                  period_store,cmd_idx)
#define WDT_ENTRY_RW(name ,cmd_idx)\
        static SENSOR_DEVICE_ATTR(name, S_IRUGO | S_IWUSR, wdt_show, \
                                  wdt_store, cmd_idx)
#define TIMEOUT_ENTRY_R(name ,cmd_idx)\
        static SENSOR_DEVICE_ATTR(name, S_IRUGO , wdt_timeout_show, NULL, cmd_idx)

#define BP_TYPE_ENTRY_R(name ,cmd_idx)\
        static SENSOR_DEVICE_ATTR(name, S_IRUGO , bp_type_show, NULL, cmd_idx)

#ifdef X86_TARGET
#define TX_DISABLE_RW(name ,cmd_idx)\
        static SENSOR_DEVICE_ATTR(name, S_IRUGO | S_IWUSR, tx_disable_show, \
                                  tx_disable_store,cmd_idx)
#endif /*X86_TARGET*/

BP_ENTRY_RW(bypass0,0x00);
BP_ENTRY_RW(bypass1,0x01);
NEXTBOOT_ENTRY_RW(nextboot0,0x00);
NEXTBOOT_ENTRY_RW(nextboot1,0x01);
BPE_ENTRY_RW(bpe0,0x00);
BPE_ENTRY_RW(bpe1,0x01);
PERIOD_ENTRY_RW(period0,0x00);
PERIOD_ENTRY_RW(period1,0x01);
WDT_ENTRY_RW(wdt0,0x00);
WDT_ENTRY_RW(wdt1,0x01);
TIMEOUT_ENTRY_R(timeout0,0x00);
TIMEOUT_ENTRY_R(timeout1,0x01);
BP_TYPE_ENTRY_R(bp_type,0x00);
#ifdef X86_TARGET
TX_DISABLE_RW(tx_disable0_0,0x00);
TX_DISABLE_RW(tx_disable0_1,0x01);
#endif /*X86_TARGET*/

static struct attribute *bypass_0_attributes[] = {
	&sensor_dev_attr_bypass0.dev_attr.attr,
	&sensor_dev_attr_nextboot0.dev_attr.attr,
	&sensor_dev_attr_bpe0.dev_attr.attr,
	&sensor_dev_attr_wdt0.dev_attr.attr,
	&sensor_dev_attr_period0.dev_attr.attr,
	&sensor_dev_attr_timeout0.dev_attr.attr,
	&sensor_dev_attr_bp_type.dev_attr.attr,
#ifdef X86_TARGET
	&sensor_dev_attr_tx_disable0_0.dev_attr.attr,
	&sensor_dev_attr_tx_disable0_1.dev_attr.attr,
#endif /*X86_TARGET*/
	NULL
};
static struct attribute *bypass_1_attributes[] = {
	&sensor_dev_attr_bypass1.dev_attr.attr,
	&sensor_dev_attr_nextboot1.dev_attr.attr,
	&sensor_dev_attr_bpe1.dev_attr.attr,
	&sensor_dev_attr_wdt1.dev_attr.attr,
	&sensor_dev_attr_period1.dev_attr.attr,
	&sensor_dev_attr_timeout1.dev_attr.attr,
	NULL
};
static struct attribute_group bypass_0_defattr_group = {
	.attrs = bypass_0_attributes,
};
static struct attribute_group bypass_1_defattr_group = {
	.attrs = bypass_1_attributes,
};

int bypass_unsetup_fs(struct i2c_client *client) {
	sysfs_remove_group(&client->dev.kobj, &bypass_1_defattr_group);
	sysfs_remove_group(&client->dev.kobj, &bypass_0_defattr_group);
 	sysfs_remove_group(&client->dev.kobj, &pca9555_defattr_group);
 	return 0;
}
int bypass_setup_fs(struct i2c_client *client, int type) {
	int err = 0;
#ifdef CONFIG_FUTURE
	int bp_port = add_bypass_device(1);
	if(bp_port >= 0)
	    bypass_client[bp_port] = client;
	printk("port:%d client:%p, address:%x\n", bp_port, client, client->addr);
#endif

	if (type==DEFATTR)
		err = sysfs_create_group(&client->dev.kobj, &pca9555_defattr_group);
	else if (type==BYPASS0ATTR)
		err = sysfs_create_group(&client->dev.kobj, &bypass_0_defattr_group);
	else if (type==BYPASS1ATTR)
		err = sysfs_create_group(&client->dev.kobj, &bypass_1_defattr_group);
	if(err!=0)
		return 1; 
	else
		return 0;
}

#ifdef CONFIG_FUTURE
extern int g_wd_timeout;


int xtm_thread_bypass(void *dummy);
//mode 0 : bypass init
//mode 1 : bypass enable
//mode 2 : bypass disable
static void abn_set_mode(int port, int mode)
{
    char tmp[12];
    switch(mode){
        case 0 :
	    sprintf(tmp, "%d", g_wd_timeout); 
	    ext_bypass_store(port, "0", 2);
	    ext_bpe_store(port, "1", 2);
	    ext_nextboot_store(port, "1", 2);
	    ext_period_store(port, tmp, strlen(tmp)+1);
	    ext_wdt_store(port, "1", 2);
	    break;
        case 1 :
	    ext_period_store(port, "255", 2);
	    ext_bypass_store(port, "2", 2);
	    ext_bpe_store(port, "1", 2);
	    ext_nextboot_store(port, "1", 2);
	    break;
        case 2 :
	    ext_period_store(port, "255", 2);
	    ext_bypass_store(port, "0", 2);
	    ext_bpe_store(port, "0", 2);
	    ext_nextboot_store(port, "0", 2);
	    break;
	default :
	    return;
    }
}

static int get_port_index(int port)
{
    return bypass_map[port];
}

void xtm_abn_init(int port)
{
    abn_set_mode(get_port_index(port), 0);
    kernel_thread((void *)xtm_thread_bypass, NULL, CLONE_KERNEL);	/* future log agent */
}

void xtm_abn_enable(int port)
{
    abn_set_mode(get_port_index(port), 1);
}


void xtm_abn_disable(int port)
{
    abn_set_mode(get_port_index(port), 2);
}

void xtm_abn_wd_timeout(int port, int timeout)
{
    char tmp[12];
    
    sprintf(tmp,"%d", timeout); 
    ext_period_store(get_port_index(port), tmp, strlen(tmp)+1);
}


void xtm_abn_wd_reset(int port)
{
    ext_wdt_store(get_port_index(port), "1", 2);
}
#endif


