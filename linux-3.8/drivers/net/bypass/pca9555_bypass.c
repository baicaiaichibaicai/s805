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
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <future/gcc/string.h>
#include "pca9555_bypass.h"

static struct attribute *bypass_0_attributes[];
int pw_card_conf=0x221;
static int board=0x7090;
int is_portwell_bypass;


int bypass_addr[4]={0x24, 0x25, 0x22, 0x26};
struct i2c_client *bypass_client[4];


bp_interface bp_int[CA_NET_INTERFACE_NO];
static struct i2c_client eeprom_client;
int find_client_bp_seg(bp_seg **seg, struct i2c_client *client, int index);

/* Addresses to scan */
static unsigned short normal_i2c[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 
					0x26, 0x27,0x4c,0x4d, 0x4e, 0x4f ,I2C_CLIENT_END};

/* Insmod parameters */
I2C_CLIENT_INSMOD_1(pca9555);

enum pca9555_cmd
{
	PCA9555_INPUT_0		= 0,
	PCA9555_INPUT_1		= 1,
	PCA9555_OUTPUT_0	= 2,
	PCA9555_OUTPUT_1	= 3,
	PCA9555_INVERT_0	= 4,
	PCA9555_INVERT_1	= 5,
	PCA9555_DIRECTION_0	= 6,
	PCA9555_DIRECTION_1	= 7,
};

static int pca9555_attach_adapter(struct i2c_adapter *adapter);
static int pca9555_detect(struct i2c_adapter *adapter, int address, int kind);
static int pca9555_detach_client(struct i2c_client *client);

/* This is the driver that will be inserted */
static struct i2c_driver ca_bypass_driver = {
	.driver = {
		.name	= "bypass",
	},
	.attach_adapter	= pca9555_attach_adapter,
	.detach_client	= pca9555_detach_client,
};

struct ca_bypass_data {
	struct i2c_client client;
	struct mutex update_lock;
};


static int __init bpcard_conf(char *str)
{
    char *token;
    int cnt=0;

    if(strlen(str) == 0)
	return 1;

    if(str[0] == '=')
	str+=1;
    token = strtok(str, ",");
    pw_card_conf = simple_strtoul(token, NULL, 16);
    while(token){
	token = strtok(NULL, ",");
	if(token == NULL)
	    break;
	bypass_addr[cnt++]=simple_strtoul(token, NULL, 16);
    }
    return 1;
}

__setup("bpcard_conf", bpcard_conf);


u8 copper_cpld_eeprom_read_nb(u8 addr, u8 index)
{
	int nextboot;

	eeprom_client.addr = addr;
	//Check nextboot status(bit2, 3), nextboot=1 => non-normal, nextboot=0 => normal
	nextboot = i2c_smbus_read_byte(&eeprom_client);
	
	if (index == CA_BYPASS_SEGA) {
		if(nextboot & BIT2) {
			return 1;
  		} else {
 	 		return 0;
  		}
	} else if (index == CA_BYPASS_SEGB) {
		if(nextboot & BIT3) {
			return 1;
  		} else {
 	 		return 0;
  		}
	}
	return 0xff;
}

int copper_cpld_eeprom_write_nb(u8 addr, u8 index, u8 val)
{
	u8 tmpval;
	int ret = 0;

	eeprom_client.addr = addr;
	tmpval = i2c_smbus_read_byte(&eeprom_client);
	
	switch(val) {
	case SET_NB_NORMAL:
		if (index == CA_BYPASS_SEGA) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT2));
		} else if (index == CA_BYPASS_SEGB) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT3));
		} else {
			ret = -1;
		}
		break;

	case SET_NB_NO_NORMAL:
		if (index == CA_BYPASS_SEGA) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT2);
		} else if (index == CA_BYPASS_SEGB) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT3);
		} else {
			ret = -1;
		}
		break;

	default :
		ret = -1;
		break;
	}

	return ret;
}

u8 copper_cpld_eeprom_read_bpe(u8 addr, u8 index)
{
	int bpe;

	eeprom_client.addr = addr;
	//Check bpe status(bit0, 1), bpe=1 => bypass, bpe=0 => open
	bpe = i2c_smbus_read_byte(&eeprom_client);

	if (index == CA_BYPASS_SEGA) {
		if(bpe & BIT0) {
			return 1;
  		} else {
 	 		return 0;
  		}
	} else if (index == CA_BYPASS_SEGB) {
		if(bpe & BIT1) {
			return 1;
  		} else {
 	 		return 0;
  		}
	}
	return 0xff;
}

int copper_cpld_eeprom_write_bpe(u8 addr, u8 index, u8 val)
{
	u8 tmpval;
	int ret = 0;

	eeprom_client.addr = addr;
	tmpval = i2c_smbus_read_byte(&eeprom_client);

	switch(val) {
	case SET_BPE_OPEN:
		if (index == CA_BYPASS_SEGA) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT0));
		} else if (index == CA_BYPASS_SEGB) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT1));
		} else {
			ret = -1;
		}
		break;

	case SET_BPE_BYPASS:
		if (index == CA_BYPASS_SEGA) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT0);
		} else if (index == CA_BYPASS_SEGB) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT1);
		} else {
			ret = -1;
		}
		break;

	default :
		ret = -1;
		break;
	}

	return ret;
}

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

int find_client_bp_seg(bp_seg **seg, struct i2c_client *client, int index)
{
	int i, j;

	//From interface 0~CA_NET_INTERFACE_NO-1, find the seg which matching the client->addr
	for (i=0; i<CA_NET_INTERFACE_NO; i++) {
		for(j=0; j<BP_INTERFACE_SEG_MAX; j++) {
			if (client->addr == bp_int[i].seg[j].bp_ctrl_addr) {
				if (bp_int[i].seg[j+index].bp_ctrl_addr == client->addr) {
					*seg = &bp_int[i].seg[j+index];
					return 0;
				} else {
					return 1;
				}
			}
			continue;
		}
	}
	return 1;
}

