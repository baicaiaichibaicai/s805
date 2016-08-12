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
#include <linux/delay.h>
#include <linux/version.h>
#include "bypass_control.h"
#include "bypass_platform.h"

#ifdef DEF_FS_SYS_SUPPORT
#include <linux/hwmon-sysfs.h>
#else
#include <linux/i2c-proc.h>
#endif

#ifdef CONFIG_FUTURE
int is_portwell_bypass;
static int pw_eeprom_set(void);
#endif

#ifdef DEF_FS_SYS_SUPPORT
int pw_card_conf      = 0x221;
module_param(pw_card_conf, int, S_IRUGO);
static char *pw_board    = "NAR7090";
module_param(pw_board, charp, S_IRUGO);
#else
static int pw_card_conf      = 0;
module_param(pw_card_conf, int, S_IRUGO);
static char *pw_board    = "";
MODULE_PARM(pw_board, "s");
#endif

bp_interface bp_int[NET_INTERFACE_MAX];
static struct i2c_client eeprom_client;
#ifdef DEF_FS_SYS_SUPPORT
static unsigned short normal_i2c[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 
					0x26, 0x27,0x4c,0x4d, 0x4e, 0x4f ,I2C_CLIENT_END};

/* Insmod parameters */
I2C_CLIENT_INSMOD_1(pca9555);
#else
/* Since we don't have an lm_sensors driver ID yet, grab an experimental one */
#define I2C_DRIVERID_PCA9555 I2C_DRIVERID_EXP0

/* Oh boy, it has the same I2C Address Range as the PCF8574! */
static unsigned short normal_i2c[] = { SENSORS_I2C_END };
static unsigned short normal_i2c_range[] = { 0x24, 0x27, SENSORS_I2C_END };
static unsigned int normal_isa[] = { SENSORS_ISA_END };
static unsigned int normal_isa_range[] = { SENSORS_ISA_END };

/* Insmod parameters */
SENSORS_INSMOD_1(pca9555);
#endif//DEF_FS_SYS_SUPPORT

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) ) /*New style I2C driver*/
/* Return 0 if detection is successful, -ENODEV otherwise */
static int pca9555_detect(struct i2c_client *client, int kind,
                          struct i2c_board_info *info);
static int pca9555_probe(struct i2c_client *client, const struct i2c_device_id *id);

#else /*Old style I2C driver*/
static int pca9555_attach_adapter(struct i2c_adapter *adapter);
static int pca9555_detach_client(struct i2c_client *client);

#ifdef DEF_FS_SYS_SUPPORT
static int pca9555_detect(struct i2c_adapter *adapter, int address, int kind);
#else
static int pca9555_detect(struct i2c_adapter *adapter, int address,
                          unsigned short flags, int kind);
#endif//DEF_FS_SYS_SUPPORT

