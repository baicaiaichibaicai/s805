#include <linux/i2c.h>
#include "bypass_control.h"
#include "bypass_kversion.h"

#define __INCLUDE_PLATEFORM_DATA__
#include "bypass_platform.h"

#include "bypass_action.h"

static u8 copper_pic_wdt_period_tab[] =COPPER_PIC_WDT_PERIOD;



/* bypass_status_read - return value: 
 * For Gen2 - 0: normal , 1: open, 2: bypass
 * For Gen1 - 0: normal , 1: non-normal*/

static int bypass_status_read(struct i2c_client* client, bp_seg * bp_seg)
{
	int sts = 0, bpe = 0;

	bypass_reg_set_high_low(client, &bp_seg->get_bypass, bp_seg->reg_reposit, bp_seg->delay_ms);
	/*Read the sts0 and sts1 pin which now means bypass status. 0 is normal, x is none normal*/
	if ((bp_seg->type == BP_COPPER_PIC) ||(bp_seg->type == BP_COPPER_CPLDV2))
		sts = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) & (bp_seg->bp_sts_bit0))? 1:0;
	else if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004)
		sts = ((i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) &
					(bp_seg->bp_sts_bit1|bp_seg->bp_sts_bit0))? 1:0);

	/*In non-normal mode with gen 2, bpe will decide the non-normal status to be open or bypass*/
	if (sts != 0 && bp_seg->generation != 1) {
		bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
		if (bp_seg->type == BP_COPPER_CPLDV2)
			bpe = copper_cpld_eeprom_read_bpe(bp_seg->eeprom_addr, bp_seg->index);
		else
			bpe = ( (i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in) &
						bp_seg->bp_sts_bit0) ? 1:0 );
		sts += bpe;
	}

	return sts;

}

static void bypass_status_write(struct i2c_client* client, bp_seg * bp_seg, int cmd_val)
{
	int tmpval;

	switch(cmd_val) {
		case SET_NORMAL:
			bypass_reg_set_high_low(client, &bp_seg->set_normal, bp_seg->reg_reposit, bp_seg->delay_ms);
			break;

		case SET_OPEN:
			/*This is CPLD type, we need to set runtime bpe value*/
			if (bp_seg->type == BP_COPPER_CPLDV2) {
				tmpval = i2c_smbus_read_byte_data(client , bp_seg->set_bpe_open.reg_in);
				i2c_smbus_write_byte_data(client, bp_seg->set_bpe_open.reg_out,
						(tmpval&bp_seg->set_bpe_open.cmd));
				/*Write to 9559 eeprom for sync*/
				copper_cpld_eeprom_write_bpe(bp_seg->eeprom_addr, bp_seg->index, 0x0);
			} else {
				bypass_reg_set_high_low(client, &bp_seg->set_bpe_open, bp_seg->reg_reposit,
						bp_seg->delay_ms);
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
				bypass_reg_set_high_low(client, &bp_seg->set_bpe_bypass, bp_seg->reg_reposit,
						bp_seg->delay_ms);
			}

			bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit, bp_seg->delay_ms);
			break;

		default :
			break;
	}
}

/* bypass_nextboot_read 
 * return value - 1:non-normal. 0:normal*/
static int bypass_nextboot_read (struct i2c_client* client, bp_seg * bp_seg)
{
	/*eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom*/
	if (bp_seg->eeprom_addr) {
		return copper_cpld_eeprom_read_nb(bp_seg->eeprom_addr, bp_seg->index);
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
		return ((i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in)& bp_seg->bp_sts_bit1)? 1:0);
	}
}

static void bypass_nextboot_write (struct i2c_client* client, bp_seg * bp_seg, int cmd_val)
{
	/*eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom*/

	if (bp_seg->eeprom_addr) {
		copper_cpld_eeprom_write_nb(bp_seg->eeprom_addr, bp_seg->index, cmd_val);
	} else {
		switch(cmd_val) {
			case SET_NB_NORMAL:
				bypass_reg_set_high_low(client, &bp_seg->set_nb_normal, bp_seg->reg_reposit,
						bp_seg->delay_ms);
				break;

			case SET_NB_NO_NORMAL:
				bypass_reg_set_high_low(client, &bp_seg->set_nb_nonormal, bp_seg->reg_reposit,
						bp_seg->delay_ms);
				break;

			default :
				break;
		}
	}
}

