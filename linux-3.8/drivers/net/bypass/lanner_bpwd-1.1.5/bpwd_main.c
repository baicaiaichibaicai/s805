/*******************************************************************************

  bpwd_main.c : Linux driver for Lanner bypass/watchdog module access code

  Lanner bypass/watchdog module access code
  Copyright(c) 2010 Lanner Electronics Inc.

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

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

*******************************************************************************/


/* Standard in kernel modules */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>
#ifndef CONFIG_FERRET_NETWORK_BYPASS
#include "../include/bpwd_ioctl.h"
#include "../include/ioaccess.h"
#include "../include/version.h"
#else
#include <linux/kthread.h>
#include <future/net/iface_array.h>
#include "include/bpwd_ioctl.h"
#include "include/ioaccess.h"
#include "include/version.h"
#endif

/*
 * Device Major Number
 */
#define BPWD_MAJOR 247 
/*
 * IO spcae size
 */
#define SMBUS_IO_SIZE 0x20

/*
 * Add support for linux kernel over 2.6.36
 */
#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#else
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif

/*
 * Add for AMD
 */
#define PCI_INDEX_PORT	0xCF8
#define PCI_DATA_PORT	0xCFC
#define PCIADDR		0x80000000

#define PMIO_INDEX_PORT	0xCD6
#define PMIO_DATA_PORT	0xCD7
#define SmBus0En	0x2C


#ifdef CONFIG_FERRET_NETWORK_BYPASS
#define LANNER_BPWD_1_2_1	1
extern int g_wd_timeout;
extern int zen_thread_bypass(void *dummy);
#endif

#ifdef CONFIG_FERRET
extern int use_e1000_port;
static int pk=1;        
static int cntbp=0;
#endif
/*
 * Is the device opened right now?
 * Prevent to access the device in the same time
 */
static int Device_Open = 0;
static struct pci_dev *smbus_dev;
static u16 io_base;

//static DEFINE_SPINLOCK(bpwd_lock);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
static int bpwd_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
#else
static long bpwd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
#endif
{
#if defined(OLDKERNEL)
	char *up =(void *)arg;
#else
	char __user *up = (void __user *)arg;
#endif
	bp_s bp; 
	

	if ( copy_from_user(&bp, up, sizeof(bp_s)))
		return -EFAULT;
//printk("Before addr= %x cmd= %x, bp_buffer=%x %x, pec=%x\n", bp.bp_addr, bp.bp_cmd, bp.bp_buffer[0], bp.bp_buffer[1], bp.bp_pec);
	switch(cmd)
	{
#ifndef CONFIG_FERRET_NETWORK_BYPASS
		case IOCTL_READ_BYTE_DATA_CMD:
			if (i2c_smbus_read_byte_data(io_base, bp.bp_addr, bp.bp_cmd, 
						&bp.bp_buffer[0], bp.bp_inhibit_err))
                		return -EFAULT;
			break;
		case IOCTL_READ_WORD_DATA_CMD:
			if (i2c_smbus_read_word_data(io_base, bp.bp_addr, bp.bp_cmd, 
							(unsigned short*)&bp.bp_buffer[0]))
                		return -EFAULT;
			break;
		case IOCTL_WRITE_BYTE_CMD:
			if (i2c_smbus_write_byte(io_base, bp.bp_addr, bp.bp_cmd, 1,
							 &bp.bp_pec))
                		return -EFAULT;
			break;
		case IOCTL_WRITE_BYTE_DATA_CMD:
			if (i2c_smbus_write_byte_data(io_base, bp.bp_addr, bp.bp_cmd,
							bp.bp_buffer[0]))
                		return -EFAULT;
			break;
		case IOCTL_READ_BLOCK_DATA_CMD:
			printk("Start read block\n");
			if (i2c_smbus_read_block_data(io_base, bp.bp_addr, bp.bp_cmd, 
							bp.bp_buffer, &bp.bp_pec))
                		return -EFAULT;
			break;
		case IOCTL_WRITE_BLOCK_DATA_CMD:
			if (i2c_smbus_write_block_data(io_base, bp.bp_addr, bp.bp_cmd, 
							bp.bp_buffer, &bp.bp_pec))
                		return -EFAULT;
			break;
#else
		case IOCTL_READ_BYTE_DATA_CMD:
			if (lanner_bypass_read_byte_data(io_base, bp.bp_addr, bp.bp_cmd, 
						&bp.bp_buffer[0], bp.bp_inhibit_err))
                		return -EFAULT;
			break;
		case IOCTL_READ_WORD_DATA_CMD:
			if (lanner_bypass_read_word_data(io_base, bp.bp_addr, bp.bp_cmd, 
							(unsigned short*)&bp.bp_buffer[0]))
                		return -EFAULT;
			break;
		case IOCTL_WRITE_BYTE_CMD:
			if (lanner_bypass_write_byte(io_base, bp.bp_addr, bp.bp_cmd, 1,
							 &bp.bp_pec))
                		return -EFAULT;
			break;
		case IOCTL_WRITE_BYTE_DATA_CMD:
			if (lanner_bypass_write_byte_data(io_base, bp.bp_addr, bp.bp_cmd,
							bp.bp_buffer[0]))
                		return -EFAULT;
			break;
		case IOCTL_READ_BLOCK_DATA_CMD:
			printk("Start read block\n");
			if (lanner_bypass_read_block_data(io_base, bp.bp_addr, bp.bp_cmd, 
							bp.bp_buffer, &bp.bp_pec))
                		return -EFAULT;
			break;
		case IOCTL_WRITE_BLOCK_DATA_CMD:
			if (lanner_bypass_write_block_data(io_base, bp.bp_addr, bp.bp_cmd, 
							bp.bp_buffer, &bp.bp_pec))
                		return -EFAULT;
			break;
#endif
		default:
			return -EFAULT;
	}
//printk("After addr= %x cmd= %x, bp_buffer=%x %x, pec=%x\n", bp.bp_addr, bp.bp_cmd, bp.bp_buffer[0], bp.bp_buffer[1], bp.bp_pec);
	if (copy_to_user(up, &bp, sizeof(bp_s)))
{
		printk("copy error\n");
		return -EFAULT;
}
	else
		return 0;
}

