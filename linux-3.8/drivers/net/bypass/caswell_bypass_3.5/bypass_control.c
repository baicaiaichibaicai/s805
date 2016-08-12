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
#include <linux/pci.h>
#include <linux/miscdevice.h>
#include "bypass_control.h"
#include "bypass_platform.h"
#include "bypass_action.h"
#include "bypass_i2cmux.h"
#include "bypass_smbus.h"

#include USER_INTERFACE_HEAD

static uint32_t card_conf      = 0;
#if 0
module_param(card_conf, int, S_IRUGO);
static char *board    = "";

MODULE_PARM_CHAR(board);
#else
#define MAX_BYPASS		2

extern char bootserial[25];
static char board[8];
static int number_of_bypass_device;

static struct _FUTURE_BYPASS {
	int slot_no;
	int seg_no;
} bypass_info[MAX_BYPASS];
#endif

bp_interface bp_int[NET_INTERFACE_MAX];
static struct i2c_client eeprom_client;

extern struct semaphore i2cmux_sem;

struct miscdevice ca_bypass_mdev = {
	.name = DRV_NAME,
	.minor = MISC_DYNAMIC_MINOR,
	.fops = NULL,
};

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
	u8 *slot = client->dev.platform_data;

	//From interface 0~NET_INTERFACE_MAX-1, find the seg which matching the client->addr
	for (i=0; i<NET_INTERFACE_MAX; i++) {
		if (bp_int[i].slot_no == *slot){
			for(j=0; j<NET_INTERFACE_MAX_BP_SEGS; j++) {
				if (client->addr == bp_int[i].seg[j].ctrl_addr) {
					if (bp_int[i].seg[j+index].ctrl_addr == client->addr) {
						*seg = &bp_int[i].seg[j+index];
						return 0;
					} else {
						return 1;
					}
				}
			}
		}
	}
	return 1;
}

int find_client_bp_interface(bp_interface **interface, struct i2c_client *client, int index)
{
	int i, j;
	u8 *slot = client->dev.platform_data;

	//From interface 0~NET_INTERFACE_MAX-1, find the seg which matching the client->addr,return Interface Number
	for (i=0; i<NET_INTERFACE_MAX; i++) {
		if (bp_int[i].slot_no == *slot){
			for(j=0; j<NET_INTERFACE_MAX_BP_SEGS; j++) {
				if (client->addr == bp_int[i].seg[j].ctrl_addr) {
					if (bp_int[i].seg[j+index].ctrl_addr == client->addr) {
						*interface = &bp_int[i];
						return 0;
					} else {
						return -1;
					}
				}
			}
		}
	}
	return 1;
}

int find_slot_bp_interface(bp_interface **interface, uint8_t slot){
	int i;

	for (i=0; i<NET_INTERFACE_MAX; i++) {
		if (bp_int[i].slot_no == slot){
			*interface = &bp_int[i];
			return 0;
		}
	}

	return 1;
}

uint8_t get_bp_seg_ctrl_addr(uint8_t slot, uint8_t seg){
	int i;

	for (i=0; i<NET_INTERFACE_MAX; i++) {
		if (bp_int[i].slot_no == slot)
			return bp_int[i].seg[seg].ctrl_addr;
	}

	return 0;
}

int bypass_check_sdp(pci_ctrl_iface* ctrl, const bp_pci_cmd* cmd)
{
	uint32_t reg_value;
	uint32_t result;

	outl(cmd->reg_offset, ctrl->ioaddr);
	reg_value = inl(ctrl->iodata);

	result = (reg_value&~(cmd->cmd_mask|cmd->dir_mask));
	if(result == 0xffffffff)	return 0;
	return 1;
}

void bypass_pci_set_high(pci_ctrl_iface* ctrl, const bp_pci_cmd* cmd, long delay)
{
	uint32_t reg_value;

	outl(cmd->reg_offset, ctrl->ioaddr);
	reg_value = inl(ctrl->iodata);

	outl(reg_value|(cmd->cmd_mask|cmd->dir_mask), ctrl->iodata);
	udelay(delay);
}

void bypass_pci_set_high_low(pci_ctrl_iface* ctrl, const bp_pci_cmd* cmd, long delay)
{
	uint32_t reg_value;

	outl(cmd->reg_offset, ctrl->ioaddr);
	reg_value = inl(ctrl->iodata);

	outl(reg_value|cmd->cmd_mask, ctrl->iodata);
	udelay(delay);
	outl(reg_value&(~cmd->cmd_mask) ,ctrl->iodata);
	udelay(delay);
	outl(reg_value|cmd->cmd_mask, ctrl->iodata);
}