void bypass_reg_set_high_low(struct i2c_client *client, bp_cmd *bp_cmd, u8 reg_reposit, u8 delay)
{
	u8 bit_high,cmd, tmpval = 0;

	bit_high = bp_cmd->bit_high;
	cmd = bp_cmd->cmd;

	/* If reg_reposit=1, we will try to reserve the original reg data */
	if (reg_reposit) {
		tmpval = i2c_smbus_read_byte_data(client , bp_cmd->reg_in);
		bit_high |= tmpval;
		cmd &= tmpval;
	}

	if (bp_cmd->bit_high) {
		i2c_smbus_write_byte_data(client , bp_cmd->reg_out, bit_high);
		CA_BOARD_DEBUG("Bypass set addr=0x%x, reg=0x%x, cmd=0x%x\n", client->addr,
			bp_cmd->reg_out,  bit_high);
		msleep(delay);
	}

	if (bp_cmd->cmd) {
		i2c_smbus_write_byte_data(client , bp_cmd->reg_out, cmd);
		CA_BOARD_DEBUG("Bypass set addr=0x%x, reg=0x%x, cmd=0x%x\n", client->addr,
			bp_cmd->reg_out, cmd);
		msleep(delay);
	}

	if (bp_cmd->bit_high) {
		i2c_smbus_write_byte_data(client , bp_cmd->reg_out, bit_high);
		CA_BOARD_DEBUG("Bypass set addr=0x%x, reg=0x%x, cmd=0x%x\n", client->addr,
			bp_cmd->reg_out,  bit_high);
	}
}

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
	if ((bp_seg->type == CA_BYPASS_COPPER_PIC) ||(bp_seg->type == CA_BYPASS_COPPER_CPLD_V2P0))
		ret = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) & (bp_seg->bp_sts_bit0))? 1:0;
	else if (bp_seg->type == CA_BYPASS_FIBER_PIC)
		ret = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) & (bp_seg->bp_sts_bit1|bp_seg->bp_sts_bit0))? 1:0;

	//ret = 0 means normal status
	if (!ret ) {
		return sprintf(buf, "%d\n", ret);
	} else {
		//This is none normal status, we need to check BPE status
		bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
		if (bp_seg->type == CA_BYPASS_COPPER_CPLD_V2P0)
			bpe = copper_cpld_eeprom_read_bpe(bp_seg->bp_eeprom_addr, bp_seg->index);
		else
			bpe = (i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in) & bp_seg->bp_sts_bit0)? 1:0;
		return sprintf(buf, "%d\n", ret+bpe);
	}
	return sprintf(buf, "%d\n", (-1));

}



static ssize_t _bypass_store(struct i2c_client *client, const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[0];
	unsigned long val = simple_strtoul(buf, NULL, 0);
	bp_seg *bp_seg;
	int tmpval;

	if(!client)
	    return -EINVAL;


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
		if (bp_seg->type == CA_BYPASS_COPPER_CPLD_V2P0) {
			tmpval = i2c_smbus_read_byte_data(client , bp_seg->set_bpe_open.reg_in);
			i2c_smbus_write_byte_data(client, bp_seg->set_bpe_open.reg_out,
				(tmpval&bp_seg->set_bpe_open.cmd));
			//Write to 9559 eeprom for sync
			copper_cpld_eeprom_write_bpe(bp_seg->bp_eeprom_addr, bp_seg->index, 0x0);
		} else {
			bypass_reg_set_high_low(client, &bp_seg->set_bpe_open, bp_seg->reg_reposit, bp_seg->delay_ms);
		}

		bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit, bp_seg->delay_ms);
		break;

	case SET_BYPASS:
		if (bp_seg->type == CA_BYPASS_COPPER_CPLD_V2P0) {
			tmpval = i2c_smbus_read_byte_data(client , bp_seg->set_bpe_bypass.reg_in);
			i2c_smbus_write_byte_data(client, bp_seg->set_bpe_bypass.reg_out,
				(tmpval|bp_seg->set_bpe_bypass.cmd));
			copper_cpld_eeprom_write_bpe(bp_seg->bp_eeprom_addr, bp_seg->index, 0x1);
		} else {
			bypass_reg_set_high_low(client, &bp_seg->set_bpe_bypass, bp_seg->reg_reposit, bp_seg->delay_ms);
		}

		bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit, bp_seg->delay_ms);
		break;

	default :
		break;
	}

	msleep(bp_seg->delay_ms);
	return count;
}



static ssize_t bypass_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	return _bypass_store(client, buf, count);
}



ssize_t ext_bypass_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];

	msleep(100);	
	return _bypass_store(client, buf, count);
}



static ssize_t nextboot_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	int nextboot;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	//bp_eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom
	if (bp_seg->bp_eeprom_addr) {
		return sprintf(buf, "%d\n", copper_cpld_eeprom_read_nb(bp_seg->bp_eeprom_addr, 
			bp_seg->index));
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
	}

	nextboot = (i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in)& bp_seg->bp_sts_bit1)? 1:0;
	//If NextBoot=1 => none normal, NextBoot=0 => normal
	return sprintf(buf, "%d\n", nextboot);
}