#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) ) */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )
static const struct i2c_device_id pca9555_id[] = {
        { "bypass", 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, pca9555_id);
static struct i2c_driver ca_bypass_driver = {
        .class = I2C_CLASS_HWMON,
        .driver = {
                .name   = "bypass",
        },
        .probe          = pca9555_probe,
        .id_table       = pca9555_id,
        .detect         = pca9555_detect,
        .address_data   = &addr_data,
};

#else

#ifdef DEF_FS_SYS_SUPPORT
/* This is the driver that will be inserted */
static struct i2c_driver ca_bypass_driver = {
	.driver = {
		.name	= "bypass",
	},
	.attach_adapter	= pca9555_attach_adapter,
	.detach_client	= pca9555_detach_client,
};
#else
/* This is the driver that will be inserted */
static struct i2c_driver ca_bypass_driver = {
//      .owner          = THIS_MODULE,
        .name           = "bypass",
        .id             = I2C_DRIVERID_PCA9555,
        .flags          = I2C_DF_NOTIFY,
        .attach_adapter = pca9555_attach_adapter,
        .detach_client  = pca9555_detach_client,
};
#endif//DEF_FS_SYS_SUPPORT

#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) ) */

static unsigned char number_of_bypass_device = 0;

u8 copper_cpld_eeprom_read_nb(u8 addr, u8 index)
{
	int nextboot;

	eeprom_client.addr = addr;
	//Check nextboot status(bit2, 3), nextboot=1 => non-normal, nextboot=0 => normal
	nextboot = i2c_smbus_read_byte(&eeprom_client);
	
	if (index == BP_SEGA) {
		if(nextboot & BIT2) {
			return 1;
  		} else {
 	 		return 0;
  		}
	} else if (index == BP_SEGB) {
		if(nextboot & BIT3) {
			return 1;
  		} else {
 	 		return 0;
  		}
	}else if (index == BP_SEGC) {
		if(nextboot & BIT4) {
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
		if (index == BP_SEGA) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT2));
		} else if (index == BP_SEGB) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT3));
		} else if (index == BP_SEGC) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT4));
		} else {
			ret = -1;
		}
		break;

	case SET_NB_NO_NORMAL:
		if (index == BP_SEGA) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT2);
		} else if (index == BP_SEGB) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT3);
		} else if (index == BP_SEGC) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT4);
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

	if (index == BP_SEGA) {
		if(bpe & BIT0) {
			return 1;
  		} else {
 	 		return 0;
  		}
	} else if (index == BP_SEGB) {
		if(bpe & BIT1) {
			return 1;
  		} else {
 	 		return 0;
  		}
	}else if (index == BP_SEGC) {
		if(bpe & BIT5) {
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
		if (index == BP_SEGA) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT0));
		} else if (index == BP_SEGB) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT1));
		}else if (index == BP_SEGC) {
			i2c_smbus_write_byte(&eeprom_client, tmpval & (~BIT5));
		} else {
			ret = -1;
		}
		break;

	case SET_BPE_BP:
		if (index == BP_SEGA) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT0);
		} else if (index == BP_SEGB) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT1);
		} else if (index == BP_SEGC) {
			i2c_smbus_write_byte(&eeprom_client, tmpval | BIT5);
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

int find_client_bp_seg(bp_seg **seg, struct i2c_client *client, int index)
{
	int i, j;

	//From interface 0~NET_INTERFACE_MAX-1, find the seg which matching the client->addr
	for (i=0; i<NET_INTERFACE_MAX; i++) {
		for(j=0; j<NET_INTERFACE_MAX_BP_SEGS; j++) {
			if (client->addr == bp_int[i].seg[j].ctrl_addr) {
				if (bp_int[i].seg[j+index].ctrl_addr == client->addr) {
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

int find_client_bp_interface(bp_interface **interface, struct i2c_client *client, int index)
{
	int i, j;

	//From interface 0~NET_INTERFACE_MAX-1, find the seg which matching the client->addr,return Interface Number
	for (i=0; i<NET_INTERFACE_MAX; i++) {
		for(j=0; j<NET_INTERFACE_MAX_BP_SEGS; j++) {
			if (client->addr == bp_int[i].seg[j].ctrl_addr) {
				if (bp_int[i].seg[j+index].ctrl_addr == client->addr) {
					*interface = &bp_int[i];
					return 0;
				} else {
					return -1;
				}
			}
			continue;
		}
	}
	return 1;
}

void bypass_reg_set_high_low(struct i2c_client *client, bp_cmd *bp_cmd, u8 reg_reposit, u8 delay)
{
	u8 bit_high,cmd, ret, tmpval = 0;

	bit_high = bp_cmd->bit_high;
	cmd = bp_cmd->cmd;

	ret = down_interruptible(bp_cmd->cmd_semaph);

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

	up(bp_cmd->cmd_semaph);

}

int client_is_bp_wdt(struct i2c_client *client, int interface) {
	int i;

	//search to find if the client addr is fiber bypass wdt ctrl
	for (i=0; i<NET_INTERFACE_MAX_BP_SEGS; i++) {
		if (client->addr == bp_int[interface].seg[i].wdt_addr)
			return 1;
		continue;
	}

	return 0;
}

int client_is_bp_eeprom(struct i2c_client *client) {
	int i,j;

	//search to find if the client addr is fiber bypass wdt ctrl
	for (i=0; i<NET_INTERFACE_MAX; i++) {
		for (j=0; j<NET_INTERFACE_MAX_BP_SEGS; j++) {
			if(bp_int[i].seg[j].type==BP_COPPER_CPLDV2)
				if (client->addr == bp_int[i].seg[j].eeprom_addr)
					return 1;
			continue;
		}
	}
	return 0;
}

int client_is_bp_ctrl(struct i2c_client *client, int interface) {
	int i;

	//search to find if the client addr is fiber bypass wdt ctrl
	for (i=0; i<NET_INTERFACE_MAX_BP_SEGS; i++) {
		if (client->addr == bp_int[interface].seg[i].ctrl_addr)
			return 1;
		continue;
	}

	return 0;
}
#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) ) /*Old style I2C driver*/
static int pca9555_attach_adapter(struct i2c_adapter *adapter)
{
#ifdef DEF_FS_SYS_SUPPORT
	return i2c_probe(adapter, &addr_data, pca9555_detect);
#else
	int ret;
	if ((ret = i2c_detect(adapter, &addr_data, pca9555_detect)) != 0)
		return ret;
	normal_i2c_range[0] = 0x20;
	normal_i2c_range[1] = 0x23;

	return i2c_detect(adapter, &addr_data, pca9555_detect);
#endif//DEF_FS_SYS_SUPPORT
}
#endif /* ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) )*/


int bypass_check_cpld_ver_ABN484(struct i2c_client *client, unsigned short addr)
/*Read CPLD version from ABN484L/ABN484 */
{
	struct i2c_client bp_client_to_read, *bp_client = &bp_client_to_read;
	int retval=0,icnt,buff=0;

	memcpy(bp_client, client, sizeof(struct i2c_client));
	if ( addr == 0 )
	    return 0xff;
	bp_client->addr = addr;

	/*Let WDT5 ( bit7 of OUTPORT0 ) be high. (we use WDT% as SCLK) */
	buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff | 0x80));

	/*FORCE WDT_DISJ_A to low level to make sure correct initial LFSR for CPLD rev*/
	buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff & 0xfd));
	
	/*LOADJ_A (bit0) LH edge to make sure proper initial LFSR value*/
	buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff & 0xfe));
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff | 0x01));

	/*Build SCLK(LH) and get Serial bit from INPORT1_bit6 ( BP_STS )*/
	for (icnt=7;icnt>=0;icnt--){
	    /*CLK LH Pulse*/
	    buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
	    i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff & 0x7f));
	    i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff | 0x80));
	    /*Read serial bit output from BY_STS*/
	    retval=(retval | (((i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_1 ) & 0x40)>>6)<<icnt));
	}

	/*LOADJ_A (bit0) LH edge to make sure proper initial LFSR value*/
	buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff & 0xfe));
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff | 0x01));

	/*FORCE WDT_DISJ_A to High  before returning from reading CPLD Rev. ID*/
	buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff | 0x02));

	return retval;

}

int bypass_check_cpld_ver_ABN482(struct i2c_client *client)
/*Read CPLD version from ABN482/ABN1000 */
{
	int retval=0,icnt;
	// wdt_disable_a pin pull low
	i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1,  i2c_smbus_read_byte_data(client , PCA9555_INPUT_1)&0xfb);
	// through WDT_5 pin make rise signal, Get WDT_STAT pin data confirm version number
	// Bit7 Bit6 Bit5 Bit4 Bit3 Bit2 Bit1 Bit0 Check0 Check1
	//    0      0      0     0      0      0      0     1          1            0
	// The CPLD version of 0x01
	for(icnt=9;icnt>=0;icnt--){
		i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_0, 0);
		i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_0, 32);
		retval|=(((i2c_smbus_read_byte_data(client,PCA9555_INPUT_1)&0x8)>>3)&0x1)<<icnt;
	}
	// wdt_disable_a pin pull high
	i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1,  i2c_smbus_read_byte_data(client , PCA9555_INPUT_1)|0x4);
	return retval>>2;
}

void bp_cmd_setup(bp_cmd *cmd_struct, u8 reg_out, u8 reg_in, u8 cmd, u8 bit_high);