/*
 * This function is called whenever a process attempts to
 * open the device file
 */
static int bpwd_open(struct inode * inode, struct file * file)
{
	/* we don't want to talk to two processes at the same time */
	if(Device_Open) return -EBUSY;
	Device_Open++;
	/* Make sure that the module isn't removed while the file
	 * is open by incrementing the usage count (the number of
	 * opened references to the module,if it's zero emmod will
	 * fail)
	 */
//	printk("Lanner Watchdog Driver Opened\n");
	return 0;
}

/*
 * This function is called when a process closes the device file.
 */
static int bpwd_release(struct inode * inode, struct file * file)
{
	/* ready for next caller */
	Device_Open--;
	/* Decrement the usage count, otherwise once you opened the file
	 * you'll never get rid of the module.
	 */
//	printk("Lanner Watchdog Driver Closed\n");
	return 0;
}

/*
 * This structure will hold the functions to be called
 * when a process does something to the device we created.
 * Since a pointer to this structure is kept in the
 * devices table, it can't be local to init_module.
 * NULL is for unimplemented functions.
 */
#if defined(OLDKERNEL)
static struct file_operations bpwd_fops = {
	owner:		THIS_MODULE,
	read:		NULL,
	write:		NULL,
	ioctl:		bpwd_ioctl,
	open:		bpwd_open,
	release:	bpwd_release,
};
#else
static const struct file_operations bpwd_fops = {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
	.ioctl		= bpwd_ioctl,
#else
	.unlocked_ioctl		= bpwd_ioctl,
#endif
	.open		= bpwd_open,
	.release	= bpwd_release,
};
#endif