/* bypass_get_bpe
 * return value: 0: open 1:bypass when in non-normal mode*/
static int bypass_bpe_read (struct i2c_client* client, bp_seg * bp_seg)
{

	/*eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom*/
	if (bp_seg->eeprom_addr) {
		return copper_cpld_eeprom_read_bpe(bp_seg->eeprom_addr, bp_seg->index);
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_nb, bp_seg->reg_reposit, bp_seg->delay_ms);
		return ((i2c_smbus_read_byte_data(client, bp_seg->get_nb.reg_in) & bp_seg->bp_sts_bit0)? 1:0);
	}
}

static void bypass_bpe_write (struct i2c_client* client, bp_seg * bp_seg, int cmd_val)
{
	int tmpval, bpe_val = 0;

	/*eeprom_addr exist=> this is CPLD type, we need to write to 9559 eeprom*/
	if (bp_seg->eeprom_addr) {
		switch(cmd_val) {
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
		copper_cpld_eeprom_write_bpe(bp_seg->eeprom_addr, bp_seg->index, cmd_val);
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_bypass, bp_seg->reg_reposit, bp_seg->delay_ms);
		switch(cmd_val) {
			case SET_BPE_OPEN:
				bypass_reg_set_high_low(client, &bp_seg->set_bpe_open, bp_seg->reg_reposit,
						bp_seg->delay_ms);
				if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004){
					bpe_val = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) & 
							(bp_seg->bp_sts_bit1|bp_seg->bp_sts_bit0))? 1:0;
					if (bpe_val)
						bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit                                                                  , bp_seg->delay_ms);
				}
				break;

			case SET_BPE_BP:
				bypass_reg_set_high_low(client, &bp_seg->set_bpe_bypass, bp_seg->reg_reposit,
						bp_seg->delay_ms);
				if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004){
					bpe_val = (i2c_smbus_read_byte_data(client, bp_seg->get_bypass.reg_in) &
							(bp_seg->bp_sts_bit1|bp_seg->bp_sts_bit0))? 1:0;
					if (bpe_val)
						bypass_reg_set_high_low(client, &bp_seg->set_nonormal, bp_seg->reg_reposit                                                                  , bp_seg->delay_ms);
				}
				break;

			default:
				break;
		}
	}	
}

static int bypass_timeout_bit_read (struct i2c_client* client, bp_seg * bp_seg)
{
	struct i2c_client bp_client;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if(bp_seg->wdt_addr)
		bp_client.addr = bp_seg->wdt_addr;

	/*There are low active at fiber PIC and copper CPLD bypass types*/

	if ((bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004) || 
			(bp_seg->type == BP_COPPER_CPLDV2))
		return ((i2c_smbus_read_byte_data(&bp_client, bp_seg->get_wdt.reg_in) & bp_seg->wdt_tmout_bit)? 0:1);
	else
		return ((i2c_smbus_read_byte_data(&bp_client, bp_seg->get_wdt.reg_in) & bp_seg->wdt_tmout_bit)? 1:0);
}

static int bypass_wdt_read (struct i2c_client* client, bp_seg * bp_seg)
{
	struct i2c_client bp_client;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->wdt_addr)
		bp_client.addr = bp_seg->wdt_addr;

	if(bp_seg->type == BP_COPPER_CPLDV2) {
		/*We don't support CPLD wdt status read now*/
		return 0xff;
	} else {
		bypass_reg_set_high_low(client, &bp_seg->get_wdt, bp_seg->reg_reposit, bp_seg->delay_ms);
		return ((i2c_smbus_read_byte_data(&bp_client, bp_seg->get_wdt.reg_in) & bp_seg->wdt_en_bit)? 1:0);
	}
}