int bypass_client_fill(struct i2c_client *client) {
	struct i2c_client bp_client;
	bp_seg *bp_seg = NULL;
	int err=-1,i,j;
	int CPLD_VER = CPLD_VER_INIT;

	memcpy(&bp_client, client, sizeof(struct i2c_client));

	for(i=0; i<NET_INTERFACE_MAX; i++) {
		if (bp_int[i].seg[0].type == BP_FIBER_PIC_ABN482) {
			if (client_is_bp_wdt(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, FIBER_PIC_ABN482_WDT_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, FIBER_PIC_ABN482_WDT_DR1_DF);
				i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, FIBER_PIC_ABN482_WDT_OUT1_DF); 
				/*Check CPLD version, if are old CPLD version expect the number is 0xff*/
				CPLD_VER=bypass_check_cpld_ver_ABN482(client); 
					/* The new CPLD disable command is modify H_L_H signal */
				if(CPLD_VER!=0xff){
					bp_cmd_setup(&(bp_int[i].seg[0].wdt_disable), PCA9555_OUTPUT_1, 0,FIBER_ABN482_DISABLE_WDT, FIBER_ABN482_DISABLE_WDT|FIBER_ABN482_WDT_DIS_BIT);
					printk("Device I2C addr:%x ,Firmware version:%x\n",client->addr,CPLD_VER);
				}else{
					printk("Device I2C addr:%x ,Firmware version: N/A\n",client->addr);
				}
			} else if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, FIBER_PIC_ABN482_CTRL_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, FIBER_PIC_ABN482_CTRL_DR1_DF);

				if ((err = bypass_setup_fs(client, BYPASS0ATTR)))
					goto exit_free;

				for (j=0; j<2; j++) {
					find_client_bp_seg(&bp_seg, client, j);
					if (bp_seg->wdt_addr)
						bp_client.addr = bp_seg->wdt_addr;
					//Forcing do WDT clear at setup time
					bypass_reg_set_high_low(&bp_client, &bp_seg->wdt_clear, bp_seg->reg_reposit, bp_seg->delay_ms);
				}
			}
#ifdef MIPS_CAVIUM_TARGET
		} else if (bp_int[i].seg[0].type == BP_FIBER_PIC_ABN1004) {
			if (client_is_bp_wdt(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, FIBER_PIC_ABN1004_WDT_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, FIBER_PIC_ABN1004_WDT_DR1_DF);
				i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, FIBER_PIC_ABN1004_WDT_OUT1_DF); 
				/*Check CPLD version, if are old CPLD version expect the number is 0xff*/
				CPLD_VER=bypass_check_cpld_ver_ABN482(client); 
					/* The new CPLD disable command is modify H_L_H signal */
				if(CPLD_VER!=0xff){
					bp_cmd_setup(&(bp_int[i].seg[0].wdt_disable), PCA9555_OUTPUT_1, 0,FIBER_ABN1004_DISABLE_WDT, FIBER_ABN1004_DISABLE_WDT|FIBER_ABN1004_WDT_DIS_BIT);
					printk("Device I2C addr:%x ,Firmware version:%x\n",client->addr,CPLD_VER);
				}else{
					printk("Device I2C addr:%x ,Firmware version: N/A\n",client->addr);
				}
			} else if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, FIBER_PIC_ABN1004_CTRL_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, FIBER_PIC_ABN1004_CTRL_DR1_DF);

				if ((err = bypass_setup_fs(client, BYPASS0ATTR)))
					goto exit_free;
				if ((err = bypass_setup_fs(client, BYPASS1ATTR)))
					goto exit_free;

				for (j=0; j<2; j++) {
					find_client_bp_seg(&bp_seg, client, j);
					if (bp_seg->wdt_addr)
						bp_client.addr = bp_seg->wdt_addr;
					//Forcing do WDT clear at setup time
					bypass_reg_set_high_low(&bp_client, &bp_seg->wdt_clear, bp_seg->reg_reposit, bp_seg->delay_ms);
				}
			}
#endif //X86_TARGET
		} else if (bp_int[i].seg[0].type == BP_COPPER_PIC) {
			if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, COPPER_PIC_CTRL_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, COPPER_PIC_CTRL_DR1_DF);
				if ((err = bypass_setup_fs(client, BYPASS0ATTR)))
					goto exit_free;
				if ((err = bypass_setup_fs(client, BYPASS1ATTR)))
					goto exit_free;
			}
		} else if (bp_int[i].seg[0].type == BP_COPPER_CPLDV2) {
			if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, COPPER_CPLD_V2P0_CTRL_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, COPPER_CPLD_V2P0_CTRL_DR1_DF);
				/*Usage get BPE from PCA9559 for synchronization of 9555_BPE and 9559_BPE*/
				i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_0, COPPER_CPLD_V2P0_CTRL_OUT0_DF);
				find_client_bp_seg(&bp_seg, client,0);

#ifdef X86_TARGET
				//In ABN484L, we need to set the pca9559 A0, A1 address pin by secondary pca9555(0x24~0x26)
				if ((client->addr>=0x24)&&(bp_int[i].seg[0].name==BP_CARD_ABN484L)) {
					//sync the pca9559 (A0, A1) to 2th pca9555 (A0, A1) by pca9555 (IO06, IO07)
					i2c_smbus_write_byte_data(client,PCA9555_OUTPUT_0,(0x3F|((client->addr&0x03)<<0x06)));
				}
#endif//X86_TARGET

				// Check BPE_9559 , BPE_9559=0, then BPE_9555=0(0x2f)
				if(copper_cpld_eeprom_read_bpe(bp_seg->eeprom_addr,bp_seg->index)==0)
                                            i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, COPPER_CPLD_V2P0_CTRL_OUT1_DF);
                                    else // otherwise, BPE_9555=1(0x3f)
                                            i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, (COPPER_CPLD_V2P0_CTRL_OUT1_DF|0x10));

				if ((err = bypass_setup_fs(client, BYPASS0ATTR)))
					goto exit_free;
				/*Check CPLD version, if are old CPLD version expect the number is 0xff*/
				
				CPLD_VER=bypass_check_cpld_ver_ABN484(client, bp_int[i].seg[0].firmare_ver_addr);
					/* The new CPLD disable command is modify H_L_H signal */
				if(CPLD_VER!=0xff && CPLD_VER!=0x00){
					printk("Device I2C addr:%x ,Firmware version:%x\n",client->addr,CPLD_VER);
				}else{
					printk("Device I2C addr:%x ,Firmware version: N/A\n",client->addr);
				}
			}
		}
	}
	return 0;