static int probe_device(void)
{
	int err=0,ret_value=0;
#ifndef CONFIG_FERRET_NETWORK_BYPASS
	unsigned long addr, data_tmp, amd_base;
#else
	/* amd_base inititalize */
	unsigned long addr, data_tmp, amd_base = 0x00;
#endif
	unsigned char tmp,ret_val;

	addr = (PCIADDR) | (0x14 << 3 <<8) | (0x00 << 8 | 0x00);
	outportl(PCI_INDEX_PORT,addr);
	data_tmp=inportl(PCI_DATA_PORT);
	printk("AMD Vendor ID: 0x%lx\n",data_tmp);
	if(data_tmp == 0x43851002)
	{
		printk("Match vendor ID !!\n");
		//Get AMD smbus base address
		outportb(PMIO_INDEX_PORT, SmBus0En+1);
		tmp=inportb(PMIO_DATA_PORT);
		amd_base = (amd_base) | tmp;
		outportb(PMIO_INDEX_PORT, SmBus0En+2);
		tmp=inportb(PMIO_DATA_PORT);
		amd_base = (amd_base << 4) | tmp;
		outportb(PMIO_INDEX_PORT, SmBus0En+3);
		tmp=inportb(PMIO_DATA_PORT);
		amd_base = (amd_base << 4) | tmp;
		//printk("Base address %d : %x\n",i,tmp);
		//printk("AMD Base Address: 0x%lx\n",amd_base);
		io_base = amd_base;
#ifndef CONFIG_FERRET_NETWORK_BYPASS
		ret_value = i2c_smbus_read_byte_data(io_base, 0x37, 0,&ret_val,1);
#else
		ret_value = lanner_bypass_read_byte_data(io_base, 0x37, 0,&ret_val,1);
#endif
		if(ret_value == 0)
			return err;	
		//printk("Return Value : %d\n",ret_value);
		io_base = io_base | 0x20;
#ifndef CONFIG_FERRET_NETWORK_BYPASS
		ret_value = i2c_smbus_read_byte_data(io_base, 0x37, 0,&ret_val,1);
#else
		ret_value = lanner_bypass_read_byte_data(io_base, 0x37, 0,&ret_val,1);
#endif
		if(ret_value == 0)
			return err;
		err= -ENODEV;
		return err;
		//printk("Return Value : %d\n",ret_value);
		//printk("---AMD Base Address: 0x%x\n",io_base);
	}
	else
	{
#if defined(OLDKERNEL)
	smbus_dev = pci_find_slot(0, PCI_DEVFN(0x1F, 3));
#else
	smbus_dev = pci_get_bus_and_slot(0, PCI_DEVFN(0x1F, 3));
#endif
		printk("Probe SMBUS controller..");
		if (!smbus_dev) {
			printk("Cannot locate Intel SMBUS controller\n");
			err = -ENODEV;
			goto probe_exit;
		}

		/* enable pci device */
		err = pci_enable_device_io(smbus_dev);
        	if (err) {
                	printk("Failed to enable SMBus PCI device (%d)\n",err);
                	err= -EBUSY;
			goto probe_exit;
        	}

		/* Get IO resource */
		pci_read_config_word(smbus_dev, 0x20, &io_base);
		io_base &= 0xfffe;
		if ( !io_base ) {
			printk("No resource for SMBUS controller\n");
			err =  -ENODEV;
			goto probe_exit;
		} else {
			printk("Got SMBUS IO = 0x%x\n", io_base);
		}
	}
	if (!request_region(io_base, SMBUS_IO_SIZE, "lanner_bypass")) {
                printk("SMBus region 0x%x already in use! "
			" Try to unload i2c_i801 driver\n", io_base);
		err = -EBUSY;
		goto probe_exit;
	}

probe_exit:
#if !defined(OLDKERNEL)
	pci_dev_put(smbus_dev);
#endif
	
	return err;
}

int bpwd_init(void)
{
	/*
	 * Register the character device
	 */
	if(register_chrdev(BPWD_MAJOR, "bpwd_drv", &bpwd_fops))
	{
		printk("bpwd_drv : unable to get major %d\n", BPWD_MAJOR);
		return -EIO;
	}
	if ( probe_device()) {
		unregister_chrdev(BPWD_MAJOR, "bpwd_drv");
		return -EIO;
	}
	//spin_lock_init(&bpwd_lock);
	printk("Lanner Bypass/Watchdog Module Driver Version %s -- loaded\n", CODE_VERSION);
	return 0;
}

/*
 * Cleanup - unregister the appropriate file from /proc
 */