static void bypass_wdt_write (struct i2c_client* client, bp_seg* bp_seg, int cmd_val)
{
	switch(cmd_val) {
		case WDT_DISABLE:
			bypass_reg_set_high_low(client, &bp_seg->wdt_disable, bp_seg->reg_reposit,
					bp_seg->delay_ms);
			break;

		case WDT_ARM_REFRESH:
			bypass_reg_set_high_low(client, &bp_seg->wdt_refresh, bp_seg->reg_reposit,
					bp_seg->delay_ms);
			break;

		case WDT_CLEAR:
			bypass_reg_set_high_low(client, &bp_seg->wdt_clear, bp_seg->reg_reposit,
					bp_seg->delay_ms);
			break;

		default :
			break;
	}
}

static int bypass_period_read (struct i2c_client* client, bp_seg * bp_seg)
{
	if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004)
		return i2c_smbus_read_byte_data(client, bp_seg->set_period.reg_in);
	else
		return bp_seg->wdt_period;
}

static void bypass_period_write (struct i2c_client* client, bp_seg * bp_seg, int cmd_val)
{
	unsigned long tmpval;
	int i;
	bp_interface* bp_iface = bp_seg->bp_iface;

	if (bp_seg->type == BP_FIBER_PIC_ABN482 || bp_seg->type == BP_FIBER_PIC_ABN1004) {
		/*Set period is just need to set second unit, only 1~63 sec would be allowed*/
		if (cmd_val> FIBER_ABN482_WDT_PERIOD_MAX)
			cmd_val = FIBER_ABN482_WDT_PERIOD_MAX;
		i2c_smbus_write_byte_data(client, bp_seg->set_period.reg_out, cmd_val);
	} else if (bp_seg->type == BP_COPPER_CPLDV2) {
		if (cmd_val> COPPER_CPLD_V2P0_WDT_PERIOD_MAX)
			cmd_val = COPPER_CPLD_V2P0_WDT_PERIOD_MAX;
		tmpval = i2c_smbus_read_byte_data(client, bp_seg->set_period.reg_in);
		tmpval &= 0x3;
		tmpval |= cmd_val<<2;
		i2c_smbus_write_byte_data(client, bp_seg->set_period.reg_out, tmpval);
	} else {
		if (cmd_val> COPPER_PIC_WDT_PERIOD_MAX)
			cmd_val = COPPER_PIC_WDT_PERIOD_MAX;
		i2c_smbus_write_byte_data(client, bp_seg->set_period.reg_out, COPPER_PIC_SET_CMD_BITS_HIGH);
		msleep(bp_seg->delay_ms);
		i2c_smbus_write_byte_data(client, bp_seg->set_period.reg_out, copper_pic_wdt_period_tab[cmd_val-1]);
	}

	for (i = 0; i < bp_iface->seg_no; i++)
		bp_iface->seg[i].wdt_period = cmd_val;
}

int bypass_get_status(struct i2c_client* client, int index)
{
	bp_seg *bp_seg;
	int sts;

	if (find_client_bp_seg(&bp_seg, client, index))
		return -EINVAL;

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	sts = bypass_status_read(client, bp_seg);

	up(&bp_seg->bp_sem);

	return sts;
}