static ssize_t _nextboot_store(struct i2c_client *client, const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[2];
	unsigned long val = simple_strtoul(buf, NULL, 0);
	bp_seg *bp_seg;


	if(!client)
	    return -EINVAL;

	if (val > 0xff)
		return -EINVAL;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;
	//bp_eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom
	if (bp_seg->bp_eeprom_addr) {
		copper_cpld_eeprom_write_nb(bp_seg->bp_eeprom_addr, bp_seg->index, val);
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

static ssize_t nextboot_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);

	return _nextboot_store(client, buf, count);
}

ssize_t ext_nextboot_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];

	msleep(100);	
	return  _nextboot_store(client, buf, count);
}

static ssize_t bpe_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	int bpe;
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	//bp_eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom
	if (bp_seg->bp_eeprom_addr) {
		return sprintf(buf, "%d\n", copper_cpld_eeprom_read_bpe(bp_seg->bp_eeprom_addr, bp_seg->index));
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
	}
	
	bpe = (i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in) & bp_seg->bp_sts_bit0)? 1:0;
	//If BPE=1 => bypass, BPE=0 => open
	return sprintf(buf, "%d\n", bpe);
}


static ssize_t _bpe_store(struct i2c_client *client, const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[2];
	unsigned long val = simple_strtoul(buf, NULL, 0);
	bp_seg *bp_seg;


	if(!client)
	    return -EINVAL;

	if (val > 0xff)
		return -EINVAL;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	//bp_eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom
	if (bp_seg->bp_eeprom_addr) {
		copper_cpld_eeprom_write_bpe(bp_seg->bp_eeprom_addr, bp_seg->index, val);  
	} else {
		switch(val) {
		case SET_BPE_OPEN:
			bypass_reg_set_high_low(client, &bp_seg->set_bpe_open, bp_seg->reg_reposit, bp_seg->delay_ms);
			break;

		case SET_BPE_BYPASS:
			bypass_reg_set_high_low(client, &bp_seg->set_bpe_bypass, bp_seg->reg_reposit, bp_seg->delay_ms);
			break;

		default:
			break;
		}
	}

	return count;
}


static ssize_t bpe_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);

	return  _bpe_store(client, buf, count);
}

ssize_t ext_bpe_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];
	
	msleep(100);	
	return  _bpe_store(client, buf, count);
}

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
	if (bp_seg->bp_wdt_addr)
		bp_client.addr = bp_seg->bp_wdt_addr;

	bypass_reg_set_high_low(client, &bp_seg->get_wdt, bp_seg->reg_reposit, bp_seg->delay_ms);

	//There are low active at fiber PIC and copper CPLD bypass types
	if ((bp_seg->type == CA_BYPASS_FIBER_PIC)||(bp_seg->type == CA_BYPASS_COPPER_CPLD_V2P0))
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
	if (bp_seg->bp_wdt_addr)
		bp_client.addr = bp_seg->bp_wdt_addr;

	if(bp_seg->type == CA_BYPASS_COPPER_CPLD_V2P0) {
		//We don't support CPLD wdt status read now
		wdt_sts = 0xff;
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_wdt, bp_seg->reg_reposit, bp_seg->delay_ms);
		wdt_sts = (i2c_smbus_read_byte_data(&bp_client, bp_seg->get_wdt.reg_in) & bp_seg->wdt_en_bit)? 1:0;
	}

	return sprintf(buf, "%d\n", wdt_sts);
}




static ssize_t _wdt_store(struct i2c_client *client, const char *buf, size_t count)
{
    	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[3];
	unsigned long val = simple_strtoul(buf, NULL, 0);
	struct i2c_client bp_client;
	bp_seg *bp_seg;


	if(!client)
	    return -EINVAL;

	if (val > 0xff)
		return -EINVAL;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->bp_wdt_addr)
		bp_client.addr = bp_seg->bp_wdt_addr;

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


static ssize_t wdt_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);

	return _wdt_store(client, buf, count);
}


ssize_t ext_wdt_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];
	
	msleep(100);	
	return _wdt_store(client, buf, count);
}


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
	if (bp_seg->bp_period_addr)
		bp_client.addr = bp_seg->bp_period_addr;

	if (bp_seg->type == CA_BYPASS_FIBER_PIC)
		return sprintf(buf, "%d\n", i2c_smbus_read_byte_data(&bp_client, bp_seg->set_period.reg_in));
	else
		return sprintf(buf, "%d\n", bp_seg->wdt_period);
}

static ssize_t _period_store(struct i2c_client *client, const char *buf, size_t count)
{
	struct sensor_device_attribute *psa = (struct sensor_device_attribute *)bypass_0_attributes[4];
	unsigned long tmpval, val = simple_strtoul(buf, NULL, 0);
	struct i2c_client bp_client;
	bp_seg *bp_seg;


	if(!client)
	    return -EINVAL;

	/* Decode input */
	if ((val > 0xff) || (val <= 0))
		return -EINVAL;
	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->bp_period_addr)
		bp_client.addr = bp_seg->bp_period_addr;

	if (bp_seg->type == CA_BYPASS_FIBER_PIC) {
		//Set period is just need to set second unit, only 1~63 sec would be allowed
		if (val> FIBER_WDT_PERIOD_MAX)
			val = FIBER_WDT_PERIOD_MAX;
		i2c_smbus_write_byte_data(&bp_client, bp_seg->set_period.reg_out, val);
	} else if (bp_seg->type == CA_BYPASS_COPPER_CPLD_V2P0) {
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


static ssize_t period_store(struct device *dev, struct device_attribute *attr,
                             const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);

	return _period_store(client, buf, count);
}

ssize_t ext_period_store(int port, const char *buf, size_t count)
{
	struct i2c_client *client = bypass_client[port];
	
	msleep(100);	
	return _period_store(client, buf, count);
}