void bpwd_exit(void)
{
	/* Unregister the device */
	unregister_chrdev(BPWD_MAJOR, "bpwd_drv");
	/* release request region */
	release_region(io_base, SMBUS_IO_SIZE);
	/* If there's an error, report it */
	printk("Lanner Bypass/Watchdog Module Driver -- Unloaded\n");
}

#ifdef CONFIG_FERRET_NETWORK_BYPASS
late_initcall(bpwd_init);
#else
module_init(bpwd_init);
#endif
module_exit(bpwd_exit);

MODULE_AUTHOR("Lanner SW");
MODULE_DESCRIPTION("Lanner Bypass/Watchdog Module Driver");
MODULE_LICENSE("Dual BSD/GPL");

#ifdef CONFIG_FERRET_NETWORK_BYPASS
extern struct nic_slot_info g_nic_slot[];
extern struct _zen_bypass_info zen_bypass_info;
static unsigned long long card_conf = 0;
static unsigned char onboard_map[8] = {0, 1, 2, 3, 4, 5, 6, 7};
static unsigned char add_on_map[8] = {1, 2, 3, 4, 0, 0, 5, 6};
static int init_flag = 0;

typedef struct {
	char model[16];
	unsigned char switch_addr;
	unsigned char switch_data;
} switch_address_t;

#define MODEL_GET_THROUGH_NUM	5
switch_address_t switch_address_tbl[MODEL_GET_THROUGH_NUM+1] = {
	{ "MB-887X"   , 0x73, 0x00 },
	{ "MB-9655"   , 0x70, 0x00 },
	{ "MB-8895"   , 0x72, 0x00 },
#ifdef LANNER_BPWD_1_2_1
	{ "MB-8771EXT", 0x73, 0x00 },
	{ "MB-7583"   , 0x73, 0x00 },
#endif
	{ "", 0, 0 }
};

/* 
 * On-board bypass - ZEN3K, ZEN4K, ZEN5K
 * Add-on   bypass - ZEN7K, ZEN8K, ZEN10K, ZEN20K
 * On-board bypass access addr 0x37
 * Add-on   bypass access addr 0x30 (addr 0x30 ~ 0x37 are switched by this function)
 */
static int switch_address_add_on_mode(int slave_addr)
{
	int res = 0;
	switch_address_t *switch_ptr;
	unsigned char second_layer_addr;

	switch_ptr = switch_address_tbl;

	while(switch_ptr->switch_addr) {
		if(!strncmp(zen_bypass_info.board, switch_ptr->model, 16))
			break;
		switch_ptr++;
	}

	if(!strncmp(switch_ptr->model, "MB-887X", 7)) {
		if(slave_addr == 0x30)
			switch_ptr->switch_data = 0x01; // channel-01
		else if(slave_addr == 0x31)
			switch_ptr->switch_data = 0x02; // channel-02
		else if(slave_addr == 0x32)
			switch_ptr->switch_data = 0x04; // channel-03
		else if(slave_addr == 0x33)
			switch_ptr->switch_data = 0x08; // channel-04
		res = 1;
	}
	else if(!strncmp(switch_ptr->model, "MB-9655", 7)) {
		if(slave_addr == 0x30)
			switch_ptr->switch_data = 0x20; // channel-05
		else if(slave_addr == 0x31)
			switch_ptr->switch_data = 0x40; // channel-06
		else if(slave_addr == 0x32)
			switch_ptr->switch_data = 0x80; // channel-07
		res = 1;
	}
	else if(!strncmp(switch_ptr->model, "MB-8895", 7)) {
		if(slave_addr == 0x30)
			switch_ptr->switch_data = 0x01; // channel-01
		else if(slave_addr == 0x31)
			switch_ptr->switch_data = 0x02; // channel-02
		else if(slave_addr == 0x32)
			switch_ptr->switch_data = 0x04; // channel-03
		else if(slave_addr == 0x33)
			switch_ptr->switch_data = 0x08; // channel-04
		else if(slave_addr == 0x34)
			switch_ptr->switch_data = 0x10; // channel-05
		else if(slave_addr == 0x35)
			switch_ptr->switch_data = 0x20; // channel-06
		else if(slave_addr == 0x36)
			switch_ptr->switch_data = 0x40; // channel-07
		else if(slave_addr == 0x37)
			switch_ptr->switch_data = 0x80; // channel-08
		res = 1;
	}
#ifdef LANNER_BPWD_1_2_1
	else if(!strncmp(switch_ptr->model, "MB-8771EXT", 10)) {
		if((slave_addr == 0x30) || (slave_addr == 0x31))
			switch_ptr->switch_data = 0x01; // channel-01
		if(lanner_bypass_write_byte(io_base, switch_ptr->switch_addr,
					switch_ptr->switch_data, 0, NULL)) {
			printk("Switch address FAILED\n");
		}

		second_layer_addr=0x72; //PCA9545 address
		if(slave_addr == 0x30)
			switch_ptr->switch_data = 0x01; // channel-01
		else if(slave_addr == 0x31)
			switch_ptr->switch_data = 0x02; // channel-02
		if(lanner_bypass_write_byte(io_base, second_layer_addr,
					switch_ptr->switch_data, 0, NULL)) {
			printk("Switch address FAILED\n");
		}
		res = 1;
		return res;
	}
	else if(!strncmp(switch_ptr->model, "MB-7583", 7)) {
		if(slave_addr == 0x30) {
			switch_ptr->switch_data = 0x01;
		}
		res = 1;
	}
#endif
	if(lanner_bypass_write_byte(io_base, switch_ptr->switch_addr,
				switch_ptr->switch_data, 0, NULL)) {
		printk("Switch address FAILED\n");
	}
	return res;
}