int bypass_set_status(struct i2c_client* client, int index, int cmd_val)
{
	bp_seg *bp_seg;
	int ret = 0;

	if (cmd_val > 0xff || find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	if (bp_seg->auto_trigger_interval != 0){
		ret = -EPERM;
		printk(KERN_WARNING "Bypass [%#x]: Please stop WDT auto-trigger before this operation\n", 
				bp_seg->ctrl_addr);
	}else{
		bypass_status_write(client, bp_seg, cmd_val);
		/*The following msleep is used to wait relay to finish moving*/
		msleep(DELAY_FOR_RELAY);
	}

	up(&bp_seg->bp_sem);

	return ret;
}

int bypass_get_nextboot (struct i2c_client* client, int index)
{
	bp_seg *bp_seg;
	int nextboot; /*If NextBoot=1 => none normal, NextBoot=0 => normal*/

	if (find_client_bp_seg(&bp_seg, client, index)) 
		return -ENXIO;

	/*generation 1 BP not support nextboot*/
	if (bp_seg->generation==1)
		return -EPERM;

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	nextboot= bypass_nextboot_read(client, bp_seg);

	up(&bp_seg->bp_sem);

	return nextboot;
}

int bypass_set_nextboot (struct i2c_client* client, int index, int cmd_val)
{
	bp_seg *bp_seg;

	if (cmd_val > 0xff || find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;

	if (bp_seg->generation==1)
		return -EPERM;

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	bypass_nextboot_write(client, bp_seg, cmd_val);
	up(&bp_seg->bp_sem);

	return 0 ;
}

int bypass_get_bpe (struct i2c_client* client, int index)
{
	bp_seg *bp_seg;
	int bpe;

	if (find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;
	/*generation 1 BP not support bpe*/
	if(bp_seg->generation==1)
		return -EPERM;

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	bpe = bypass_bpe_read(client, bp_seg);

	up(&bp_seg->bp_sem);

	/*If BPE=1 => bypass, BPE=0 => open*/
	return bpe;

}

int bypass_set_bpe (struct i2c_client* client, int index, int cmd_val)
{
	bp_seg *bp_seg;

	if (cmd_val > 0xff || find_client_bp_seg(&bp_seg, client, index))
		return -EINVAL;
	/*generation 1 BP not support bpe*/
	if(bp_seg->generation==1)
		return -EPERM;

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	bypass_bpe_write(client, bp_seg, cmd_val);

	up(&bp_seg->bp_sem);

	return 0;
}

int bypass_get_timeout_bit (struct i2c_client* client, int index)
{
	bp_seg *bp_seg;
	int timeout;

	if (find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	timeout = bypass_timeout_bit_read(client, bp_seg);

	up(&bp_seg->bp_sem);

	return timeout;

}

int bypass_get_wdt (struct i2c_client* client, int index)
{
	bp_seg *bp_seg;
	int wdt_sts;

	if (find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	wdt_sts = bypass_wdt_read(client, bp_seg);

	up(&bp_seg->bp_sem);

	return wdt_sts;
}

void bypass_pci_set_sdp (pci_ctrl_iface* ctrl_iface, bp_seg * bp_seg)
{
	if(bypass_check_sdp(ctrl_iface, bp_seg->pci_wdt_refresh))
	{
		spin_lock(&ctrl_iface->sdp_spin_lock);
		bypass_pci_set_high(ctrl_iface, bp_seg->pci_wdt_refresh, bp_seg->delay_ms);
		spin_unlock(&ctrl_iface->sdp_spin_lock);
	}
}

void bypass_pci_set_wdt (pci_ctrl_iface* ctrl_iface, bp_seg * bp_seg)
{
	spin_lock(&ctrl_iface->sdp_spin_lock);
	bypass_pci_set_high_low(ctrl_iface, bp_seg->pci_wdt_refresh, bp_seg->delay_ms);
	spin_unlock(&ctrl_iface->sdp_spin_lock);
}



int bypass_set_wdt (struct i2c_client* client, int index, int cmd_val)
{
	struct i2c_client bp_client;
	bp_interface *bp_iface;
	bp_seg *bp_seg;
	int ret = 0;

	if (cmd_val > 0xff)
		return -EINVAL;
	if (find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;
	bp_iface = bp_seg->bp_iface;

	if (!bp_iface->pci_ctrl.iodata || cmd_val != WDT_ARM_REFRESH){
		memcpy(&bp_client, client, sizeof(struct i2c_client));
		if (bp_seg->wdt_addr)
			bp_client.addr = bp_seg->wdt_addr;
	}

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	if (cmd_val == WDT_CLEAR)
		bypass_wdt_write(&bp_client, bp_seg, cmd_val);
	else if (bp_seg->auto_trigger_interval != 0){
		ret = -EPERM;
		printk(KERN_WARNING "Bypass [%#x]: Please stop WDT auto-trigger before this operation\n", 
				bp_seg->ctrl_addr);
	}else{
		if (bp_iface->pci_ctrl.iodata && cmd_val == WDT_ARM_REFRESH){
			bypass_pci_set_sdp(&bp_iface->pci_ctrl, bp_seg);
			bypass_pci_set_wdt(&bp_iface->pci_ctrl, bp_seg);
		}
		else
			bypass_wdt_write(&bp_client, bp_seg, cmd_val);
	}

	up(&bp_seg->bp_sem);

	return ret;
}

static inline int get_all_iface_sem(bp_interface *bp_iface)
{
	int i,j;

	for (i = 0; i < bp_iface->seg_no; i++){
		if (down_interruptible(&bp_iface->seg[i].bp_sem)){
			for (j = i - 1; j >= 0; j--)
				up(&bp_iface->seg[j].bp_sem);
			return -ERESTARTSYS;
		}
	}

	return 0;
}

static inline void releas_all_iface_sem(bp_interface *bp_iface)
{
	int i;
	for (i = 0; i < bp_iface->seg_no; i++)
		up(&bp_iface->seg[i].bp_sem);
}

int bypass_get_period (struct i2c_client* client, int index)
{
	struct i2c_client bp_client;
	bp_interface *bp_iface;
	bp_seg *bp_seg;
	int period;

	if (find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;
	bp_iface = bp_seg->bp_iface;

	memcpy(&bp_client, client, sizeof(struct i2c_client));

	if (bp_seg->period_addr)
		bp_client.addr = bp_seg->period_addr;

	if (get_all_iface_sem(bp_iface))
		return -ERESTARTSYS;

	period = bypass_period_read(&bp_client, bp_seg);

	releas_all_iface_sem(bp_iface);

	return period;

}

int bypass_set_period (struct i2c_client* client, int index, int cmd_val)
{
	struct i2c_client bp_client;
	bp_interface *bp_iface;
	bp_seg *bp_seg;
	int i;

	if ((cmd_val > 0xff) || (cmd_val <= 0))
		return -EINVAL;
	if(find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;

	bp_iface = bp_seg->bp_iface;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->period_addr)
		bp_client.addr = bp_seg->period_addr;

	if (get_all_iface_sem(bp_iface))
		return -ERESTARTSYS;

	for (i = 0; i < bp_iface->seg_no; i++){
		if (bp_iface->seg[i].auto_trigger_interval != 0){
			printk(KERN_WARNING "Bypass [%#x]: Please stop auto-trigger before the operation\n", 
					bp_seg->ctrl_addr);
			releas_all_iface_sem(bp_iface);
			return -EPERM;
		}
	}

	bypass_period_write(&bp_client, bp_seg, cmd_val);

	releas_all_iface_sem(bp_iface);

	return 0;
}

int bypass_get_fw_ver (struct i2c_client* client, int index)
{
	struct i2c_client bp_client;
	bp_interface *bp_iface;
	bp_seg *bp_seg;
	int period;
	int i;

	if(find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;
	bp_iface = bp_seg->bp_iface;

	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->period_addr)
		bp_client.addr = bp_seg->period_addr;

	/* Check if firmware value has been captured */
	if(bp_iface->fw_version == 0x0){
		if (get_all_iface_sem(bp_iface))
			return -ERESTARTSYS;

		for (i = 0; i < bp_iface->seg_no; i++){
			if (bp_iface->seg[i].auto_trigger_interval != 0){
				printk(KERN_WARNING "Bypass [%#x]: Please stop auto-trigger before the operation\n", bp_seg->ctrl_addr);
				releas_all_iface_sem(bp_iface);
				return -EPERM;
			}
		}

		/* Storage period value */
		period = bypass_period_read(&bp_client, bp_seg);

		/* Get firmware version */
		switch(bp_seg->type){
			case BP_FIBER_PIC_ABN482 :
			case BP_FIBER_PIC_ABN1004 :
				bp_iface->fw_version = bypass_check_cpld_ver_ABN482(&bp_client);
				break;
			case BP_COPPER_CPLDV2 :
				bp_iface->fw_version = bypass_check_cpld_ver_ABN484(&bp_client, bp_seg->firmware_ver_addr);
				break;
		}

		/* Set fw ver to 0xff if fw is old version. */
		if(bp_iface->fw_version == 0x0 || bp_iface->fw_version == 0xff)
			bp_iface->fw_version = 0xff;

		/* Set back the period */
		bypass_period_write(&bp_client, bp_seg, period);

		releas_all_iface_sem(bp_iface);
	}

	return bp_iface->fw_version;
}

static void auto_trigger_timeout(unsigned long seg_addr)
{
	bp_seg* seg = (bp_seg *) seg_addr;
	bypass_pci_set_wdt(&seg->bp_iface->pci_ctrl, seg);
	mod_timer(&seg->trigger_timer, jiffies + seg->auto_trigger_interval);
}

int bypass_get_auto_trigger(struct i2c_client* client, int index)
{
	bp_seg *bp_seg;
	if (find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;

	return jiffies_to_msecs(bp_seg->auto_trigger_interval)/10;
}

int bypass_set_auto_trigger(struct i2c_client* client, int index, int cmd_val)
{
	struct i2c_client bp_client;
	bp_interface *bp_iface;
	bp_seg *bp_seg;
	unsigned long interval = 0;

	if (find_client_bp_seg(&bp_seg, client, index))
		return -ENXIO;

	bp_iface = bp_seg->bp_iface;

	if(!bp_iface->pci_ctrl.iodata)
		return -EPERM;

	/*Init data struct to read period*/
	memcpy(&bp_client, client, sizeof(struct i2c_client));
	if (bp_seg->period_addr)
		bp_client.addr = bp_seg->period_addr;	

	if (get_all_iface_sem(bp_iface))
		return -ERESTARTSYS;

	/*unit of cmd_val is 1/100 sec*/
	if (cmd_val >= bypass_period_read(&bp_client, bp_seg) * 100 || cmd_val < 0){
		printk(KERN_WARNING "Bypass: I2C %#x invalid input or period value not set\n", client->addr);
		releas_all_iface_sem(bp_iface);
		return -EINVAL;
	}

	if (cmd_val == 0){
		del_timer_sync(&bp_seg->trigger_timer);
	}else{
		interval =  msecs_to_jiffies(cmd_val * 10);

		if (bp_seg->auto_trigger_interval > 0){
			bypass_pci_set_wdt(&bp_seg->bp_iface->pci_ctrl, bp_seg);
			mod_timer(&bp_seg->trigger_timer, jiffies + interval);
		}else{
			bp_seg->trigger_timer.data = (unsigned long) bp_seg;	
			bp_seg->trigger_timer.function = auto_trigger_timeout;
			bp_seg->trigger_timer.expires = jiffies + interval;
			add_timer(&bp_seg->trigger_timer);
		}
	}

	bp_seg->auto_trigger_interval = interval;

	releas_all_iface_sem(bp_iface);

	return 0;
}

int bypass_get_txdisable(struct i2c_client* client, int index)
{
	bp_seg *bp_seg;
	unsigned long cmd_mask;
	int txdis_sts = 0;

	if (find_client_bp_seg(&bp_seg, client, 0))
		return -EINVAL;

	if (bp_seg->name != BP_CARD_ABN482)
		return -EPERM;

	cmd_mask = (index ? FIBER_ABN482_TX_DISABLE1 : FIBER_ABN482_TX_DISABLE0);

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	txdis_sts = ((i2c_smbus_read_byte_data(client, PCA9555_OUTPUT_1) & cmd_mask) ? 1:0);

	up(&bp_seg->bp_sem);

	return txdis_sts;
}

int bypass_set_txdisable(struct i2c_client* client, int index, int cmd_val)
{
	bp_seg *bp_seg;
	unsigned long cmd_mask;
	int tmpval;

	if (find_client_bp_seg(&bp_seg, client, 0))
		return -EINVAL;

	if (bp_seg->name != BP_CARD_ABN482)
		return -EPERM;

	cmd_mask = (index ? FIBER_ABN482_TX_DISABLE1 : FIBER_ABN482_TX_DISABLE0 );

	if (down_interruptible(&bp_seg->bp_sem))
		return -ERESTARTSYS;

	tmpval = i2c_smbus_read_byte_data(client , PCA9555_OUTPUT_1);

	if ( cmd_val == 1)
		i2c_smbus_write_byte_data(client, PCA9555_OUTPUT_1, (tmpval | cmd_mask));
	else if ( cmd_val == 0 )
		i2c_smbus_write_byte_data(client, PCA9555_OUTPUT_1, (tmpval & ~cmd_mask));

	up(&bp_seg->bp_sem);

	return 0;
}