static ssize_t bp_type_show(struct device *dev, struct device_attribute *attr,
                            char *buf)
{
	struct sensor_device_attribute *psa = to_sensor_dev_attr(attr);
	struct i2c_client *client = to_i2c_client(dev);
	bp_seg *bp_seg;

	if (find_client_bp_seg(&bp_seg, client, psa->index))
		return -EINVAL;
#ifdef CAVIUM_BYPASS_ENABLE
	if (bp_seg->type == CA_BYPASS_FIBER_PIC) {
		return sprintf(buf, "Fiber-Bypass\n");
	} else if (bp_seg->type == CA_BYPASS_COPPER_PIC) {
		return sprintf(buf, "Copper-PIC-Bypass\n");
	} else if (bp_seg->type == CA_BYPASS_COPPER_CPLD_V2P0) {
		return sprintf(buf, "Copper-CPLD-Bypass\n");
	} else {
		return sprintf(buf, "Unknow-Type\n");
	}
#else
	if (bp_seg->name == CA_BYPASS_ABN482){
		return sprintf(buf, "ABN482\n");
	} else if (bp_seg->name == CA_BYPASS_ABN484){
		return sprintf(buf, "ABN484\n");
	}else if (bp_seg->name == CA_BYPASS_NAR5520){
		return sprintf(buf, "NAR5520\n");
	} else if (bp_seg->name == CA_BYPASS_NAR5530){
		return sprintf(buf, "NAR5530\n");
	} else if (bp_seg->name == CA_BYPASS_NAR5650){
		return sprintf(buf, "NAR5650\n");
	} else {
		return sprintf(buf, "Unknow-Type\n");
	}
#endif //CAVIUM_BYPASS_ENABLE


}

#define BYPASS_ENTRY_RW(name ,cmd_idx)\
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

BYPASS_ENTRY_RW(bypass0,0x00);
BYPASS_ENTRY_RW(bypass1,0x01);
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

static struct attribute *bypass_0_attributes[] = {
	&sensor_dev_attr_bypass0.dev_attr.attr,
	&sensor_dev_attr_nextboot0.dev_attr.attr,
	&sensor_dev_attr_bpe0.dev_attr.attr,
        &sensor_dev_attr_wdt0.dev_attr.attr,
        &sensor_dev_attr_period0.dev_attr.attr,
	&sensor_dev_attr_timeout0.dev_attr.attr,
	&sensor_dev_attr_bp_type.dev_attr.attr,
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

int client_is_bp_wdt(struct i2c_client *client, int interface) {
	int i;

	//search to find if the client addr is fiber bypass wdt ctrl
	for (i=0; i<BP_INTERFACE_SEG_MAX; i++) {
		if (client->addr == bp_int[interface].seg[i].bp_wdt_addr)
			return 1;
		continue;
	}

	return 0;
}

int client_is_bp_eeprom(struct i2c_client *client) {
	int i,j;

	//search to find if the client addr is fiber bypass wdt ctrl
        for (i=0; i<CA_NET_INTERFACE_NO; i++) {
		for (j=0; j<BP_INTERFACE_SEG_MAX; j++) {
			if(bp_int[i].seg[j].type==CA_BYPASS_COPPER_CPLD_V2P0)
				if (client->addr == bp_int[i].seg[j].bp_eeprom_addr)
					return 1;
			continue;
		}
	}
	return 0;
}

int client_is_bp_ctrl(struct i2c_client *client, int interface) {
	int i;

	//search to find if the client addr is fiber bypass wdt ctrl
	for (i=0; i<BP_INTERFACE_SEG_MAX; i++) {
		if (client->addr == bp_int[interface].seg[i].bp_ctrl_addr)
			return 1;
		continue;
	}

	return 0;
}

int ca_bypass_client_setup(struct i2c_client *client) {
	struct i2c_client bp_client;
	bp_seg *bp_seg;
	int i, j;

	memcpy(&bp_client, client, sizeof(struct i2c_client));

	for(i=0; i<CA_NET_INTERFACE_NO; i++) {
		if (bp_int[i].seg[0].type == CA_BYPASS_FIBER_PIC) {
			if (client_is_bp_wdt(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, 0x00);
#ifdef CAVIUM_BYPASS_ENABLE                
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, 0xE8);
#else				
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, 0x08);
				//init BP_Disa
				i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, 0xE7);
#endif //CAVIUM_BYPASS_ENABLE            
			} else if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, 0x80);
#ifdef CAVIUM_BYPASS_ENABLE                
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, 0xFF);
#else				
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, 0xC0);
#endif //CAVIUM_BYPASS_ENABLE
				sysfs_create_group(&client->dev.kobj, &bypass_0_defattr_group);
#ifdef CAVIUM_BYPASS_ENABLE                
				sysfs_create_group(&client->dev.kobj, &bypass_1_defattr_group);                
#endif //CAVIUM_BYPASS_ENABLE
				for (j=0; j<2; j++) {
					find_client_bp_seg(&bp_seg, client, j);
					if (bp_seg->bp_wdt_addr)
						bp_client.addr = bp_seg->bp_wdt_addr;
					//Forcing do WDT clear at setup time
					bypass_reg_set_high_low(&bp_client, &bp_seg->wdt_clear, bp_seg->reg_reposit, bp_seg->delay_ms);
				}
			}
		} else if (bp_int[i].seg[0].type == CA_BYPASS_COPPER_PIC) {
			if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, 0x70);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, 0x70);
				sysfs_create_group(&client->dev.kobj, &bypass_0_defattr_group);
				sysfs_create_group(&client->dev.kobj, &bypass_1_defattr_group);                
			}
		} else if (bp_int[i].seg[0].type == CA_BYPASS_COPPER_CPLD_V2P0) {
			if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, 0x00);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, 0xC0);
				sysfs_create_group(&client->dev.kobj, &bypass_0_defattr_group);
			}
		}
	}
	return 0;
}

