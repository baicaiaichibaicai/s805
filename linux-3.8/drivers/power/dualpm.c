#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/notifier.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/jiffies.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <future/log/logevent.h>
#include <future/general.h>
#include <linux/syscalls.h>
#include <asm-generic/termbits.h>
#include <asm-generic/ioctls.h>
#include <linux/kthread.h>
#include "lanner_psu.h"

int thread_dualpm(void *dump);
void system_log(u32 errer, char *desc);

int smbase, smb_err, ich_iobase, lanner_psu;
unsigned char pwr_6k_addr[3] = {0x58, 0x59, 0x5F};
unsigned char pwr_addr[2]  = {0xB0, 0xB2};
unsigned char WB_00_pec[2] = {0xEA, 0x3C};
unsigned char WB_20_pec[2] = {0x0A, 0xDC};
unsigned char WB_23_pec[2] = {0x03, 0xD5};

static int psu_enable, power_flag, temper_flag[2], mfr_flag = 0;
static int psu_addr, psu_offset, temper_offset, temper_div;
static unsigned char pwr1_mask = 0x00, pwr2_mask = 0x01;
/* get information of PSU 6K device */
static unsigned char mfr_id[MODULE_NUM][I2C_SMBUS_BLOCK_MAX +1];
static unsigned char mfr_model[MODULE_NUM][I2C_SMBUS_BLOCK_MAX +1];
static unsigned char mfr_revision[MODULE_NUM][I2C_SMBUS_BLOCK_MAX +1];
static unsigned char mfr_model_option[MODULE_NUM][I2C_SMBUS_BLOCK_MAX +1];
static unsigned char mfr_fw_id[MODULE_NUM][I2C_SMBUS_BLOCK_MAX +1];
static unsigned char mfr_fw_revision[MODULE_NUM][I2C_SMBUS_BLOCK_MAX +1];
static unsigned char mfr_fw_date[MODULE_NUM][I2C_SMBUS_BLOCK_MAX +1];
/* get firmware ID of PSU 3K devide */
static unsigned char mfr_fw_3k_id[FW_3K_ID_LEN+1];

extern struct i2c_adapter *i2c_get_adapter(int nr);