void bypass_reg_set_high_low(struct i2c_client *client, bp_cmd *bp_cmd, u8 reg_reposit, long delay)
{
	u8 bit_high, cmd, tmpval = 0;

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
		udelay(delay);
	}

	if (bp_cmd->cmd) {
		i2c_smbus_write_byte_data(client , bp_cmd->reg_out, cmd);
		CA_BOARD_DEBUG("Bypass set addr=0x%x, reg=0x%x, cmd=0x%x\n", client->addr,
				bp_cmd->reg_out, cmd);  
		udelay(delay);
	}

	if (bp_cmd->bit_high) {
		i2c_smbus_write_byte_data(client , bp_cmd->reg_out, bit_high);
		CA_BOARD_DEBUG("Bypass set addr=0x%x, reg=0x%x, cmd=0x%x\n", client->addr,
				bp_cmd->reg_out,  bit_high);
		udelay(delay);
	}
}

int client_is_bp_wdt(struct i2c_client *client, int interface) {
	int i;
	u8 *slot = client->dev.platform_data;

	if (bp_int[interface].slot_no != *slot)
		return 0;

	//search to find if the client addr is fiber bypass wdt ctrl
	for (i=0; i<NET_INTERFACE_MAX_BP_SEGS; i++) {
		if (client->addr == bp_int[interface].seg[i].wdt_addr)
			return 1;
		if(bp_int[interface].seg[0].name == BP_CARD_NIP5514){
			if (client->addr == bp_int[interface].seg[i].period_addr)
				return 1;
		}
		continue;
	}

	return 0;
}

int client_is_bp_eeprom(struct i2c_client *client) {
	int i,j;
	u8 *slot = client->dev.platform_data;

	//search to find if the client addr is fiber bypass wdt ctrl
	for (i=0; i<NET_INTERFACE_MAX; i++) {
		if (bp_int[i].slot_no == *slot){
			for (j=0; j<NET_INTERFACE_MAX_BP_SEGS; j++) {
				if(bp_int[i].seg[j].type==BP_COPPER_CPLDV2)
					if (client->addr == bp_int[i].seg[j].eeprom_addr)
						return 1;
			}
		}
	}
	return 0;
}

int client_is_bp_ctrl(struct i2c_client *client, int interface) {
	int i;
	u8 *slot = client->dev.platform_data;

	if (bp_int[interface].slot_no != *slot)
		return 0;

	//search to find if the client addr is fiber bypass wdt ctrl
	for (i=0; i<NET_INTERFACE_MAX_BP_SEGS; i++) {
		if (client->addr == bp_int[interface].seg[i].ctrl_addr)
			return 1;
		continue;
	}

	return 0;
}