exit_free:
	return err;
}

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) ) /*New style I2C driver*/
/* Return 0 if detection is successful, -ENODEV otherwise */
static int pca9555_detect(struct i2c_client *client, int kind,
                          struct i2c_board_info *info)
#else /*Old style I2C driver*/

/* This function is called by i2c_probe */
#ifdef DEF_FS_SYS_SUPPORT
static int pca9555_detect(struct i2c_adapter *adapter, int address, int kind)
#else
/* This function is called by i2c_detect */
int pca9555_detect(struct i2c_adapter *adapter, int address,
			unsigned short flags, int kind)
#endif//DEF_FS_SYS_SUPPORT
#endif /*( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )*/
{
	int err = 0;        
	struct i2c_client *new_client;
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) ) /*New style I2C driver*/
       struct i2c_adapter *adapter = client->adapter;
	new_client = client;
        if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
                goto exit;
#else /*Old style I2C driver*/

	struct ca_bypass_data *data;
#ifdef DEF_FS_SYS_SUPPORT
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

#else
	/* Make sure we aren't probing the ISA bus!! This is just a safety check
	   at this moment; i2c_detect really won't call us. */
#ifdef CONFIG_CA_BOARD_DEBUG
	if (i2c_is_isa_adapter(adapter)) {
		printk("pca9555.o: pca9555_detect called for an ISA bus adapter?!?\n");
		return 0;
	}
#endif //CONFIG_CA_BOARD_DEBUG

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE))
		goto exit;
	/* OK. For now, we presume we have a valid client. We now create the
	   client structure, even though we cannot fill it completely yet. */

	if (!(new_client = kmalloc(sizeof(struct i2c_client) + sizeof(struct ca_bypass_data),GFP_KERNEL))){
		err = -ENOMEM;
		goto exit;
	}
	data = (struct ca_bypass_data *) (new_client + 1);
	new_client->addr = address;
	new_client->data = data;
	new_client->adapter = adapter;

#endif//DEF_FS_SYS_SUPPORT
#endif /*( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) )*/

	if(eeprom_client.addr==0)
		memcpy(&eeprom_client, new_client, sizeof(struct i2c_client));
	if(client_is_bp_eeprom(new_client)) {
		memcpy(&eeprom_client, new_client, sizeof(struct i2c_client));
		return 0;
	} else {
		new_client->driver = &ca_bypass_driver;
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )
		strlcpy(info->type, "bypass", I2C_NAME_SIZE);
#else
		new_client->flags = 0;
#ifdef DEF_FS_SYS_SUPPORT
		if (kind < 0) {
			/* Detection: the pca9555 only has 8 registers (0-7).
			   A read of 7 should succeed, but a read of 8 should fail. */
			if ((i2c_smbus_read_byte_data(new_client, 7) < 0) ||
			    (i2c_smbus_read_byte_data(new_client, 8) >= 0))
				goto exit_kfree;
		}
		strlcpy(new_client->name, "bypass", I2C_NAME_SIZE);
#else
		/* Fill in the remaining client fields and put it into the global list */
		strcpy(new_client->name, "PCA9555 chip");
#endif//DEF_FS_SYS_SUPPORT

		/* Tell the I2C layer a new client has arrived */
		if ((err = i2c_attach_client(new_client)))
			goto exit_kfree;

#ifdef CONFIG_CA_BOARD_DEBUG	
		if((err = bypass_setup_fs(new_client, DEFATTR)))
			goto exit_detach;     
#endif//CONFIG_CA_BOARD_DEBUG
		if((err = bypass_client_fill(new_client)))
                        goto exit_detach;

#endif /* ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) ) */
		number_of_bypass_device++;
		return 0;
	}

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) )
exit_detach:
	i2c_detach_client(new_client);

exit_kfree:
	kfree(data);
#endif /*( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) )*/
exit:
#ifdef CONFIG_FUTURE
	if(err == 0)
	    is_portwell_bypass = 1;