int lanner_psu_thread(void *arg)
{
	struct __wait_queue_head msleepq;
	unsigned int psu_word;
	unsigned char pwr, new_data=0x00, last_data=0x03;

	init_waitqueue_head(&msleepq);

	smb_err = 0x00;
	smbase = lanner_psu_get_smbase();
	ich_iobase = lanner_psu_get_gpiobase()&0xfffe;
	outportb((ich_iobase+0x32), (inportb(ich_iobase+0x32)|0x40));
	outportb((ich_iobase+0x36), (inportb(ich_iobase+0x36)&~0x40));
	outportb((ich_iobase+0x3A), (inportb(ich_iobase+0x3A)&~0x40));
	delay(100);
	
	psu_word = lanner_psu_read_word(pwr_addr[0], STATUS_WORD);
	if(psu_word)
		return -1;

	psu_enable = 0x03;
	while(1) {
		interruptible_sleep_on_timeout(&msleepq, HZ*2);

		if(psu_enable != 0x03)
			continue;

		for(pwr=0;pwr<2;pwr++) {
			psu_word = lanner_psu_read_word(pwr_addr[pwr], STATUS_WORD);
			if(!(psu_word&0x800))
				new_data |= (0x01<<pwr);
			else
				new_data |= (0x00<<pwr);

			lanner_psu_write_byte(pwr_addr[pwr], 0x00, 0x20);
			psu_word = lanner_psu_read_word(pwr_addr[pwr], READ_TEMPTURE_1);
			temper_flag[pwr] = lanner_psu_value_convert(psu_word); 
		}

		if((last_data&(0x01<<pwr1_mask)) && !(new_data&(0x01<<pwr1_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Down");
		else if(!(last_data&(0x01<<pwr1_mask)) && (new_data&(0x01<<pwr1_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Up");
		if((last_data&(0x01<<pwr2_mask)) && !(new_data&(0x01<<pwr2_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Down");
		else if(!(last_data&(0x01<<pwr2_mask)) && (new_data&(0x01<<pwr2_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Up");

		last_data = new_data;
		power_flag = new_data;
		new_data = 0x00;
	}
	outportb((ich_iobase+0x3A), (inportb(ich_iobase+0x3A)|0x40));
}

int lanner_psu_pec_thread(void *arg)
{
	struct __wait_queue_head msleepq;
	unsigned int psu_word;
	unsigned char pwr, new_data=0x00, last_data=0x03;

	init_waitqueue_head(&msleepq);

	smb_err = 0x00;
	smbase = lanner_psu_get_smbase();
		
	psu_word = lanner_psu_read_word(pwr_addr[0], STATUS_WORD);
	if(psu_word)
		return -1;

	psu_enable = 0x03;
	while(1) {
		interruptible_sleep_on_timeout(&msleepq, HZ*2);

		if(psu_enable != 0x03)
			continue;

		for(pwr=0;pwr<2;pwr++) {
			psu_word = lanner_psu_read_word(pwr_addr[pwr], STATUS_WORD);
			if(!(psu_word&0x800))
				new_data |= (0x01<<pwr);
			else
				new_data |= (0x00<<pwr);

			lanner_psu_write_byte_pec(pwr_addr[pwr], 0x00, 0x23, WB_23_pec[pwr]);
			psu_word = lanner_psu_read_word(pwr_addr[pwr], READ_TEMPTURE_1);
			temper_flag[pwr] = lanner_psu_value_convert(psu_word);
		}

		if((last_data&(0x01<<pwr1_mask)) && !(new_data&(0x01<<pwr1_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Down");
		else if(!(last_data&(0x01<<pwr1_mask)) && (new_data&(0x01<<pwr1_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Up");
		if((last_data&(0x01<<pwr2_mask)) && !(new_data&(0x01<<pwr2_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Down");
		else if(!(last_data&(0x01<<pwr2_mask)) && (new_data&(0x01<<pwr2_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Up");

		last_data = new_data;
		power_flag = new_data;
		new_data = 0x00;
	}
}

int lanner_i2c_thread(void *arg)
{
	struct __wait_queue_head 	msleepq;
	struct i2c_adapter *adap;
	union i2c_smbus_data psu_data;
	unsigned char pwr, new_data = 0x00, last_data = 0x00;

	init_waitqueue_head(&msleepq);

	adap = i2c_get_adapter(0);
	if (!adap)
		return -1;

	for(pwr=0;pwr<2;pwr++) {
		if(i2c_smbus_xfer(adap, psu_addr+pwr, 0, I2C_SMBUS_READ, psu_offset, I2C_SMBUS_WORD_DATA, &psu_data))
			return -1;
		if(!(psu_data.word&0x0800))
			last_data |= (0x01<<pwr);
		else
			last_data |= (0x00<<pwr);
	}

	psu_enable = 0x03;
	while(1) {
		interruptible_sleep_on_timeout(&msleepq, HZ*2);

		if(psu_enable != 0x03)
			continue;

		for(pwr=0;pwr<2;pwr++) {
			i2c_smbus_xfer(adap, psu_addr+pwr, 0, I2C_SMBUS_READ, psu_offset, I2C_SMBUS_WORD_DATA, &psu_data);
			if(!(psu_data.word&0x0800))
				new_data |= (0x01<<pwr);
			else
				new_data |= (0x00<<pwr);

			if(temper_div) {
				i2c_smbus_xfer(adap, psu_addr+pwr, 0, I2C_SMBUS_READ, temper_offset+pwr, I2C_SMBUS_WORD_DATA, &psu_data);
				temper_flag[pwr] = lanner_psu_value_convert(psu_data.word);
			}
		}

		if((new_data&(0x01<<pwr1_mask)) && !(last_data&(0x01<<pwr1_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Up");
		else if(!(new_data&(0x01<<pwr1_mask)) && (last_data&(0x01<<pwr1_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Down");
		if((new_data&(0x01<<pwr2_mask)) && !(last_data&(0x01<<pwr2_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Up");
		else if(!(new_data&(0x01<<pwr2_mask)) && (last_data&(0x01<<pwr2_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Down");

		last_data  = new_data;
		power_flag = new_data;
		new_data = 0x00;
	}
}

int zen6k_i2c_thread(void *arg)
{
    struct __wait_queue_head 	msleepq;
    struct i2c_adapter *adap;
    union i2c_smbus_data psu_data, data;
	unsigned char pwr, new_data = 0x00, last_data = 0x00;
    
    init_waitqueue_head(&msleepq);
	mfr_flag = MFR_IS_ZEN6K;

    adap = i2c_get_adapter(0);
	if (!adap)
		return -1;

	for(pwr=0;pwr<2;pwr++) {
		if(i2c_smbus_xfer(adap, psu_addr+pwr, 0, I2C_SMBUS_READ, psu_offset, I2C_SMBUS_WORD_DATA, &psu_data))
			return -1;
		/* 
		 * CAR4020에서 psu_data.word 값이 간헐적으로 0xf8XX로 출력이 됨.
		 * 데이터 시트에서는 POWER_GOOD# 플래그가 상위 3번 비트 (0x0800)으로 정의
		 * 따라서 간헐적으로 0xf8XX의 값을 출력받게 되면 Power가 down됬다고 인식.
		 * 실제로 파워를 제거하였을 때, psu_data.word값은 0x2848을 나타냄.
		 */
		if(psu_data.word == 0x2848)
			last_data |= (0x00<<pwr);
		else
			last_data |= (0x01<<pwr);
	}

	psu_enable = 0x03;
    while(1) {
		interruptible_sleep_on_timeout(&msleepq, HZ*2);

		if(psu_enable != 0x03)
			continue;

		for(pwr = 0;pwr < MODULE_NUM ; pwr++) {
				i2c_smbus_xfer(adap, pwr_6k_addr[pwr], 0, I2C_SMBUS_READ, MFR_ID, I2C_SMBUS_BLOCK_DATA, &data);
				memset(mfr_id[pwr],0,I2C_SMBUS_BLOCK_MAX + 1);
				memcpy(mfr_id[pwr],&data.block[1],data.block[0]);

				i2c_smbus_xfer(adap, pwr_6k_addr[pwr], 0, I2C_SMBUS_READ, MFR_MODEL, I2C_SMBUS_BLOCK_DATA, &data);
				memset(mfr_model[pwr],0,I2C_SMBUS_BLOCK_MAX + 1);
				memcpy(mfr_model[pwr],&data.block[1],data.block[0]);

				i2c_smbus_xfer(adap, pwr_6k_addr[pwr], 0, I2C_SMBUS_READ, MFR_REVISION, I2C_SMBUS_BLOCK_DATA, &data);
				memset(mfr_revision[pwr],0,I2C_SMBUS_BLOCK_MAX + 1);
				memcpy(mfr_revision[pwr],&data.block[1],data.block[0]);

				i2c_smbus_xfer(adap, pwr_6k_addr[pwr], 0, I2C_SMBUS_READ, MFR_MODEL_OPTION, I2C_SMBUS_BLOCK_DATA, &data);
				memset(mfr_model_option[pwr],0,I2C_SMBUS_BLOCK_MAX + 1);
				memcpy(mfr_model_option[pwr],&data.block[1],data.block[0]);

				i2c_smbus_xfer(adap, pwr_6k_addr[pwr], 0, I2C_SMBUS_READ, MFR_FW_ID, I2C_SMBUS_BLOCK_DATA, &data);
				memset(mfr_fw_id[pwr],0,I2C_SMBUS_BLOCK_MAX + 1);
				memcpy(mfr_fw_id[pwr],&data.block[1],data.block[0]);

				i2c_smbus_xfer(adap, pwr_6k_addr[pwr], 0, I2C_SMBUS_READ, MFR_FW_REVISION, I2C_SMBUS_BLOCK_DATA, &data);
				memset(mfr_fw_revision[pwr],0,I2C_SMBUS_BLOCK_MAX + 1);
				memcpy(mfr_fw_revision[pwr],&data.block[1],data.block[0]);

				i2c_smbus_xfer(adap, pwr_6k_addr[pwr], 0, I2C_SMBUS_READ, MFR_FW_DATE, I2C_SMBUS_BLOCK_DATA, &data);
				memset(mfr_fw_date[pwr],0,I2C_SMBUS_BLOCK_MAX + 1);
				memcpy(mfr_fw_date[pwr],&data.block[1],data.block[0]);
				
			}

		for(pwr=0;pwr<2;pwr++) {
			i2c_smbus_xfer(adap, psu_addr+pwr, 0, I2C_SMBUS_READ, psu_offset, I2C_SMBUS_WORD_DATA, &psu_data);
			if(psu_data.word == 0x2848)
				new_data |= (0x00<<pwr);
			else
				new_data |= (0x01<<pwr);

			if(temper_div) {
				i2c_smbus_xfer(adap, psu_addr+pwr, 0, I2C_SMBUS_READ, temper_offset, I2C_SMBUS_WORD_DATA, &psu_data);
				temper_flag[pwr] = psu_data.word&0x7ff;
			}
		}

		if((new_data&(0x01<<pwr1_mask)) && !(last_data&(0x01<<pwr1_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Up");
		else if(!(new_data&(0x01<<pwr1_mask)) && (last_data&(0x01<<pwr1_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Down");
		if((new_data&(0x01<<pwr2_mask)) && !(last_data&(0x01<<pwr2_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Up");
		else if(!(new_data&(0x01<<pwr2_mask)) && (last_data&(0x01<<pwr2_mask)))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Down");

		last_data  = new_data;
		power_flag = new_data;
		new_data = 0x00;
	}
}

int zen3k_i2c_thread(void *arg)
{
    struct __wait_queue_head 	msleepq;
    struct i2c_adapter *adap;
    union i2c_smbus_data new_data,data;
    union i2c_smbus_data last_data;
	uint8_t pwr;
    
    init_waitqueue_head(&msleepq);
	mfr_flag = MFR_IS_ZEN3K;

    adap = i2c_get_adapter(0);
	if (!adap)
		return -1;

    if(i2c_smbus_xfer(adap, psu_addr, 0, I2C_SMBUS_READ, psu_offset, I2C_SMBUS_BYTE_DATA, &last_data))
		return -1;

    psu_enable = 0x03;
    while(1) {
		interruptible_sleep_on_timeout(&msleepq, HZ*2);

		if(psu_enable != 0x03)
			continue;

		memset(mfr_fw_3k_id,0,FW_3K_ID_LEN+1);
		for (pwr= 0; pwr < FW_3K_ID_LEN; pwr++)
		{
			i2c_smbus_xfer(adap, MFR_3K_FW_ID, 0 , I2C_SMBUS_READ, pwr + FW_3K_ID_START , I2C_SMBUS_BYTE_DATA, &data);
			mfr_fw_3k_id[pwr] = data.byte;
		}

		i2c_smbus_xfer(adap, psu_addr, 0, I2C_SMBUS_READ, psu_offset, I2C_SMBUS_BYTE_DATA, &new_data);

		if((last_data.byte&0x02) && !(new_data.byte&0x02))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Up");
		else if(!(last_data.byte&0x02) && (new_data.byte&0x02))
			system_log(LOG_SYSTEM_STATUS, "Power[1] : Down");
		if((last_data.byte&0x04) && !(new_data.byte&0x04))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Up");
		else if(!(last_data.byte&0x04) && (new_data.byte&0x04))
			system_log(LOG_SYSTEM_STATUS, "Power[2] : Down");

		last_data = new_data;
		power_flag = ~(new_data.byte>>1);
	}
}

void show_power_info(struct vty *vty)
{
    if(psu_enable != 0x03){
		debug(1, "\n\nPower Monitor Module is not Support or Disabled\n");
		return;
	}

	vty_out(vty, "\nPower Supply#1 is %s\n", (power_flag&(0x01<<pwr1_mask))? "present" : "not present");
	vty_out(vty, "Power Supply#2 is %s\n", (power_flag&(0x01<<pwr2_mask))? "present" : "not present");
	if(!temper_div) {
		vty_out(vty, "\nPower Supply doesn't support temperature information\n\n");
		return;
	}

	vty_out(vty, "\nPower Supply#1 temperature is %dC\n", temper_flag[pwr1_mask]/temper_div);
	vty_out(vty, "Power Supply#2 temperature is %dC\n\n", temper_flag[pwr2_mask]/temper_div);
}
EXPORT_SYMBOL(show_power_info);

static int dualpm_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int offset = 0;

    offset += snprintf(page+offset, count-offset, " Power Monitor is %s\n", (psu_enable==0x03)? "running" : "stopped");
	if(psu_enable == 0x03) {
		offset += snprintf(page+offset, count-offset, "Power #1 : %s\n", (power_flag&(0x01<<pwr1_mask))? "on" : "off");
		offset += snprintf(page+offset, count-offset, "Power #2 : %s\n", (power_flag&(0x01<<pwr2_mask))? "on" : "off");
	}

    return offset;
}

static int mfr_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int offset = 0;
	uint8_t pwr;

	if(psu_enable == 0x03 && mfr_flag != 0) {
		// show information of PSU device
		if (mfr_flag == MFR_IS_ZEN3K){
			offset += snprintf(page+offset, count-offset, "\tMFR_FW_ID: %s\n",mfr_fw_3k_id);
		}

		if (mfr_flag == MFR_IS_ZEN6K){
			for (pwr=0; pwr < MODULE_NUM; pwr++) {
				if (pwr == MODULE_NUM-1)
					offset += snprintf(page+offset, count-offset, "Backplane:\n");
				else
					offset += snprintf(page+offset, count-offset, "Power #%d\n",pwr);
				offset += snprintf(page+offset, count-offset, "\tMFR_ID: %s\n",mfr_id[pwr]);
				offset += snprintf(page+offset, count-offset, "\tMFR_MODEL: %s\n",mfr_model[pwr]);
				offset += snprintf(page+offset, count-offset, "\tMFR_REVISION: %s\n",mfr_revision[pwr]);
				offset += snprintf(page+offset, count-offset, "\tMFR_MODEL_OPTION: %s\n",mfr_model_option[pwr]);
				offset += snprintf(page+offset, count-offset, "\tMFR_FW_ID: %s\n",mfr_fw_id[pwr]);
				offset += snprintf(page+offset, count-offset, "\tMFR_FW_REVISION: %s\n",mfr_fw_revision[pwr]);
				offset += snprintf(page+offset, count-offset, "\tMFR_FW_DATE: %s\n",mfr_fw_date[pwr]);
			}
		}
	}


    return offset;
}

static int dualpm_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
    char tmp[20] = {0};

    if(count)
		return count;

    copy_from_user(tmp, buffer, count);

    if(tmp[0] == '0')
		psu_enable &= ~0x01;
    else
		psu_enable |= 0x01;

    return count;
}

int thread_dualpm(void *arg)
{
    extern int end_of_boot;
    static struct task_struct *zen_psu_task;

    while (!end_of_boot) {
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ/2);
    }

    if (!strncmp(bootserial, "WZN301R", 7)) {
		psu_addr 	  = 0x5A;
		psu_offset    = 0x01;
		temper_div	  = 0;
		zen_psu_task = kthread_run(zen3k_i2c_thread, NULL, "zen_psu");
		if (IS_ERR(zen_psu_task))
			printk("zen_psu start error. \n");
	}
	else if(!strncmp(bootserial, "WZN601", 6)) {
		psu_addr	  = 0x58;
		psu_offset    = 0x79;
		temper_offset = 0x8D;
		temper_div    = 2;
		zen_psu_task = kthread_run(zen6k_i2c_thread, NULL, "zen_psu");
		if (IS_ERR(zen_psu_task))
			printk("zen_psu start error. \n");
	}
	else if(!strncmp(bootserial, "WZN401", 6) ||
			!strncmp(bootserial, "WZN501", 6)) {
		pwr1_mask  = 0x01;
		pwr2_mask  = 0x00;
		temper_div = 1000;
		zen_psu_task = kthread_run(lanner_psu_thread, NULL, "zen_psu");
		if (IS_ERR(zen_psu_task))
			printk("zen_psu start error. \n");
	}
	else if(!strncmp(bootserial, "WZN701", 6) ||
			!strncmp(bootserial, "WZN801", 6) ||
			!strncmp(bootserial, "WZN102", 6) ||
			!strncmp(bootserial, "WZN202", 6)) {
		psu_addr	  = 0x58;
		psu_offset    = 0x79;
		temper_offset = 0x8D;
		pwr1_mask     = 0x01;
		pwr2_mask     = 0x00;
		if(!strncmp(bootserial, "WZN102", 6)) {
			pwr1_mask = 0x00;
			pwr2_mask = 0x01;
		}
		temper_div    = 1000;
		zen_psu_task = kthread_run(lanner_i2c_thread, NULL, "zen_psu");
		if (IS_ERR(zen_psu_task))
			printk("zen_psu start error. \n");
	}
	return 0;
}

extern struct proc_dir_entry *proc_ferret_system;

static int dualpm_proc_init(void)
{
    struct proc_dir_entry *p = NULL;
    struct proc_dir_entry *mfr_info = NULL;
    static struct task_struct *pm_task;

    p = create_proc_entry("power_info", 0, proc_ferret_system);
    if (p == NULL) {
		printk("Creating proc entry failed : /proc/ferret/system/power_info\n");
		return -ENOMEM;
	}

    p->read_proc = dualpm_proc_read;
    p->write_proc = dualpm_proc_write;

    mfr_info = create_proc_entry("mfr_info", 0, proc_ferret_system);
    if ( mfr_info == NULL) {
		printk("Creating proc entry failed : /proc/ferret/system/mfr_info\n");
		return -ENOMEM;
	}
    mfr_info->read_proc = mfr_proc_read;

    pm_task = kthread_run(thread_dualpm, NULL, "pmthreadd");
    if (IS_ERR(pm_task))
		printk("pmthreadd start error. \n");
    return 0;
}

module_init(dualpm_proc_init);