static int pca9555_attach_adapter(struct i2c_adapter *adapter)
{
	return i2c_probe(adapter, &addr_data, pca9555_detect);
}

/* This function is called by i2c_probe */
static int pca9555_detect(struct i2c_adapter *adapter, int address, int kind)
{
	struct i2c_client *new_client;
	struct ca_bypass_data *data;
	int err = 0, bp;
	
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_READ_BYTE_DATA
					    | I2C_FUNC_SMBUS_BYTE))
		goto exit;

	/* OK. For now, we presume we have a valid client. We now create the
	   client structure, even though we cannot fill it completely yet. */
	if (!(data = kzalloc(sizeof(struct ca_bypass_data), GFP_KERNEL))) {
		err = -ENOMEM;
		goto exit;
	}

	new_client = &data->client;
	i2c_set_clientdata(new_client, data);
	new_client->addr = address;
	new_client->adapter = adapter;

	for(bp=0;bp<4;bp++)
	    if(bypass_addr[bp] == address)
		bypass_client[bp] = new_client;

	if(eeprom_client.addr==0)
		memcpy(&eeprom_client, new_client, sizeof(struct i2c_client));

	if(client_is_bp_eeprom(new_client)) {
		memcpy(&eeprom_client, new_client, sizeof(struct i2c_client));
		return 0;
	} else {
		new_client->driver = &ca_bypass_driver;
		new_client->flags = 0;
		if (kind < 0) {
			/* Detection: the pca9555 only has 8 registers (0-7).
			   A read of 7 should succeed, but a read of 8 should fail. */
			if ((i2c_smbus_read_byte_data(new_client, 7) < 0) ||
			    (i2c_smbus_read_byte_data(new_client, 8) >= 0))
				goto exit_kfree;
		}

		strlcpy(new_client->name, "bypass", I2C_NAME_SIZE);

		/* Tell the I2C layer a new client has arrived */
		if ((err = i2c_attach_client(new_client)))
			goto exit_kfree;
#ifdef CONFIG_CA_BOARD_DEBUG	
		/* Register sysfs hooks */
		err = sysfs_create_group(&new_client->dev.kobj,
					 &pca9555_defattr_group);
#endif

		ca_bypass_client_setup(new_client);	

		if (err)
			goto exit_detach;

		return 0;
	}
exit_detach:
	i2c_detach_client(new_client);
exit_kfree:
	kfree(data);
exit:
	if(err == 0)
	    is_portwell_bypass = 1;
	return err;
}

static int pca9555_detach_client(struct i2c_client *client)
{
	int err;

	sysfs_remove_group(&client->dev.kobj, &pca9555_defattr_group);

	if ((err = i2c_detach_client(client)))
		return err;

	kfree(i2c_get_clientdata(client));
	return 0;
}

/* Fill the bypass segments cmd structure */
void bp_cmd_setup(bp_cmd *cmd_struct, u8 reg_out, u8 reg_in, u8 cmd, u8 bit_high)
{
	memset(&(cmd_struct->reg_out), reg_out, sizeof(u8));
	memset(&(cmd_struct->reg_in), reg_in, sizeof(u8));
	memset(&(cmd_struct->cmd), cmd, sizeof(u8));
	memset(&(cmd_struct->bit_high), bit_high, sizeof(u8));
}

/* Setup the bypass segnebt function of copper cpld type */
void copper_cpld_v2p0_bp_seg_set(bp_seg *seg_struct, u8 index)
{
	bp_cmd_setup(&(seg_struct->set_normal), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_SET_NORMAL, (u8)~(COPPER_CPLD_V2P0_SET_NORMAL));

	bp_cmd_setup(&(seg_struct->set_nonormal), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_SET_NONORMAL, (u8)~(COPPER_CPLD_V2P0_SET_NONORMAL));

	bp_cmd_setup(&(seg_struct->set_bpe_open), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_SET_BPE_OPEN, 0);

	bp_cmd_setup(&(seg_struct->set_bpe_bypass), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_SET_BPE_BYPASS, 0);

	bp_cmd_setup(&(seg_struct->get_bypass), 0, PCA9555_INPUT_1, 0, 0);

	bp_cmd_setup(&(seg_struct->get_nb), 0, PCA9555_INPUT_1, 0, 0);

	bp_cmd_setup(&(seg_struct->get_wdt), 0, PCA9555_INPUT_1, 0, 0);

	bp_cmd_setup(&(seg_struct->wdt_disable), PCA9555_OUTPUT_0, PCA9555_INPUT_0,
			COPPER_CPLD_V2P0_DISABLE_WDT, (u8)~(COPPER_CPLD_V2P0_DISABLE_WDT));

	bp_cmd_setup(&(seg_struct->wdt_refresh), PCA9555_OUTPUT_0, PCA9555_INPUT_0,
			COPPER_CPLD_V2P0_REFRESH_BP_WDT, (u8)~(COPPER_CPLD_V2P0_REFRESH_BP_WDT));

	bp_cmd_setup(&(seg_struct->wdt_clear), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_CLR_BP_WDT, (u8)~(COPPER_CPLD_V2P0_CLR_BP_WDT));

	bp_cmd_setup(&(seg_struct->set_period), PCA9555_OUTPUT_0, PCA9555_INPUT_0, 0, 0);
}