static int get_num_of_bp_in_slot(int index)
{
#ifdef CONFIG_FERRET  
	int j,pj=0;
#endif
	if((index == 4) || (index == 5))
		return 0;
	if(!strncmp(zen_bypass_info.serial, "WZN401A", 7)) {
		if(index != 1)
			return 0;
#ifdef CONFIG_FERRET
		cntbp=6;
		for(j=0; j< (g_nic_slot[index].num_of_iface / 2 ) ; j++) {
			zen_bypass_info.allbp[cntbp]=g_nic_slot[index].element[pj]; 
			zen_bypass_info.allbp[cntbp+1]=g_nic_slot[index].element[pj+1];
			cntbp+=2; 
			pj+=2;
			pk++;
		}
#endif
		return g_nic_slot[index].bypass;
	}
#ifdef CONFIG_FERRET
	if(!strncmp(zen_bypass_info.serial, "WZN301A", 7) ||
			!strncmp(zen_bypass_info.serial, "WZN301S", 7) ||
			!strncmp(zen_bypass_info.serial, "WZN501A", 7))
		cntbp=6;

	if ((g_nic_slot[add_on_map[index]].mixed_iface==1))	{
		zen_bypass_info.allbp[cntbp]=g_nic_slot[add_on_map[index]].element[4];
		zen_bypass_info.allbp[cntbp+1]=g_nic_slot[add_on_map[index]].element[5];
		zen_bypass_info.allbp[cntbp+2]=g_nic_slot[add_on_map[index]].element[6];
		zen_bypass_info.allbp[cntbp+3]=g_nic_slot[add_on_map[index]].element[7];
		cntbp+=4;
	}
    else {
		for(j=0; j< (g_nic_slot[add_on_map[index]].num_of_iface / 2 ) ; j++) {
			zen_bypass_info.allbp[cntbp]=g_nic_slot[add_on_map[index]].element[pj];
			zen_bypass_info.allbp[cntbp+1]=g_nic_slot[add_on_map[index]].element[pj+1];
			cntbp+=2;
			pj+=2;
			pk++;
		}
	}
#endif
	return g_nic_slot[add_on_map[index]].bypass;
}