int bypass_check_cpld_ver_ABN484(struct i2c_client *client, unsigned short addr)
	/*Read CPLD version from ABN484L/ABN484 */
{
	struct i2c_client bp_client_to_read, *bp_client = &bp_client_to_read;
	int retval=0,icnt;
	unsigned int buff=0;
	bp_interface* bp_iface = NULL;
	bp_seg* seg = NULL;

	memcpy(bp_client, client, sizeof(struct i2c_client));
	if ( addr == 0 )
		return 0xff;

	bp_client->addr = addr;

	if (find_client_bp_seg(&seg, bp_client, 0))
		return 0xff;

	bp_iface = seg->bp_iface;

	/*Let WDT5 ( bit7 of OUTPORT0 ) be high. (we use WDT5 as SCLK) */
	buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff | 0x80));

	/*FORCE WDT_DISJ_A to low level to make sure correct initial LFSR for CPLD rev*/
	buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
	i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff & 0xfd));

	/*LOADJ_A (bit0) LH edge to make sure proper initial LFSR value*/
	if (bp_iface->pci_ctrl.ioaddr != 0){
		outl(seg->pci_wdt_refresh->reg_offset, bp_iface->pci_ctrl.ioaddr);
		buff = inl(bp_iface->pci_ctrl.iodata);
		outl(buff&(~seg->pci_wdt_refresh->cmd_mask) ,bp_iface->pci_ctrl.iodata);
		udelay(15);
		outl(buff | seg->pci_wdt_refresh->cmd_mask ,bp_iface->pci_ctrl.iodata);
	}else{
		buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
		i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff & 0xfe));
		i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff | 0x01));
	}

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
	if (bp_iface->pci_ctrl.ioaddr != 0){
		outl(seg->pci_wdt_refresh->reg_offset, bp_iface->pci_ctrl.ioaddr);
		buff = inl(bp_iface->pci_ctrl.iodata);
		outl(buff&(~seg->pci_wdt_refresh->cmd_mask) ,bp_iface->pci_ctrl.iodata);
		udelay(15);
		outl(buff | seg->pci_wdt_refresh->cmd_mask ,bp_iface->pci_ctrl.iodata);
	}else{
		buff=i2c_smbus_read_byte_data(bp_client, PCA9555_INPUT_0);
		i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff & 0xfe));
		i2c_smbus_write_byte_data(bp_client, PCA9555_OUTPUT_0, (buff | 0x01));
	}

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
	int i,j;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if(eeprom_client.addr==0)
		memcpy(&eeprom_client, client, sizeof(struct i2c_client));

	for(i=0; i<NET_INTERFACE_MAX; i++) {
		if (bp_int[i].seg[0].type == BP_FIBER_PIC_ABN482) {
			if (client_is_bp_wdt(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, FIBER_PIC_ABN482_WDT_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, FIBER_PIC_ABN482_WDT_DR1_DF);
				i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, FIBER_PIC_ABN482_WDT_OUT1_DF); 
			} 
			if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, FIBER_PIC_ABN482_CTRL_DR0_DF);
				// For PCA9539 design, io0~7: pic control io8~15 wdt control
				if((client->addr >= 0x74)&&(client->addr <= 0x77)){
					i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, FIBER_PIC_PCA9539_CTRL_DR1_DF);
					i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, FIBER_PIC_ABN482_WDT_OUT1_DF); 
				}else
					i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, FIBER_PIC_ABN482_CTRL_DR1_DF);

				for (j=0; j<2; j++) {
					//Forcing do WDT clear at setup time
					bypass_set_wdt(&bp_client, j, WDT_CLEAR);
					/* Initial period value to 31, because the fixed firmware version issue */
					bypass_set_period(&bp_client, j, 31);
				}
			}
		} else if (bp_int[i].seg[0].type == BP_COPPER_CPLDV2) {
			if (client_is_bp_wdt(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, COPPER_CPLD_V2P0_CTRL_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, COPPER_CPLD_V2P0_CTRL_DR1_DF);
			}
			if (client_is_bp_ctrl(client, i)) {
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_0, COPPER_CPLD_V2P0_CTRL_DR0_DF);
				i2c_smbus_write_byte_data(client , PCA9555_DIRECTION_1, COPPER_CPLD_V2P0_CTRL_DR1_DF);
				/*Usage get BPE from PCA9559 for synchronization of 9555_BPE and 9559_BPE*/
				i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_0, COPPER_CPLD_V2P0_CTRL_OUT0_DF);
				find_client_bp_seg(&bp_seg, client,0);

				//In ABN484L, we need to set the pca9559 A0, A1 address pin by secondary pca9555(0x24~0x26)
				if (((client->addr>=0x24)&&((bp_int[i].seg[0].name==BP_CARD_ABN484L)||(bp_int[i].seg[0].name==BP_CARD_NIP54121)))||(bp_int[i].seg[0].name==BP_CARD_NIP5514)) {
					//sync the pca9559 (A0, A1) to 2th pca9555 (A0, A1) by pca9555 (IO06, IO07)
					i2c_smbus_write_byte_data(client,PCA9555_OUTPUT_0,(0x3F|((client->addr&0x03)<<0x06)));
				}

				// Check BPE_9559 , BPE_9559=0, then BPE_9555=0(0x2f)
				if(copper_cpld_eeprom_read_bpe(bp_seg->eeprom_addr,bp_seg->index)==0)
					i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, COPPER_CPLD_V2P0_CTRL_OUT1_DF);
				else // otherwise, BPE_9555=1(0x3f)
					i2c_smbus_write_byte_data(client , PCA9555_OUTPUT_1, (COPPER_CPLD_V2P0_CTRL_OUT1_DF|0x10));

				/* Initial period value to maximum */
				for (j=0; j<2; j++)
					bypass_set_period(&bp_client, j, COPPER_CPLD_V2P0_WDT_PERIOD_MAX);
			}
		}
	}
	return 0;
}