/* Setup the bypass segnebt function of copper pic type */
void copper_pic_bp_seg_set(bp_seg *seg_struct, u8 index)
{
	u8 cmd_bit_high = COPPER_PIC_SET_CMD_BITS_HIGH;
	u8 reg_out = PCA9555_OUTPUT_0+(index%2);
	u8 reg_in = PCA9555_INPUT_0+(index%2);

	bp_cmd_setup(&(seg_struct->set_normal), reg_out, reg_in, COPPER_PIC_SET_NORMAL,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nonormal), reg_out, reg_in, COPPER_PIC_SET_NONORMAL,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_bpe_open), reg_out, 0, COPPER_PIC_SET_BPE_OPEN,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_bpe_bypass), reg_out, 0, COPPER_PIC_SET_BPE_BYPASS,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nb_normal), reg_out, 0, COPPER_PIC_SET_NB_NORMAL,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nb_nonormal), reg_out, 0, COPPER_PIC_SET_NB_NO_NORMAL,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_bypass), reg_out, reg_in, COPPER_PIC_GET_MODE_STS,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_nb), reg_out, reg_in, COPPER_PIC_GET_NB_BPE_STS,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_wdt), reg_out, reg_in, COPPER_PIC_GET_BP_WDT_STS,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->wdt_disable), reg_out, 0, COPPER_PIC_BP_WDT_DIS,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->wdt_refresh), reg_out, 0, COPPER_PIC_BP_WDT_REFRESH,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->wdt_clear), reg_out, 0, COPPER_PIC_BP_WDT_CLR,
		cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_period), reg_out, 0, 0, cmd_bit_high);
}

/* Setup the bypass segnebt function of fiber pic type */
void fiber_pic_bp_seg_set(bp_seg *seg_struct, u8 index)
{
	u8 cmd_bit_high = FIBER_SET_CMD_BITS_HIGH;

	bp_cmd_setup(&(seg_struct->set_normal), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_SET_NORMAL(index), FIBER_SET_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nonormal), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_SET_NONORMAL(index), FIBER_SET_NONORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_bpe_open), PCA9555_OUTPUT_0, 0,
		FIBER_SET_OPEN(index), FIBER_SET_OPEN(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_bpe_bypass), PCA9555_OUTPUT_0, 0,
		FIBER_SET_BYPASS(index), FIBER_SET_BYPASS(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nb_normal), PCA9555_OUTPUT_0, 0,
		FIBER_SET_NB_NORMAL(index), FIBER_SET_NB_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nb_nonormal), PCA9555_OUTPUT_0, 0,
		FIBER_SET_NB_NO_NORMAL(index), FIBER_SET_NB_NO_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_bypass), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_GET_MODE_STS, FIBER_GET_MODE_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_nb), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_GET_NB_BPE_STS, FIBER_GET_NB_BPE_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_wdt), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
		FIBER_GET_BP_WDT_STS, FIBER_GET_BP_WDT_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->wdt_disable), PCA9555_OUTPUT_1, 0,
		FIBER_DISABLE_WDT, 0);

	bp_cmd_setup(&(seg_struct->wdt_refresh), PCA9555_OUTPUT_1, 0,
		FIBER_REFRESH_BP_WDT, FIBER_REFRESH_BP_WDT|FIBER_WDT_LOAD_BIT);

	bp_cmd_setup(&(seg_struct->wdt_clear), PCA9555_OUTPUT_1, 0,
		FIBER_CLR_BP_WDT, FIBER_CLR_BP_WDT|FIBER_WDT_CLEAR_BIT);

	bp_cmd_setup(&(seg_struct->set_period), PCA9555_OUTPUT_0, PCA9555_INPUT_0, 0, 0);
}