int lanner_bypass_scan_bus(void)
{
	int i, res=0;
	unsigned char ret_val;
	unsigned char addr, scan_addr;

#ifdef CONFIG_FERRET 
	int f,kf=0;
#endif

	/*
	 * Almost all device support read byte or read byte data command
	 * Try to probe known device
	 */
	unsigned char known_address[] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

	if(zen_bypass_info.is_onboard) {
		card_conf |= 0x03;
		res += 3;
#ifdef CONFIG_FERRET
		cntbp+= 6;
#endif
	}

	for(i=0;i<zen_bypass_info.num_of_slot;i++) {
		scan_addr = known_address[i];
		if(switch_address_add_on_mode(scan_addr))
			addr = 0x30;
		else
			addr = 0x37;
		if(!lanner_bypass_read_byte_data(io_base, addr, 0, &ret_val, 1)) {
			unsigned long long bp;
			bp = get_num_of_bp_in_slot(i);
			res += bp;
			card_conf |= (0x0f&bp)<<((i+1)*4);
		}
	}
#ifdef CONFIG_FERRET 
	if(!strncmp(bootserial,"WZN301A", 7) || 
			!strncmp(bootserial, "WZN301S", 7) || 
			!strncmp(bootserial, "WZN401A", 7) ||
			!strncmp(bootserial, "WZN501A", 7)) {
		cntbp=0;
		if(!strncmp(bootserial, "WZN501A", 7))
			kf=0;
		else if(!strncmp(bootserial, "WZN301A", 7) ||
				!strncmp(bootserial, "WZN301S", 7) ||
				!strncmp(bootserial, "WZN401A", 7))
			kf=-2;
		for(f=0; f < 3; f++) {
			zen_bypass_info.allbp[cntbp]=g_nic_slot[0].element[kf+2];
			zen_bypass_info.allbp[cntbp+1]=g_nic_slot[0].element[kf+3];
			cntbp+=2;
			kf+=2;
			pk++;
		}
	}
#endif
	return res;
}

void lanner_bypass_enable(int data)
{
	int i, slot, addr, mode;
	unsigned char port, pair = 0x00;
	mode = data;

	for(slot=0;slot<zen_bypass_info.num_of_slot+1;slot++) {
		unsigned char mask = 0x00;
		pair = (card_conf>>(slot*4))&0x0f;
		if(!pair)
			continue;
		if(!slot) {
			lanner_bypass_write_byte(io_base, 0, 0, 0, NULL);
			addr = 0x37;
		}
		else {
			if(switch_address_add_on_mode(0x30|(slot-1)))
				addr = 0x30;
			else
				addr = 0x37;
		}

		for(i=0;i<pair;i++)
			mask |= 0x01<<i;
		port = mode&mask;
		if(!slot)
			port = onboard_map[port];
		else if(g_nic_slot[add_on_map[slot-1]].mixed_iface)
			port = port<<0x02;
		mode = mode>>pair;

		lanner_bypass_write_byte_data(io_base, addr, SO_BP_SET_CMD, port);
		lanner_bypass_write_byte_data(io_base, addr, JO_BP_SET_CMD, port);
		lanner_bypass_write_byte_data(io_base, addr, RT_BP_SET_CMD, port);
	}
}

void lanner_bypass_disable(int data)
{
	int i, slot, addr, mode;
	unsigned char port, pair = 0x00;
	port = data;

	for(slot=0;slot<zen_bypass_info.num_of_slot+1;slot++) {
		unsigned char mask = 0x00;
		pair = (card_conf>>(slot*4))&0x0f;
		if(!pair)
			continue;
		if(!slot) {
			lanner_bypass_write_byte(io_base, 0, 0, 0, NULL);
			addr = 0x37;
		}
		else {
			if(switch_address_add_on_mode(0x30|(slot-1)))
				addr = 0x30;
			else
				addr = 0x37;
		}

		for(i=0;i<pair;i++)
			mask |= 0x01<<i;
		port = mode&mask;
		if(!slot)
			port = onboard_map[port];
		else if(g_nic_slot[add_on_map[slot-1]].mixed_iface && slot)
			port = port<<0x02;
		mode = mode>>pair;

		lanner_bypass_write_byte_data(io_base, addr, SO_BP_SET_CMD, 0);
		lanner_bypass_write_byte_data(io_base, addr, JO_BP_SET_CMD, 0);
		lanner_bypass_write_byte_data(io_base, addr, RT_BP_SET_CMD, 0);
	}
}