int bypass_create_fs(struct i2c_client *client)
{
	int err, i;

	for(i=0; i < NET_INTERFACE_MAX; i++) {
		if (client_is_bp_ctrl(client, i)) {
			if(!((bp_int[i].seg[0].name==BP_CARD_NIP54121)&&(client->addr>=0x24))) {
				if ((err = bypass_setup_fs(&ca_bypass_mdev, bp_int[i].slot_no, (client->addr >> 2) & 0x1))) {
					goto exit_free;
				}
			}					
		}
	}

	return 0;

exit_free:
	return -1;
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
			FIBER_ABN1004_DISABLE_WDT, FIBER_ABN1004_DISABLE_WDT|FIBER_ABN1004_WDT_DIS_BIT);

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
			FIBER_ABN482_DISABLE_WDT, FIBER_ABN482_DISABLE_WDT|FIBER_ABN482_WDT_DIS_BIT);

	bp_cmd_setup(&(seg_struct->wdt_refresh), PCA9555_OUTPUT_1, 0,
			FIBER_ABN482_REFRESH_BP_WDT, FIBER_ABN482_REFRESH_BP_WDT|FIBER_ABN482_WDT_LOAD_BIT);

	bp_cmd_setup(&(seg_struct->wdt_clear), PCA9555_OUTPUT_1, 0,
			FIBER_ABN482_CLR_BP_WDT, FIBER_ABN482_CLR_BP_WDT|FIBER_ABN482_WDT_CLEAR_BIT);

	bp_cmd_setup(&(seg_struct->set_period), PCA9555_OUTPUT_0, PCA9555_INPUT_0, 0, 0);
}

int find_bypass_mb_index(unsigned char* board_conf)
{
	int index;

	for(index=0; index<BP_MB_MAX; index++) {
		if(strcmp(board_conf, BP_MB_NAME[index])==0) {
			//return the board model index
			return index;
		} 
	}
	return -1;
}

static inline uint32_t sdp_read (uint16_t ioaddr, uint16_t iodata, uint32_t reg){
	outl(reg, ioaddr);
	return inl(iodata);
}


static inline int is_slot_dev (uint16_t ioaddr, uint16_t iodata, uint32_t slot_chk_reg, uint32_t slot_chk_mask){
	return  (sdp_read(ioaddr, iodata, slot_chk_reg) & slot_chk_mask);
}

static inline int get_slot_id (uint16_t ioaddr, uint16_t iodata, uint32_t slot_id_reg0,uint32_t slot_id_reg1, 
		uint32_t slot_id_mask0, uint32_t slot_id_mask1){

	uint32_t reg_val0 = sdp_read (ioaddr, iodata, slot_id_reg0);
	uint32_t reg_val1 = sdp_read (ioaddr, iodata, slot_id_reg1);
	return  ((reg_val0 & slot_id_mask0) ? 1 : 0)  + ((reg_val1 & slot_id_mask1) ? 2 : 0);
}

/*Get ioaddr and iodata for intel MAC chip*/
static inline int get_io_iface(struct pci_dev* dev, int bar_offset, uint16_t* ioaddr_pt, uint16_t* iodata_pt){
	uint32_t reg_val;

	if (pci_read_config_dword(dev, bar_offset, &reg_val) || !IS_IOSPACE(reg_val) )
		return -EINVAL;

	reg_val &= ~0x1;

	*ioaddr_pt = reg_val + IOADDR_OFFSET;
	*iodata_pt = reg_val + IODATA_OFFSET;

	return 0;
}


static void attatch_macs(mac_list* macs, const bp_pci_info *info){
	struct pci_dev* pci_slot_dev = NULL;
	struct pci_dev* wdt_mac_dev = NULL;
	mac_list* mac_list_pt;
	uint16_t ioaddr;
	uint16_t iodata;

	INIT_LIST_HEAD(&macs->dev_list);

	while ( (pci_slot_dev = pci_scan_subsys(info->vid, info->did, info->svid, info->sdid, pci_slot_dev))){

		/*Obtain base address for io space*/
		if (get_io_iface(pci_slot_dev, info->io_bar_offset, &ioaddr, &iodata)){
			CA_BOARD_DEBUG("Cannot get PCI device with VID: %x, DID: %x, SVID: %x, SDID: %x!!\n",
					info->vid, info->did, info->svid, info->sdid);
			continue;
		}

		/*Exam if this pci device is validate*/
		if(pci_slot_dev->devfn == 0){
			if((info->sdid == SDID_ABN484_SDP && is_slot_dev(ioaddr, iodata, info->slot_chk_reg, info->slot_chk_mask))||(info->sdid == SDID_ABN482_SDP)){

				mac_list_pt = kmalloc(sizeof(mac_list), GFP_KERNEL);
				if (mac_list_pt == NULL){
					printk("Bypass init failed!! memory allocation fail.\n");
					continue;
				}

				mac_list_pt->slot_id = get_slot_id(ioaddr, iodata, info->slot_id_reg0, info->slot_id_reg1, 
						info->slot_id_mask0,info->slot_id_mask1);

				/*The following function will increase PCI reference count in Linux 2.6*/
				wdt_mac_dev = pci_scan_bus_and_slot(pci_slot_dev->bus->number, pci_slot_dev->devfn | info->slot_dev_mask);

				if (!wdt_mac_dev || get_io_iface(wdt_mac_dev, info->io_bar_offset, &ioaddr, &iodata)){
					printk("Cannot get WDT device!!\n");
					continue;
				}

				mac_list_pt->mac_dev = wdt_mac_dev;
				mac_list_pt->ioaddr = ioaddr;
				mac_list_pt->iodata = iodata;

				list_add_tail(&mac_list_pt->dev_list, &macs->dev_list);
			}
		}
	};
}