#endif
	return err;
}

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) ) /*New I2C driver*/
static int pca9555_probe(struct i2c_client *client,
                        const struct i2c_device_id *id)
{
#ifdef CONFIG_CA_BOARD_DEBUG    
         bypass_setup_fs(client, DEFATTR);
#endif
        return bypass_client_fill(client);
}
#endif /*( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )*/

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) )
static int pca9555_detach_client(struct i2c_client *client)
{
	int err;
	bypass_unsetup_fs(client);
	if ((err = i2c_detach_client(client)))
		return err;
#ifdef DEF_FS_SYS_SUPPORT
	kfree(i2c_get_clientdata(client));
#else
	kfree(client);
#endif//DEF_FS_SYS_SUPPORT
	return 0;
}
#endif /*( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27) )*/

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
			COPPER_CPLD_V2P0_SET_NORMAL_L, COPPER_CPLD_V2P0_SET_NORMAL_H);

	bp_cmd_setup(&(seg_struct->set_nonormal), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_SET_NONORMAL_L, COPPER_CPLD_V2P0_SET_NONORMAL_H);

	bp_cmd_setup(&(seg_struct->set_bpe_open), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_SET_BPE_OPEN, 0);

	bp_cmd_setup(&(seg_struct->set_bpe_bypass), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_SET_BPE_BP, 0);

	bp_cmd_setup(&(seg_struct->get_bypass), 0, PCA9555_INPUT_1, 0, 0);

	bp_cmd_setup(&(seg_struct->get_nb), 0, PCA9555_INPUT_1, 0, 0);

	bp_cmd_setup(&(seg_struct->get_wdt), 0, PCA9555_INPUT_1, 0, 0);

	bp_cmd_setup(&(seg_struct->wdt_disable), PCA9555_OUTPUT_0, PCA9555_INPUT_0,
			COPPER_CPLD_V2P0_DISABLE_WDT_L, COPPER_CPLD_V2P0_DISABLE_WDT_H);

	bp_cmd_setup(&(seg_struct->wdt_refresh), PCA9555_OUTPUT_0, PCA9555_INPUT_0,
			COPPER_CPLD_V2P0_REFRESH_BP_WDT_L, COPPER_CPLD_V2P0_REFRESH_BP_WDT_H);

	bp_cmd_setup(&(seg_struct->wdt_clear), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
			COPPER_CPLD_V2P0_CLR_BP_WDT_L, COPPER_CPLD_V2P0_CLR_BP_WDT_H);

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

	bp_cmd_setup(&(seg_struct->set_bpe_bypass), reg_out, 0, COPPER_PIC_SET_BPE_BP,
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

void fiber_pic_ABN1004_bp_seg_set(bp_seg *seg_struct, u8 index)
{
	u8 cmd_bit_high = FIBER_ABN1004_SET_CMD_BITS_HIGH;

	bp_cmd_setup(&(seg_struct->set_normal), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_ABN1004_SET_NORMAL(index), FIBER_ABN1004_SET_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nonormal), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_ABN1004_SET_NONORMAL(index), FIBER_ABN1004_SET_NONORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_bpe_open), PCA9555_OUTPUT_0, 0,
		FIBER_ABN1004_SET_OPEN(index), FIBER_ABN1004_SET_OPEN(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_bpe_bypass), PCA9555_OUTPUT_0, 0,
		FIBER_ABN1004_SET_BP(index), FIBER_ABN1004_SET_BP(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nb_normal), PCA9555_OUTPUT_0, 0,
		FIBER_ABN1004_SET_NB_NORMAL(index), FIBER_ABN1004_SET_NB_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nb_nonormal), PCA9555_OUTPUT_0, 0,
		FIBER_ABN1004_SET_NB_NO_NORMAL(index), FIBER_ABN1004_SET_NB_NO_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_bypass), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_ABN1004_GET_MODE_STS, FIBER_ABN1004_GET_MODE_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_nb), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_ABN1004_GET_NB_BPE_STS, FIBER_ABN1004_GET_NB_BPE_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_wdt), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
		FIBER_ABN1004_GET_BP_WDT_STS, FIBER_ABN1004_GET_BP_WDT_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->wdt_disable), PCA9555_OUTPUT_1, 0,
		FIBER_ABN1004_DISABLE_WDT, 0);

	bp_cmd_setup(&(seg_struct->wdt_refresh), PCA9555_OUTPUT_1, 0,
		FIBER_ABN1004_REFRESH_BP_WDT, FIBER_ABN1004_REFRESH_BP_WDT|FIBER_ABN1004_WDT_LOAD_BIT);

	bp_cmd_setup(&(seg_struct->wdt_clear), PCA9555_OUTPUT_1, 0,
		FIBER_ABN1004_CLR_BP_WDT, FIBER_ABN1004_CLR_BP_WDT|FIBER_ABN1004_WDT_CLEAR_BIT);

	bp_cmd_setup(&(seg_struct->set_period), PCA9555_OUTPUT_0, PCA9555_INPUT_0, 0, 0);
}
void fiber_pic_ABN482_bp_seg_set(bp_seg *seg_struct, u8 index)
{
	u8 cmd_bit_high = FIBER_ABN482_SET_CMD_BITS_HIGH;

	bp_cmd_setup(&(seg_struct->set_normal), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_ABN482_SET_NORMAL(index), FIBER_ABN482_SET_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nonormal), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_ABN482_SET_NONORMAL(index), FIBER_ABN482_SET_NONORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_bpe_open), PCA9555_OUTPUT_0, 0,
		FIBER_ABN482_SET_OPEN(index), FIBER_ABN482_SET_OPEN(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_bpe_bypass), PCA9555_OUTPUT_0, 0,
		FIBER_ABN482_SET_BP(index), FIBER_ABN482_SET_BP(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nb_normal), PCA9555_OUTPUT_0, 0,
		FIBER_ABN482_SET_NB_NORMAL(index), FIBER_ABN482_SET_NB_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->set_nb_nonormal), PCA9555_OUTPUT_0, 0,
		FIBER_ABN482_SET_NB_NO_NORMAL(index), FIBER_ABN482_SET_NB_NO_NORMAL(index)|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_bypass), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_ABN482_GET_MODE_STS, FIBER_ABN482_GET_MODE_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_nb), PCA9555_OUTPUT_0, PCA9555_INPUT_1,
		FIBER_ABN482_GET_NB_BPE_STS, FIBER_ABN482_GET_NB_BPE_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->get_wdt), PCA9555_OUTPUT_1, PCA9555_INPUT_1,
		FIBER_ABN482_GET_BP_WDT_STS, FIBER_ABN482_GET_BP_WDT_STS|cmd_bit_high);

	bp_cmd_setup(&(seg_struct->wdt_disable), PCA9555_OUTPUT_1, 0,
		FIBER_ABN482_DISABLE_WDT, 0);

	bp_cmd_setup(&(seg_struct->wdt_refresh), PCA9555_OUTPUT_1, 0,
		FIBER_ABN482_REFRESH_BP_WDT, FIBER_ABN482_REFRESH_BP_WDT|FIBER_ABN482_WDT_LOAD_BIT);

	bp_cmd_setup(&(seg_struct->wdt_clear), PCA9555_OUTPUT_1, 0,
		FIBER_ABN482_CLR_BP_WDT, FIBER_ABN482_CLR_BP_WDT|FIBER_ABN482_WDT_CLEAR_BIT);

	bp_cmd_setup(&(seg_struct->set_period), PCA9555_OUTPUT_0, PCA9555_INPUT_0, 0, 0);
}

int find_bypass_mb_index(unsigned char* board_conf)
{
	int index;

	printk("board_conf:%s\n", board_conf);
	for(index=0; index<BP_MB_MAX; index++) {
		if(strcmp(board_conf, BP_MB_NAME[index])==0) {
			//return the pw_board model index
			return index;
		} 
	}
	return -1;
}

/**
 * ca_bypass_modeling: 
 *
 * According to the platform config and user input parameter, fill the global bypass structure
 */