int ca_bypass_init(void) {
	int interface, no, pos, i;
	#ifdef CAVIUM_BYPASS_ENABLE
		u8 ca_bypass_ctrl_addr[] = CA_NET_BP_CTRL_ADDR;
		u8 ca_bypass_wdt_addr[] = CA_NET_BP_WDT_ADDR;
		u8 ca_bypass_period_addr[] = CA_NET_BP_PERIOD_ADDR;
		u8 ca_bypass_eeprom_addr[] = CA_NET_BP_EEPROM_ADDR;
		u8 ca_bypass_seg_no[] = CA_NET_BYPASS_SEG_NO;
		u8 ca_bypass_type[] = CA_NET_BYPASS_TYPE;
        #else
		u8 ca_bypass_ctrl_addr[BP_INTERFACE_SEG_MAX*2];
		u8 ca_bypass_wdt_addr[BP_INTERFACE_SEG_MAX*2] ;
		u8 ca_bypass_period_addr[BP_INTERFACE_SEG_MAX*2] ;
		u8 ca_bypass_eeprom_addr[BP_INTERFACE_SEG_MAX*2] ;
		u8 ca_bypass_seg_no[BP_INTERFACE_SEG_MAX] ;
		u8 ca_bypass_type[BP_INTERFACE_SEG_MAX];       
		u8 ca_bypass_name[BP_INTERFACE_SEG_MAX];   
		for(i=0;i<BP_INTERFACE_SEG_MAX;i++)
		{
			ca_bypass_type[i]=CA_BYPASS_TYPE[i];
			ca_bypass_name[i]=CA_BYPASS_NAME[i];
			ca_bypass_seg_no[i]=CA_BYPASS_SEG_NO[i];
		}
		for(i=0;i<BP_INTERFACE_SEG_MAX*2;i++)
		{
			ca_bypass_ctrl_addr[i]=CA_BP_CTRL_ADDR[i];
			ca_bypass_eeprom_addr[i]=CA_BP_EEPROM_ADDR[i];
			ca_bypass_wdt_addr[i]=CA_BP_WDT_ADDR[i];
			ca_bypass_period_addr[i]=CA_BP_PERIOD_ADDR[i];
		}    
	#endif    //CAVIUM_BYPASS_ENABLE

	for(interface=0; interface<CA_NET_INTERFACE_NO; interface++) {
		bp_int[interface].seg_no = ca_bypass_seg_no[interface];

		for(no=0; no<ca_bypass_seg_no[interface]; no++) {
			//To find the exaclty position
			pos = 0;
			for(i=0; i<interface; i++)
				pos += ca_bypass_seg_no[i];
			pos += no;
                            
			bp_int[interface].seg[no].type = ca_bypass_type[interface];
			bp_int[interface].seg[no].name= ca_bypass_name[interface];
			bp_int[interface].seg[no].index = (no%2);
			bp_int[interface].seg[no].bp_ctrl_addr = ca_bypass_ctrl_addr[pos];
			bp_int[interface].seg[no].bp_wdt_addr = ca_bypass_wdt_addr[pos];
			bp_int[interface].seg[no].bp_period_addr = ca_bypass_period_addr[pos];
			bp_int[interface].seg[no].bp_eeprom_addr = ca_bypass_eeprom_addr[pos];

			if (ca_bypass_type[interface] == CA_BYPASS_COPPER_PIC) {
				bp_int[interface].seg[no].delay_ms = COPPER_PIC_MICROCON_DELAY;
				bp_int[interface].seg[no].bp_sts_bit0 = COPPER_PIC_STS_BIT0;
				bp_int[interface].seg[no].bp_sts_bit1 = COPPER_PIC_STS_BIT1;
				bp_int[interface].seg[no].wdt_en_bit = COPPER_PIC_WDT_STS_BIT;
				bp_int[interface].seg[no].wdt_tmout_bit = COPPER_PIC_WDT_TIMEOUT_BIT;
				bp_int[interface].seg[no].reg_reposit = 0;
				copper_pic_bp_seg_set(&bp_int[interface].seg[no], bp_int[interface].seg[no].index);
			} else if (ca_bypass_type[interface] == CA_BYPASS_COPPER_CPLD_V2P0) {
				bp_int[interface].seg[no].delay_ms = COPPER_CPLD_V2P0_MICROCON_DELAY;
				bp_int[interface].seg[no].bp_sts_bit0 = COPPER_CPLD_V2P0_STS_BIT0;
				bp_int[interface].seg[no].bp_sts_bit1 = COPPER_CPLD_V2P0_STS_BIT1;
				bp_int[interface].seg[no].wdt_en_bit = COPPER_CPLD_V2P0_WDT_STS_BIT;
				bp_int[interface].seg[no].wdt_tmout_bit = COPPER_CPLD_V2P0_WDT_TIMEOUT_BIT;
				bp_int[interface].seg[no].reg_reposit = 1;
				copper_cpld_v2p0_bp_seg_set(&bp_int[interface].seg[no], bp_int[interface].seg[no].index);
			} else if (ca_bypass_type[interface] == CA_BYPASS_FIBER_PIC) {
				bp_int[interface].seg[no].delay_ms = FIBER_MICROCON_DELAY;
				//index = 0 means it's seg a
				if (bp_int[interface].seg[no].index) {
					bp_int[interface].seg[no].bp_sts_bit0 = FIBER_SEGB_STS_BIT0;
					bp_int[interface].seg[no].bp_sts_bit1 = FIBER_SEGB_STS_BIT1;
				} else {
					bp_int[interface].seg[no].bp_sts_bit0 = FIBER_SEGA_STS_BIT0;
					bp_int[interface].seg[no].bp_sts_bit1 = FIBER_SEGA_STS_BIT1;
				}
				bp_int[interface].seg[no].wdt_en_bit = FIBER_WDT_BP_STS_BIT;
				bp_int[interface].seg[no].wdt_tmout_bit = FIBER_WDT_TIMEOUT_BIT;
				bp_int[interface].seg[no].reg_reposit = 0;
				fiber_pic_bp_seg_set(&bp_int[interface].seg[no], bp_int[interface].seg[no].index);
			}
			CA_BOARD_DEBUG("bp%d:%d ctrl=0x%x, wdt=0x%x, period=0x%x, eeprom=0x%x\n", interface,
				no, bp_int[interface].seg[no].bp_ctrl_addr, bp_int[interface].seg[no].bp_wdt_addr,
				bp_int[interface].seg[no].bp_period_addr, bp_int[interface].seg[no].bp_eeprom_addr);
		}
	}
	return 0;	
}