void bp_register_pci_mac_type(const bp_pci_info * type_info, bp_mac_type_list* bp_macs)
{
	bp_mac_type_list* list_pt;

	/*Check if this type has bean already registereed*/
	list_for_each_entry(list_pt, &bp_macs->type_list, type_list){
		if (IS_THE_SAME_PCI_TYPE(type_info, list_pt->type_info))
			return;
	}

	/*Since this type of mac is not registered, we do this now*/

	/*Create new node*/
	list_pt = kmalloc(sizeof(bp_mac_type_list), GFP_KERNEL);
	if(!list_pt){
		printk("Bypass init failed!! Cannot create MAC type\n");
		return;
	}		    

	list_pt->type_info = type_info;
	list_add_tail(&list_pt->type_list, &bp_macs->type_list);

	attatch_macs(&list_pt->mac_list, type_info);
}

void bp_unregister_all_pci_mac_type(bp_mac_type_list* bp_macs){
	bp_mac_type_list* macs_pt;
	mac_list* mac_list_pt;

	list_for_each_entry(macs_pt, &bp_macs->type_list, type_list){
		list_for_each_entry(mac_list_pt, &macs_pt->mac_list.dev_list, dev_list){
			pci_dev_put(mac_list_pt->mac_dev);
			kfree(mac_list_pt);
		}
		kfree(macs_pt);
	}
}


void bp_pci_mac_bound(bp_interface* bp_iface, const bp_pci_initdata* idata, bp_mac_type_list* macs){
	int i;
	uint8_t slot_id;
	bp_mac_type_list* macs_pt;
	mac_list *mac_list_pt, *mac_list_next;
	const bp_pci_info* idata_info = &idata->info;

	/*Currently, only ABN484 is sasified the following rule*/
	slot_id = CTRL_ADDRESS_TO_SLOT_ID(bp_iface->seg[0].ctrl_addr);

	list_for_each_entry(macs_pt, &macs->type_list, type_list){
		if (!IS_THE_SAME_PCI_TYPE(idata_info, macs_pt->type_info))
			continue;

		list_for_each_entry_safe(mac_list_pt, mac_list_next, &macs_pt->mac_list.dev_list, dev_list){

			if (mac_list_pt->slot_id == slot_id){

				bp_iface->mac_dev = mac_list_pt->mac_dev;

				bp_iface->pci_ctrl.ioaddr = mac_list_pt->ioaddr;
				bp_iface->pci_ctrl.iodata = mac_list_pt->iodata;
				spin_lock_init(&bp_iface->pci_ctrl.sdp_spin_lock);

				for (i = 0; i < bp_iface->seg_no; i ++)
					bp_iface->seg[i].pci_wdt_refresh = (idata->refresh_wdt_cmds + i);

				list_del(&mac_list_pt->dev_list);
				kfree(mac_list_pt);
				break;
			}
		}
	}
}

void list_mac_type_and_dev(bp_mac_type_list* macs){
	bp_mac_type_list* pt;
	mac_list* mac_pt;
	struct pci_dev* dev;

	list_for_each_entry(pt, &macs->type_list, type_list){
		list_for_each_entry(mac_pt, &pt->mac_list.dev_list, dev_list)
			dev = mac_pt->mac_dev;
	}
}