int ca_bypass_modeling(void)
{
	/* Series of tmp array to contain the bypass info by user input parameter and configuration table in header file */
	unsigned char ca_bypass_ctrl_addr[NET_INTERFACE_MAX*NET_INTERFACE_MAX_BP_SEGS];
	unsigned char ca_bypass_wdt_addr[NET_INTERFACE_MAX*NET_INTERFACE_MAX_BP_SEGS];
	unsigned char ca_bypass_period_addr[NET_INTERFACE_MAX*NET_INTERFACE_MAX_BP_SEGS];
	unsigned char ca_bypass_eeprom_addr[NET_INTERFACE_MAX*NET_INTERFACE_MAX_BP_SEGS];
	unsigned char ca_bypass_seg_no[NET_INTERFACE_MAX];
	unsigned char ca_bypass_type[NET_INTERFACE_MAX];
	unsigned char ca_bypass_gen[NET_INTERFACE_MAX];
	unsigned char ca_bypass_name[NET_INTERFACE_MAX];
	unsigned char ca_bypass_firmare_ver_addr[NET_INTERFACE_MAX];
	unsigned char *ca_bypass_modelname[NET_INTERFACE_MAX];
	int interface=0, seg_total=0, seg_index=0, cur_seg;
	int mb_index, card_index=0, slot_num=0, slot_conf;
	int no, pos, i;

	memset(ca_bypass_ctrl_addr,0,sizeof(ca_bypass_ctrl_addr));
	memset(ca_bypass_wdt_addr,0,sizeof(ca_bypass_wdt_addr));
	memset(ca_bypass_period_addr,0,sizeof(ca_bypass_period_addr));
	memset(ca_bypass_eeprom_addr,0,sizeof(ca_bypass_eeprom_addr));
	memset(ca_bypass_seg_no,0,sizeof(ca_bypass_seg_no));
	memset(ca_bypass_type,0,sizeof(ca_bypass_type));
	memset(ca_bypass_gen,0,sizeof(ca_bypass_gen));
	memset(ca_bypass_name,0,sizeof(ca_bypass_name));
	memset(ca_bypass_modelname,0,sizeof(ca_bypass_modelname));
        
	/* Search the bypass model index */
	if ((mb_index = find_bypass_mb_index(pw_board)) == (-1)) {
		CA_BOARD_DEBUG("Board parameter may be invalid or has no on board bypass module");
		return -1;
	} else {
	    printk("interface:%d mb_index:%x\n", interface, mb_index);
		/* Do motherboard information configuring */
		ca_bypass_type[interface] = mb_bypass_info[mb_index].type;
		ca_bypass_gen[interface] = mb_bypass_info[mb_index].generation;
		ca_bypass_name[interface] = mb_bypass_info[mb_index].name;
		ca_bypass_modelname[interface] = BP_MB_NAME[mb_index];
		ca_bypass_seg_no[interface] = mb_bypass_info[mb_index].segs;
		for(seg_index=0; seg_index<mb_bypass_info[mb_index].segs; seg_index++){
			cur_seg = seg_total+seg_index;
			ca_bypass_ctrl_addr[cur_seg] = mb_bypass_info[mb_index].ctrl_addr[seg_index];
			ca_bypass_eeprom_addr[cur_seg] = mb_bypass_info[mb_index].eeprom_addr[seg_index];
			ca_bypass_wdt_addr[cur_seg] = mb_bypass_info[mb_index].wdt_addr[seg_index];
			ca_bypass_period_addr[cur_seg] = mb_bypass_info[mb_index].period_addr[seg_index];
		}
		seg_total += mb_bypass_info[mb_index].segs;
		interface++;
	}

	/* Do add-card information configuring */
	if(pw_card_conf) {
		/* Config slot information according to input pw_card_conf */
		for(slot_num=0; slot_num<ADDCARD_MAX_SLOTS; slot_num++) {
			/* Check if current slot is configured, only 4-bits for one slot */
			/* Since the input ex. conf=0x201, 2 means slot0 and 1 for slot2, we need to reorder pw_card_conf */
			slot_conf = (pw_card_conf>>(((ADDCARD_MAX_SLOTS-1)-slot_num)*4)) & 0xf;
			/* Check if this is a meaningful conf */
			if (slot_conf > BP_CARD_MAX) {
				CA_BOARD_DEBUG("Err! Bad pw_card_conf parameter");
				return -1;
			}
			/* Current slot config is 0, skip this */
			if (!slot_conf)
				continue;
			card_index = slot_conf-1;

			ca_bypass_type[interface] = addcard_bypass_info[slot_num][card_index].type;
			ca_bypass_gen[interface] = addcard_bypass_info[slot_num][card_index].generation;
			ca_bypass_name[interface] = addcard_bypass_info[slot_num][card_index].name;
			ca_bypass_modelname[interface] = BP_CARD_NAME[card_index];
			ca_bypass_seg_no[interface] = addcard_bypass_info[slot_num][card_index].segs;
			ca_bypass_firmare_ver_addr[interface] = addcard_bypass_info[slot_num][card_index].firmare_ver_addr;
			for(seg_index=0; seg_index<addcard_bypass_info[slot_num][card_index].segs; seg_index++) {
				cur_seg = seg_total+seg_index;
				ca_bypass_ctrl_addr[cur_seg] = addcard_bypass_info[slot_num][card_index].ctrl_addr[seg_index];
				ca_bypass_eeprom_addr[cur_seg] = addcard_bypass_info[slot_num][card_index].eeprom_addr[seg_index];
				ca_bypass_wdt_addr[cur_seg] = addcard_bypass_info[slot_num][card_index].wdt_addr[seg_index];
				ca_bypass_period_addr[cur_seg] = addcard_bypass_info[slot_num][card_index].period_addr[seg_index];
			}
			seg_total += addcard_bypass_info[slot_num][card_index].segs;
			interface++;
		}
	}

	/* Fill each interface bypass data into bp_int[] which handle the each bypass seg action */
	for(interface=0; interface<NET_INTERFACE_MAX; interface++) {
		bp_int[interface].seg_no = ca_bypass_seg_no[interface];
		bp_int[interface].modelname= ca_bypass_modelname[interface];
		for(no=0; no<ca_bypass_seg_no[interface]; no++) {
			//To find the exaclty position
			pos = 0;
			for(i=0; i<interface; i++)
				pos += ca_bypass_seg_no[i];
			pos += no;
 
			bp_int[interface].seg[no].type = ca_bypass_type[interface];
			bp_int[interface].seg[no].name= ca_bypass_name[interface];
			bp_int[interface].seg[no].generation=ca_bypass_gen[interface];
			bp_int[interface].seg[no].firmare_ver_addr = ca_bypass_firmare_ver_addr[interface];
			bp_int[interface].seg[no].index = (no%3);
			bp_int[interface].seg[no].ctrl_addr = ca_bypass_ctrl_addr[pos];
			bp_int[interface].seg[no].wdt_addr = ca_bypass_wdt_addr[pos];
			bp_int[interface].seg[no].period_addr = ca_bypass_period_addr[pos];
			bp_int[interface].seg[no].eeprom_addr = ca_bypass_eeprom_addr[pos];
			init_MUTEX (&bp_int[interface].seg[no].bp_semaph);
			bp_int[interface].seg[no].set_normal.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].set_nonormal.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].set_bpe_open.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].set_bpe_bypass.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].set_nb_normal.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].set_nb_nonormal.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].wdt_disable.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].wdt_refresh.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].wdt_clear.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].set_period.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].get_bypass.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
		        bp_int[interface].seg[no].get_nb.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;
			bp_int[interface].seg[no].get_wdt.cmd_semaph = &bp_int[interface].seg[no].bp_semaph;


			if (ca_bypass_type[interface] == BP_COPPER_PIC) {
				bp_int[interface].seg[no].delay_ms = COPPER_PIC_MICROCON_DELAY;
				bp_int[interface].seg[no].bp_sts_bit0 = COPPER_PIC_STS_BIT0;
				bp_int[interface].seg[no].bp_sts_bit1 = COPPER_PIC_STS_BIT1;
				bp_int[interface].seg[no].wdt_en_bit = COPPER_PIC_WDT_STS_BIT;
				bp_int[interface].seg[no].wdt_tmout_bit = COPPER_PIC_WDT_TIMEOUT_BIT;
				bp_int[interface].seg[no].reg_reposit = 0;
				copper_pic_bp_seg_set(&bp_int[interface].seg[no], bp_int[interface].seg[no].index);
			} else if (ca_bypass_type[interface] == BP_COPPER_CPLDV2) {
				bp_int[interface].seg[no].delay_ms = COPPER_CPLD_V2P0_MICROCON_DELAY;
				bp_int[interface].seg[no].bp_sts_bit0 = COPPER_CPLD_V2P0_STS_BIT0;
				bp_int[interface].seg[no].bp_sts_bit1 = COPPER_CPLD_V2P0_STS_BIT1;
				bp_int[interface].seg[no].wdt_en_bit = COPPER_CPLD_V2P0_WDT_STS_BIT;
				bp_int[interface].seg[no].wdt_tmout_bit = COPPER_CPLD_V2P0_WDT_TIMEOUT_BIT;
				bp_int[interface].seg[no].reg_reposit = 1;
				copper_cpld_v2p0_bp_seg_set(&bp_int[interface].seg[no], bp_int[interface].seg[no].index);
			} else if (ca_bypass_type[interface] == BP_FIBER_PIC_ABN482) {
				bp_int[interface].seg[no].delay_ms = FIBER_ABN482_MICROCON_DELAY;
				bp_int[interface].seg[no].bp_sts_bit0 = FIBER_ABN482_SEGB_STS_BIT0;
				bp_int[interface].seg[no].bp_sts_bit1 = FIBER_ABN482_SEGB_STS_BIT1;
				bp_int[interface].seg[no].wdt_en_bit = FIBER_ABN482_WDT_BP_STS_BIT;
				bp_int[interface].seg[no].wdt_tmout_bit = FIBER_ABN482_WDT_TIMEOUT_BIT;
				bp_int[interface].seg[no].reg_reposit = 0;
				fiber_pic_ABN482_bp_seg_set(&bp_int[interface].seg[no], bp_int[interface].seg[no].index);

			} else if (ca_bypass_type[interface] == BP_FIBER_PIC_ABN1004) {
				bp_int[interface].seg[no].delay_ms = FIBER_ABN1004_MICROCON_DELAY;
				//index = 0 means it's seg a
				if (bp_int[interface].seg[no].index) {
					bp_int[interface].seg[no].bp_sts_bit0 = FIBER_ABN1004_SEGB_STS_BIT0;
					bp_int[interface].seg[no].bp_sts_bit1 = FIBER_ABN1004_SEGB_STS_BIT1;
				} else {
					bp_int[interface].seg[no].bp_sts_bit0 = FIBER_ABN1004_SEGA_STS_BIT0;
					bp_int[interface].seg[no].bp_sts_bit1 = FIBER_ABN1004_SEGA_STS_BIT1;
				}
				bp_int[interface].seg[no].wdt_en_bit = FIBER_ABN1004_WDT_BP_STS_BIT;
				bp_int[interface].seg[no].wdt_tmout_bit = FIBER_ABN1004_WDT_TIMEOUT_BIT;
				bp_int[interface].seg[no].reg_reposit = 0;
				fiber_pic_ABN1004_bp_seg_set(&bp_int[interface].seg[no], bp_int[interface].seg[no].index);
			}
			printk("bp%d:%d ctrl=0x%x, wdt=0x%x, period=0x%x, eeprom=0x%x\n", interface,
				no, bp_int[interface].seg[no].ctrl_addr, bp_int[interface].seg[no].wdt_addr,
				bp_int[interface].seg[no].period_addr, bp_int[interface].seg[no].eeprom_addr);
		}
	}
	return 0;
}