int string_init(void){
	int INTERFACE_NO=0,address_num=0,board_num=0,slot_num=0;
	if((pw_card_conf!=0)||(board!=0)){     
		switch(board)/*if board config found*/
		{
			case NAR5650:                
			INTERFACE_NO++;
			for(address_num=0;address_num<BYPASS_5650_SEG_NO;address_num++){                
				CA_BYPASS_TYPE[slot_num]=CA_BYPASS_COPPER_CPLD_V2P0;
				CA_BYPASS_NAME[slot_num]=CA_BYPASS_NAR5650;                
				CA_BYPASS_SEG_NO[slot_num]=BYPASS_5650_SEG_NO;
				CA_BP_CTRL_ADDR[address_num]=i2c_5650_ctrl_addr[(address_num)];
				CA_BP_EEPROM_ADDR[address_num]=i2c_5650_eeprom_addr[address_num];
			}
			board_num=1;
			break;
			case NAR5530:
			INTERFACE_NO++;
			for(address_num=0;address_num<BYPASS_5530_SEG_NO;address_num++){                
				CA_BYPASS_TYPE[slot_num]=CA_BYPASS_COPPER_CPLD_V2P0;
				CA_BYPASS_NAME[slot_num]=CA_BYPASS_NAR5530;
				CA_BYPASS_SEG_NO[slot_num]=BYPASS_5530_SEG_NO;
				CA_BP_CTRL_ADDR[address_num]=i2c_5530_ctrl_addr[(address_num)];
				CA_BP_EEPROM_ADDR[address_num]=i2c_5530_eeprom_addr[address_num];
			}
			board_num=1;
			break;            
			case NAR5520:
			INTERFACE_NO++;
			for(address_num=0;address_num<BYPASS_5520_SEG_NO;address_num++){                
				CA_BYPASS_TYPE[slot_num]=CA_BYPASS_COPPER_CPLD_V2P0;
				CA_BYPASS_NAME[slot_num]=CA_BYPASS_NAR5520;
				CA_BYPASS_SEG_NO[slot_num]=BYPASS_5520_SEG_NO;
				CA_BP_CTRL_ADDR[address_num]=i2c_5520_ctrl_addr[(address_num)];
				CA_BP_EEPROM_ADDR[address_num]=i2c_5520_eeprom_addr[address_num];
			}
			board_num=1;
			break;            
		}
        
                  /*config slot information*/
		for(slot_num=board_num;slot_num<3;slot_num++)/*if board_num > 0 express board need config*/
		{		
			if(((pw_card_conf>>((2-slot_num)*4))&0xf)!=0x00)/*if conf config dound*/
			{
				INTERFACE_NO++;
				switch(((pw_card_conf>>((2-slot_num)*4))&0xf)){
					case 1://for abn484
						CA_BYPASS_TYPE[slot_num]=CA_BYPASS_COPPER_CPLD_V2P0;
						CA_BYPASS_NAME[slot_num]=CA_BYPASS_ABN484;                        
						CA_BYPASS_SEG_NO[slot_num]=2;
						CA_BP_CTRL_ADDR[address_num]=i2c_ctrl_addr[(slot_num*2)];
						CA_BP_CTRL_ADDR[address_num+1]=i2c_ctrl_addr[(slot_num*2)+1];
						CA_BP_EEPROM_ADDR[address_num]=i2c_eeprom_addr[slot_num];
						CA_BP_EEPROM_ADDR[address_num+1]=i2c_eeprom_addr[slot_num];
						address_num+=2;
					break;
				case 2://for abn482
						CA_BYPASS_TYPE[slot_num]=  CA_BYPASS_FIBER_PIC;
						CA_BYPASS_NAME[slot_num]=CA_BYPASS_ABN482;                        
						CA_BYPASS_SEG_NO[slot_num]=1;
						CA_BP_CTRL_ADDR[address_num]=i2c_ctrl_addr[(slot_num*2)];
						CA_BP_WDT_ADDR[address_num]=i2c_ctrl_addr[(slot_num*2)+1];
						CA_BP_PERIOD_ADDR[address_num]=i2c_ctrl_addr[(slot_num*2)+1];
						address_num+=1;
					break;              
				default :
					break;
				}
			}
		}
	}
    return INTERFACE_NO;
}

int __init pca9555_init(void)
{
	if(string_init()==0){
		printk("CA bypass Can't fount devices\n");
		return -1;
	}
	ca_bypass_init();

#ifdef CA_BOARD_ADAPTER_TYPE
	printk("CA bypass driver %s-%s-BYPASS-%s loaded\n", CA_BOARD_TYPE, CA_BOARD_ADAPTER_TYPE, CA_BOARD_PATCH_VER_STR);
#else
	printk("CA bypass driver %s-BYPASS-%s loaded\n", CA_BOARD_TYPE, CA_BOARD_PATCH_VER_STR);
#endif  //CA_BOARD_ADAPTER_TYPE
	return i2c_add_driver(&ca_bypass_driver);
}

static void __exit pca9555_exit(void)
{
	i2c_del_driver(&ca_bypass_driver);
}

MODULE_AUTHOR("Benjamin Wang <shengbau@gmail.com>");
MODULE_DESCRIPTION("CA bypass driver");
MODULE_LICENSE("GPL");

module_init(pca9555_init);
module_exit(pca9555_exit);

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

extern char g_serial[25];
void xtm_abn_init(int port)
{
	static struct task_struct *bp_task;
    if(!strncmp(g_serial, "WXT102Z00BZH", 12))  		  		// XTM10000 OPTION
	port = (port==2)? 3 : 2;
    
    abn_set_mode(port, 0);
    //kernel_thread((void *)xtm_thread_bypass, NULL, CLONE_KERNEL);	/* future log agent */
	bp_task = kthread_run(xtm_thread_bypass, NULL, "bypassd");
	if (IS_ERR(bp_task)) {
		printk("bypassd start error. \n");
		return;
	}
}

void xtm_abn_enable(int port)
{
    if(!strncmp(g_serial, "WXT102Z00BZH", 12))  		  		// XTM10000 OPTION
	port = (port==2)? 3 : 2;
    
    abn_set_mode(port, 1);
}


void xtm_abn_disable(int port)
{
    if(!strncmp(g_serial, "WXT102Z00BZH", 12))  		  		// XTM10000 OPTION
	port = (port==2)? 3 : 2;
 
    abn_set_mode(port, 2);
}

void xtm_abn_wd_timeout(int port, int timeout)
{
    char tmp[12];
    if(!strncmp(g_serial, "WXT102Z00BZH", 12))  		  		// XTM10000 OPTION
	port = (port==2)? 3 : 2;

    sprintf(tmp,"%d", timeout); 
    ext_period_store(port, tmp, strlen(tmp)+1);
}


void xtm_abn_wd_reset(int port)
{
    if(!strncmp(g_serial, "WXT102Z00BZH", 12))  		  		// XTM10000 OPTION
	port = (port==2)? 3 : 2;
 
    ext_wdt_store(port, "1", 2);
}