void release_bp_resource(void){
	int interface, no;

	for (interface = 0; interface<NET_INTERFACE_MAX; interface++){
		if (bp_int[interface].mac_dev)
			pci_dev_put(bp_int[interface].mac_dev);

		for (no = 0; no < bp_int[interface].seg_no; no++){
			if (timer_pending(&bp_int[interface].seg[no].trigger_timer))
				del_timer_sync(&bp_int[interface].seg[no].trigger_timer);
		}

		if (bp_int[interface].seg_no > 0)
			bypass_unsetup_fs(&ca_bypass_mdev, bp_int[interface].slot_no);
	}
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
	unsigned char ca_bypass_firmware_ver_addr[NET_INTERFACE_MAX];
	unsigned char *ca_bypass_modelname[NET_INTERFACE_MAX];
	unsigned char ca_bypass_slot_no[NET_INTERFACE_MAX];
	const bp_pci_initdata *ca_bypass_pci_initdata[NET_INTERFACE_MAX];
	int interface=0, seg_total=0, seg_index=0, cur_seg;
	int mb_index, card_index=0, slot_num=0, slot_conf;
	int no, pos, i;
	bp_mac_type_list bp_macs;
	INIT_LIST_HEAD(&bp_macs.type_list);

	memset(ca_bypass_ctrl_addr,0,sizeof(ca_bypass_ctrl_addr));
	memset(ca_bypass_wdt_addr,0,sizeof(ca_bypass_wdt_addr));
	memset(ca_bypass_period_addr,0,sizeof(ca_bypass_period_addr));
	memset(ca_bypass_eeprom_addr,0,sizeof(ca_bypass_eeprom_addr));
	memset(ca_bypass_seg_no,0,sizeof(ca_bypass_seg_no));
	memset(ca_bypass_type,0,sizeof(ca_bypass_type));
	memset(ca_bypass_gen,0,sizeof(ca_bypass_gen));
	memset(ca_bypass_name,0,sizeof(ca_bypass_name));
	memset(ca_bypass_modelname,0,sizeof(ca_bypass_modelname));
	memset(ca_bypass_pci_initdata,0,sizeof(ca_bypass_pci_initdata));

	/* Search the bypass model index */
	if ((mb_index = find_bypass_mb_index(board)) == (-1)) {
		CA_BOARD_DEBUG("Board parameter may be invalid or has no on board bypass module");
		return -1;
	} else {
		/* Do motherboard information configuring */
		ca_bypass_type[interface] = mb_bypass_info[mb_index].type;
		ca_bypass_gen[interface] = mb_bypass_info[mb_index].generation;
		ca_bypass_name[interface] = mb_bypass_info[mb_index].name;
		ca_bypass_modelname[interface] = BP_MB_NAME[mb_index];
		ca_bypass_seg_no[interface] = mb_bypass_info[mb_index].segs;
		ca_bypass_firmware_ver_addr[interface] = mb_bypass_info[mb_index].firmware_ver_addr;
		ca_bypass_pci_initdata[interface] = mb_bypass_info[mb_index].pci_initdata;

		if (mb_bypass_info[mb_index].pci_initdata)
			bp_register_pci_mac_type(&mb_bypass_info[mb_index].pci_initdata->info, &bp_macs);

		for(seg_index=0; seg_index<mb_bypass_info[mb_index].segs; seg_index++){
			cur_seg = seg_total+seg_index;
			ca_bypass_ctrl_addr[cur_seg] = mb_bypass_info[mb_index].ctrl_addr[seg_index];
			ca_bypass_eeprom_addr[cur_seg] = mb_bypass_info[mb_index].eeprom_addr[seg_index];
			ca_bypass_wdt_addr[cur_seg] = mb_bypass_info[mb_index].wdt_addr[seg_index];
			ca_bypass_period_addr[cur_seg] = mb_bypass_info[mb_index].period_addr[seg_index];
		}
		if (ca_bypass_gen[interface] > 0) {
			slot_num = mb_bypass_info[mb_index].ctrl_addr[1] % 4;
			ca_bypass_slot_no[interface] = slot_num;
			seg_total += mb_bypass_info[mb_index].segs;
		} else {
			ca_bypass_slot_no[interface] = -1;
		}
		interface++;
	}

	/* Do add-card information configuring */
	if(card_conf) {
		/* Config slot information according to input card_conf */
		for(slot_num=0; slot_num<ADDCARD_MAX_SLOTS; slot_num++) {
			/* Check if current slot is configured, only 4-bits for one slot */
			/* Since the input ex. conf=0x201, 2 means slot2 and 1 for slot0, we need to reorder card_conf */
			slot_conf = (card_conf >> (slot_num * 4)) & 0xf;
			/* Check if this is a meaningful conf */
			if (slot_conf > BP_CARD_MAX_PLATEFORM) {
				CA_BOARD_DEBUG("Err! Bad card_conf parameter");
				return -1;
			}
			/* Current slot config is 0, skip this */
			if (!slot_conf)
				continue;
			card_index = slot_conf-1;

			ca_bypass_type[interface] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].type;
			ca_bypass_gen[interface] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].generation;
			ca_bypass_name[interface] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].name;
			ca_bypass_modelname[interface] = BP_CARD_NAME[card_index];
			ca_bypass_seg_no[interface] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].segs;
			ca_bypass_firmware_ver_addr[interface] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].firmware_ver_addr;
			ca_bypass_pci_initdata[interface] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].pci_initdata;
			if (ADDCARD_BYPASS_INFO[slot_num][card_index].pci_initdata)
				bp_register_pci_mac_type(&ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].pci_initdata->info, &bp_macs);
			for(seg_index=0; seg_index<ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].segs; seg_index++) {
				cur_seg = seg_total+seg_index;
				ca_bypass_ctrl_addr[cur_seg] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].ctrl_addr[seg_index];
				ca_bypass_eeprom_addr[cur_seg] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].eeprom_addr[seg_index];
				ca_bypass_wdt_addr[cur_seg] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].wdt_addr[seg_index];
				ca_bypass_period_addr[cur_seg] = ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].period_addr[seg_index];
			}
			ca_bypass_slot_no[interface] = slot_num;
			seg_total += ADDCARD_BYPASS_INFO[slot_num % MAX_PCI_SLOTS_PER_CPU][card_index].segs;
			interface++;
		}
	}

	/* Fill each interface bypass data into bp_int[] which handle the each bypass seg action */
	for(interface=0; interface<NET_INTERFACE_MAX; interface++) {
		bp_int[interface].seg_no = ca_bypass_seg_no[interface];
		bp_int[interface].modelname = ca_bypass_modelname[interface];
		bp_int[interface].fw_version = 0x0;
		bp_int[interface].slot_no = ca_bypass_slot_no[interface];
		for(no = 0; no < bp_int[interface].seg_no; no++) {
			//To find the exaclty position
			pos = 0;
			for(i=0; i<interface; i++)
				pos += bp_int[i].seg_no;
			pos += no;

			bp_int[interface].seg[no].bp_iface = &bp_int[interface]; 

			bp_int[interface].seg[no].type = ca_bypass_type[interface];
			bp_int[interface].seg[no].name= ca_bypass_name[interface];
			bp_int[interface].seg[no].generation=ca_bypass_gen[interface];
			bp_int[interface].seg[no].firmware_ver_addr = ca_bypass_firmware_ver_addr[interface];
			bp_int[interface].seg[no].index = (no%3);
			bp_int[interface].seg[no].ctrl_addr = ca_bypass_ctrl_addr[pos];
			bp_int[interface].seg[no].wdt_addr = ca_bypass_wdt_addr[pos];
			bp_int[interface].seg[no].period_addr = ca_bypass_period_addr[pos];
			bp_int[interface].seg[no].eeprom_addr = ca_bypass_eeprom_addr[pos];

			init_MUTEX (&bp_int[interface].seg[no].bp_sem);
			init_timer (&bp_int[interface].seg[no].trigger_timer);

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
			printk("bp%d:%d ctrl=0x%x, wdt=0x%x, period=0x%x, eeprom=0x%x, slot=%d\n", interface,
					no, bp_int[interface].seg[no].ctrl_addr, bp_int[interface].seg[no].wdt_addr,
					bp_int[interface].seg[no].period_addr, bp_int[interface].seg[no].eeprom_addr, bp_int[interface].slot_no);

			bypass_info[number_of_bypass_device].slot_no = bp_int[interface].slot_no;
			bypass_info[number_of_bypass_device].seg_no = no;

			number_of_bypass_device++;
		}

		if (ca_bypass_pci_initdata[interface]){
			bp_pci_mac_bound(&bp_int[interface], ca_bypass_pci_initdata[interface], &bp_macs);
			list_mac_type_and_dev(& bp_macs);
		}
	}
	bp_unregister_all_pci_mac_type(&bp_macs);
	return 0;
}