static int __init pca9555_init(void)
{
	int ret;

#ifdef CONFIG_FUTURE
	pw_eeprom_set();
#endif
	
	if(ca_bypass_modeling()){
		printk("Setup and fill the bypass model data failed!\n");
		return -1;
	}

	printk("CA network-bypass driver %s loaded\n", CA_BOARD_PATCH_VER_STR);

	ret = i2c_add_driver(&ca_bypass_driver);
	if (!number_of_bypass_device){
	    printk("Loading CA network-bypass driver failed. No device was found !!\n");
	    i2c_del_driver(&ca_bypass_driver);
	    return -ENODEV;
	}
	return ret;
}

static void __exit pca9555_exit(void)
{
	i2c_del_driver(&ca_bypass_driver);
}

#ifdef CONFIG_FUTURE
#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/pci.h>

extern char *strtok(char *s, const char *ct);
unsigned short nar7090_slot_id[3][2] = {{0x25e4, 0x25e5}, {0x25e6, 0x25e7}, {0x3510, 0x3514}};
unsigned short nar7100_slot_id[3][2] = {{0x340a, 0x340b}, {0x340c, 0x340d}, {0x340e, 0x340f}};


static void get_eeprom(struct net_device *netdev, int offset, char *val, int len)
{
    struct ethtool_eeprom eeprom;
    
    eeprom.len = len;
    eeprom.offset = offset;
    netdev->ethtool_ops->get_eeprom(netdev, &eeprom, val);
}