void lanner_bypass_wd_reset(int data)
{
	int slot, addr;
	unsigned char pair = 0x00;

	for(slot=0;slot<zen_bypass_info.num_of_slot+1;slot++) {
		pair = (card_conf>>(slot*4))&0x0f;
		if(!pair)
			continue;
		if(!slot) {
			lanner_bypass_write_byte(io_base, 0, 0, 0, NULL);
			addr = 0x37;
		}
		else {
			if(switch_address_add_on_mode(0x30|(slot-1)))
				addr = 0x30;
			else
				addr = 0x37;
		}
		lanner_bypass_write_byte_data(io_base, addr, RT_WDT1_START_CMD, 0);
	}
}

void lanner_bypass_wd_timeout(int data, int timeout)
{
	int slot, addr;
	unsigned char pair = 0x00;

	for(slot=0;slot<zen_bypass_info.num_of_slot+1;slot++) {
		pair = (card_conf>>(slot*4))&0x0f;
		if(!pair)
			continue;
		if(!slot) {
			lanner_bypass_write_byte(io_base, 0, 0, 0, NULL);
			addr = 0x37;
		}
		else {
			if(switch_address_add_on_mode(0x30|(slot-1)))
				addr = 0x30;
			else
				addr = 0x37;
		}
		lanner_bypass_write_byte_data(io_base, addr, RT_WDT1_TIMER_CMD, timeout);
	}
}

void lanner_bypass_init(int data)
{
	static struct task_struct *bypass_task;
	int i, slot, addr, mode;
	unsigned char port, pair = 0x00;
	mode = data;

	if(!init_flag) {
		/* ZEN4K has one add-on slot, but Board parameter MB-887X has 4 add-on slots */
		if(!strncmp(zen_bypass_info.serial, "WZN401A", 7)) {
			add_on_map[0] = 0;
			add_on_map[1] = 1;
			add_on_map[2] = 0;
			add_on_map[3] = 0;
		}
		/* ZEN5K on-board bypass pair order sort */
		else if(!strncmp(zen_bypass_info.serial, "WZN501A", 7)) {
			onboard_map[0] = 0;
			onboard_map[1] = 4;
			onboard_map[2] = 2;
			onboard_map[3] = 6;
			onboard_map[4] = 1;
			onboard_map[5] = 5;
			onboard_map[6] = 3;
			onboard_map[7] = 7;
		}
		lanner_bypass_scan_bus();
		init_flag++;
	}

	for(slot=0;slot<zen_bypass_info.num_of_slot+1;slot++) {
		unsigned char mask = 0x00;

		pair = (card_conf>>(slot*4))&0x0f;
		if(!pair)
			continue;
		if(!slot) {
			lanner_bypass_write_byte(io_base, 0, 0, 0, NULL);
			addr = 0x37;
		}
		else {
			if(switch_address_add_on_mode(0x30|(slot-1)))
				addr = 0x30;
			else
				addr = 0x37;
		}

		for(i=0;i<pair;i++)
			mask |= 0x01<<i;
		port = mode&mask;
		if(!slot)
			port = onboard_map[port];
		else if(g_nic_slot[add_on_map[slot-1]].mixed_iface && slot)
			/*
			 * 1G 8pair NIC(copper, fiber mixed) has 2pair(Copper) bypass
			 * This NIC has segment 3 and 4, therefore shift 2bit for port
			 */
			port = port<<0x02;
		mode = mode>>pair;

		lanner_bypass_write_byte_data(io_base, addr, RT_WDT1_PAIR_CMD, port);
		lanner_bypass_write_byte_data(io_base, addr, RT_WDT1_START_CMD, 0);
		lanner_bypass_write_byte_data(io_base, addr, RT_WDT1_TIMER_CMD, g_wd_timeout);
		lanner_bypass_write_byte_data(io_base, addr, SO_BP_SET_CMD, port);
		lanner_bypass_write_byte_data(io_base, addr, JO_BP_SET_CMD, port);
		lanner_bypass_write_byte_data(io_base, addr, RT_BP_SET_CMD, 0);
		lanner_bypass_write_byte_data(io_base, addr, SAVE_SETTING_CMD, port);
	}

	bypass_task = kthread_run(zen_thread_bypass, NULL, "bypass");
	if(IS_ERR(bypass_task))
		printk("bypass watchdog start error.\n");
}
#endif