static int cas_gen2bp_probe(void){
	int interface, index;
	struct i2c_client *client;

	for (interface = 0; interface < NET_INTERFACE_MAX; interface++){
		if (bp_int[interface].seg_no > 0){
			bypass_smbus_client_create(&bp_int[interface].slot_no);
			for (index = (MAX_CLIENT_ADDR_PER_SLOT - 1); index >= 0; index--){
				if (find_bypass_mb_index(board) == BP_MB_NAR5520) {
					client = bypass_get_i2c_client_by_idx_NAR5520(bp_int[interface].slot_no, index);
					if (client == NULL)
						continue;
				}
				else
					client = bypass_get_i2c_client_by_idx(bp_int[interface].slot_no, index);

				if (down_interruptible(&i2cmux_sem))
					return -ERESTARTSYS;

				bypass_switch_i2cmux(bp_int[interface].slot_no);
				bypass_client_fill(client);

				up(&i2cmux_sem);
			}

			for (index = 0; index < MAX_CLIENT_ADDR_PER_SLOT; index++){
				if (find_bypass_mb_index(board) == BP_MB_NAR5520) {
					client = bypass_get_i2c_client_by_idx_NAR5520(bp_int[interface].slot_no, index);
					if (client == NULL)
						continue;
				}
				else
					client = bypass_get_i2c_client_by_idx(bp_int[interface].slot_no, index);

				bypass_create_fs(client);
			}
			bypass_register_fs(&ca_bypass_mdev, bp_int[interface].slot_no);
		}
	}

	return 0;
}