static void set_eeprom(struct net_device *netdev, int offset, int val, int len)
{
    struct ethtool_eeprom eeprom;
    char val_ptr[4];
   
    if(len > sizeof(int))
	return;

    eeprom.len = len;
    eeprom.offset = offset;
    netdev->ethtool_ops->get_eeprom(netdev, &eeprom, val_ptr);
    memcpy(val_ptr, &val, len);
    netdev->ethtool_ops->set_eeprom(netdev, &eeprom, val_ptr);
}

static void get_pciinfo(struct net_device *netdev, char *info)
{
    struct ethtool_drvinfo drvinfo;
    
    netdev->ethtool_ops->get_drvinfo(netdev, &drvinfo);
    memcpy(info, drvinfo.bus_info, ETHTOOL_BUSINFO_LEN);
}


static int get_slot(struct net_device *netdev, unsigned short slot_id[3][2])
{
    struct pci_dev *pcidev = NULL;
    char info[ETHTOOL_BUSINFO_LEN];
    char *tmp;
    int slot, bus;
    unsigned char val;

    get_pciinfo(netdev, info);

    tmp = strtok(info, ":");
    tmp = strtok(NULL, ":");
    bus = simple_strtoul(tmp, NULL, 16);

    while((pcidev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, pcidev))){
	for(slot=0; slot<3; slot++){
	    if((pcidev->device==slot_id[slot][0]) || (pcidev->device==slot_id[slot][1])){
		pci_read_config_byte(pcidev, 0x19, &val);
		if(bus == val)
		    return slot;
		break;
	    }
	}
    }
    return -1;
}

int get_model(void)
{
    struct pci_dev *pcidev = NULL;

    pcidev = pci_get_device(0x8086, nar7100_slot_id[0][0], pcidev);
    if(pcidev)
	return BP_MB_NAR7100;
    
    pcidev = pci_get_device(0x8086, nar7090_slot_id[0][0], pcidev);
    if(pcidev)
    	return BP_MB_NAR7090;

    return -1;
}


static int pw_eeprom_set(void)
{
	int n, slot, model;
	unsigned short w10;
	unsigned char w12, w20, w40;
	extern struct net_device *netdev_global[NR_ETH];
	
	pw_card_conf =0;

	model = get_model();
	if(model < 0)
	    return 0;

	pw_board = BP_MB_NAME[model];

	for(n=0; n<NR_ETH; n++){
	    struct net_device *dev;
	    
	    dev = netdev_global[n];
	    if(!dev)
		continue;
	   
	  
	    if(model == BP_MB_NAR7090)
		slot = get_slot(dev, nar7090_slot_id);
	    else if(model == BP_MB_NAR7100){
		slot = get_slot(dev, nar7100_slot_id);
	    }
	    else
		continue;

	    if(slot < 0)
		continue;

	    get_eeprom(dev, 0x10, (unsigned char *)&w10, 2);
	    get_eeprom(dev, 0x12, &w12, 1);
	    get_eeprom(dev, 0x20, &w20, 1);
	    get_eeprom(dev, 0x40, &w40, 1);
	    printk("%s %x %x %x %x slot:%d\n", dev->name, w10, w12, w20, w40, slot);
	  
	    if(w10 == 0x4840){
		pw_card_conf |= (0x01<<(2-slot)*4);
		if(w12 == 0x10){
		    if(w20 != (0x4c+slot)) set_eeprom(dev, 0x20, 0x4c+slot, 1);
		    if(w40 != (0x0c+slot)) set_eeprom(dev, 0x40, 0x0c+slot, 1);
		}
		else if(w12 == 0x11){
		    if(w40 != (0x0c+slot)) set_eeprom(dev, 0x40, 0x0c+slot, 1);
		}
	    }
	    else if(w10 == 0x4820){
		pw_card_conf |= (0x02<<(2-slot)*4);
		if(slot == 0){
		    if(w20 != 0x80) set_eeprom(dev, 0x20, 0x80, 1);
		    if(w40 != 0x00) set_eeprom(dev, 0x40, 0x00, 1);
		}
		else if(slot == 1){
		    if(w20 != 0x81) set_eeprom(dev, 0x20, 0x81, 1);
		    if(w40 != 0x01) set_eeprom(dev, 0x40, 0x01, 1);
		}
		else if(slot == 2){
		    if(w20 != 0xc0) set_eeprom(dev, 0x20, 0xc0, 1);
		    if(w40 != 0x40) set_eeprom(dev, 0x40, 0x40, 1);
		}
	    }
	    else if(w10 == 0x5212){
		pw_card_conf |= (0x02<<(2-slot)*4);
	    }
	    else if(w10 == 0x01a9){
		pw_card_conf |= (0x02<<(2-slot)*4);
	    }
	}
	printk("card_cnf:%x\n", pw_card_conf);
	return 0;
}



#endif


MODULE_AUTHOR("Benjamin Wang <shengbau@cas-well.com>");
MODULE_DESCRIPTION("CA bypass driver");
MODULE_LICENSE("GPL");

#ifdef CONFIG_FUTURE
late_initcall(pca9555_init);
#else
module_init(pca9555_init);
#endif
module_exit(pca9555_exit);