static int __init cas_gen2bp_init(void)
{
	int ret;

	if (!strncmp(bootserial, "WNA401A", 7))
		strcpy(board, "CAR3030");
	else if(!strncmp(bootserial, "WNA601A", 7) ||
			!strncmp(bootserial, "WNA601B", 7))
		strcpy(board, "CAR4010");
	else if(!strncmp(bootserial, "WNA801A", 7) ||
			!strncmp(bootserial, "WNA102A", 7) ||
			!strncmp(bootserial, "WNA202A", 7))
		strcpy(board, "CAR5030");

	if ((ret = find_bypass_mb_index(board)) == (-1)) {
		printk("Board parameter may be invalid or has no on board bypass module\n");
		return -ENODEV;
	}

	if (card_conf == 0x0)
		gen2bp_init(board, &card_conf);

	if(ca_bypass_modeling()){
		printk("Setup and fill the bypass model data failed!\n");
		return -1;
	}

	printk("CA network-bypass driver %s loaded\n", CA_BOARD_PATCH_VER_STR);

	ret = misc_register(&ca_bypass_mdev);
	if (ret) {
		printk("Loading CA network-bypass driver failed !!\n");
		goto err_misc;
	}

	if (bypas_mux_init(board)){
		printk("bypass_mux_init failed !\n");
		goto err_misc;
	}

	if (bypass_smbus_init()) {
		printk("bypass_smbus_init failed !\n");
		goto err_misc;
	}

	cas_gen2bp_probe();

	return ret;

err_misc:
	misc_deregister(&ca_bypass_mdev);

	return -ENODEV;
}

static void __exit cas_gen2bp_exit(void)
{
	release_bp_resource();
	bypass_mux_remove();
	misc_deregister(&ca_bypass_mdev);
}

MODULE_AUTHOR("Benjamin Wang <shengbau@cas-well.com>");
MODULE_DESCRIPTION("CASwell(R) Ethernet Bypass Gen2 Driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

#ifdef CONFIG_FERRET_NETWORK_BYPASS
late_initcall(cas_gen2bp_init);
#else
module_init(cas_gen2bp_init);
#endif
module_exit(cas_gen2bp_exit);

#define NFW_BYPASS_DISABLE      0       
#define NFW_BYPASS_ENABLE       2

#define NFW_BPE_DISABLE         0
#define NFW_BPE_ENABLE          1

#define NFW_NEXTBOOT_DISABLE    0
#define NFW_NEXTBOOT_ENABLE     1

int nfw_bypass_store(int slot_no, int seg_no, int cmd);
int nfw_nextboot_store(int slot_no, int seg_no, int cmd);
int nfw_bpe_store(int slot_no, int seg_no, int cmd);

int nfw_get_bypass_device_count(void)
{
	printk(KERN_INFO "found bypass devices : %d\n", number_of_bypass_device);
	return number_of_bypass_device;
} 

void nfw_carboard_enable(int port)
{
	if (port < MAX_BYPASS) {
		nfw_bypass_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_BYPASS_ENABLE);
		nfw_bpe_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_BPE_ENABLE);
		nfw_nextboot_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_NEXTBOOT_ENABLE);
	}
} 

void nfw_carboard_disable(int port)
{
	if (port < MAX_BYPASS) {
		nfw_bypass_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_BYPASS_DISABLE);
		nfw_bpe_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_BPE_DISABLE);
		nfw_nextboot_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_NEXTBOOT_DISABLE);
	}
} 

void nfw_carboard_wd_reset(int port)
{
} 

void nfw_carboard_wd_timeout(int port, int timeout)
{
} 

void nfw_carboard_init(int port)
{
	if (port < MAX_BYPASS) {
		nfw_bypass_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_BYPASS_DISABLE);
		nfw_bpe_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_BPE_ENABLE);
		nfw_nextboot_store(bypass_info[port].slot_no,
				bypass_info[port].seg_no, NFW_NEXTBOOT_ENABLE);
	}
} 
