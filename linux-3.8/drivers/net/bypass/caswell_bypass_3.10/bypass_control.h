/*
  * We define the pca9555->bypass module comd here
  */
#include <linux/synclink.h>
#include <linux/version.h>
#include <linux/spinlock.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )
#include <linux/semaphore.h>
#endif
#include <linux/miscdevice.h>
#include "bypass_kversion.h"
#include "bypass_platform_type.h"
#include DELAY_MSLEEP_HEAD

#include "bypass_filesystem.h"
#include "bypass_command.h"
/*  Driver version definition */
#define DRV_NAME                   "caswell_bpgen2"
#define DRV_VERSION                 "3.10.0"
#define DRV_DATE                    "Dec. 17 2014"
#define CA_BOARD_PATCH_VER_STR      "V" DRV_VERSION " (" DRV_DATE ")"

#ifdef CONFIG_CA_BOARD_DEBUG
#define CA_BOARD_DEBUG(fmt,args...)	printk(fmt ,##args)
#else
#define CA_BOARD_DEBUG(fmt,args...)
#endif //CONFIG_CA_BOARD_DEBUG

#ifndef module_param
#define module_param(v,t,p) MODULE_PARM(v, "i");
#endif
// Initial value of CPLD version 
#define CPLD_VER_INIT	0xff

//For sysfs cmd
#define SET_NORMAL		0
#define SET_OPEN		1
#define SET_BP			2
#define SET_NB_NORMAL		0
#define SET_NB_NO_NORMAL	1
#define SET_BPE_OPEN		0
#define SET_BPE_BP		1
#define WDT_DISABLE		0
#define WDT_ARM_REFRESH		1
#define WDT_CLEAR		2
#define DELAY_FOR_RELAY		34	

//We have copper and fiber two interface, default is copper
#define COPPER_PIC_WDT_264258			0x80	//orig microcon cmd is 0x01
#define COPPER_PIC_WDT_528516			0x01	//orig microcon cmd is 0x02
#define COPPER_PIC_WDT_1057032			0x81	//orig microcon cmd is 0x03
#define COPPER_PIC_WDT_2114065			0x02	//orig microcon cmd is 0x04
#define COPPER_PIC_WDT_4228129			0x82	//orig microcon cmd is 0x05
#define COPPER_PIC_WDT_8456258			0x03	//orig microcon cmd is 0x06
#define COPPER_PIC_WDT_1691252			0x83	//orig microcon cmd is 0x07
#define COPPER_PIC_WDT_3382503			0x04	//orig microcon cmd is 0x08
#define COPPER_PIC_WDT_6755006			0x84	//orig microcon cmd is 0x09
#define COPPER_PIC_WDT_1353001			0x05	//orig microcon cmd is 0x0A
#define COPPER_PIC_WDT_2706003			0x85	//orig microcon cmd is 0x0B
#define COPPER_PIC_WDT_PERIOD 			{0x80, 0x01, 0x81, 0x02, 0x82, 0x03, 0x83, 0x04, 0x84, 0x05, 0x85}
#define COPPER_PIC_WDT_PERIOD_MAX		0x0b

#define COPPER_PIC_SET_NB_NORMAL		0x08	//orig microcon cmd is 0x10
#define COPPER_PIC_SET_NB_NO_NORMAL		0x88	//orig microcon cmd is 0x11
#define COPPER_PIC_SET_BPE_OPEN			0x09	//orig microcon cmd is 0x12
#define COPPER_PIC_SET_BPE_BP			0x89	//orig microcon cmd is 0x13
#define COPPER_PIC_SET_NORMAL			0x0A	//orig microcon cmd is 0x14
#define COPPER_PIC_SET_NONORMAL			0x8A	//orig microcon cmd is 0x15

#define COPPER_PIC_GET_MODE_STS			0x0B	//orig microcon cmd is 0x16
#define COPPER_PIC_GET_NB_BPE_STS		0x8B	//orig microcon cmd is 0x17
#define COPPER_PIC_GET_BP_WDT_STS		0x0C	//orig microcon cmd is 0x18

#define COPPER_PIC_BP_WDT_CLR			0x8C	//orig microcon cmd is 0x19
#define COPPER_PIC_BP_WDT_REFRESH		0x0D	//orig microcon cmd is 0x1A
#define COPPER_PIC_BP_WDT_DIS			0x8D	//orig microcon cmd is 0x1B

#define COPPER_PIC_SET_CMD_BITS_HIGH		0x8F	//orig microcon cmd is 0x1F

#define COPPER_PIC_MICROCON_DELAY		10	//delay about 10 us for I2C cmd complete 
							//(but currently 10 ms is used) 
#define COPPER_PIC_STS_BIT0			BIT4
#define COPPER_PIC_STS_BIT1			BIT5

#define COPPER_PIC_WDT_TIMEOUT_BIT		BIT4
#define COPPER_PIC_WDT_STS_BIT			BIT5

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define COPPER_CPLD_V2P0_SET_NORMAL_L		253	//orig CPLD cmd is 0xFD
#define COPPER_CPLD_V2P0_SET_NORMAL_H		2	//orig CPLD cmd is 0xFD
#define COPPER_CPLD_V2P0_SET_NONORMAL_L		251    //orig CPLD cmd is 0xFB
#define COPPER_CPLD_V2P0_SET_NONORMAL_H		4    //orig CPLD cmd is 0xFB

#define COPPER_CPLD_V2P0_SET_BPE_OPEN		239    //orig CPLD cmd is 0xEF
#define COPPER_CPLD_V2P0_SET_BPE_BP		16    //orig CPLD cmd is 0x10

#define COPPER_CPLD_V2P0_CLR_BP_WDT_L		254	//orig CPLD cmd is 0x01 for wdt clear
#define COPPER_CPLD_V2P0_CLR_BP_WDT_H		1	//orig CPLD cmd is 0x01 for wdt clear
// because ABN484L bit6&7 is config pca9559 address,so bit6&7 is fixed
#define COPPER_CPLD_V2P0_REFRESH_BP_WDT_L	254	//orig CPLD cmd is 0x01 for wdt arm refresh
#define COPPER_CPLD_V2P0_REFRESH_BP_WDT_H	1	//orig CPLD cmd is 0x01 for wdt arm refresh
#define COPPER_CPLD_V2P0_DISABLE_WDT_L		253	//orig CPLD cmd is 0x02 for wdt disable
#define COPPER_CPLD_V2P0_DISABLE_WDT_H		2	//orig CPLD cmd is 0x02 for wdt disable

#define COPPER_CPLD_V2P0_SET_BP_PERIOD(sec)	(sec & 0x3f) << 2;   
#define COPPER_CPLD_V2P0_WDT_PERIOD_MAX		63

#define COPPER_CPLD_V2P0_MICROCON_DELAY		10

#define COPPER_CPLD_V2P0_STS_BIT0		BIT6
#define COPPER_CPLD_V2P0_STS_BIT1		BIT4

#define COPPER_CPLD_V2P0_WDT_TIMEOUT_BIT	BIT7
#define COPPER_CPLD_V2P0_WDT_STS_BIT		BIT3

//Fiber sreial have different cmd and status return type, see the ref.

/* ############################  Define for ABN482 family  ################################*/
#define FIBER_ABN482_TX_DISABLE0		BIT5
#define FIBER_ABN482_TX_DISABLE1		BIT4
#define FIBER_ABN482_SEGB_STS_BIT0		BIT6
#define FIBER_ABN482_SEGB_STS_BIT1		BIT7
#define FIBER_ABN482_SEGA_STS_BIT0		BIT6
#define FIBER_ABN482_SEGA_STS_BIT1		BIT7

#define FIBER_ABN482_WDT_TIMEOUT_BIT		BIT3
#define FIBER_ABN482_WDT_BP_STS_BIT		BIT4  

#define FIBER_ABN482_SET_CMD_BITS_HIGH		0x40

#define FIBER_ABN482_SET_NORMAL(index)		0x20
#define FIBER_ABN482_SET_NONORMAL(index)	0x21
#define FIBER_ABN482_SET_OPEN(index)		0x24
#define FIBER_ABN482_SET_BP(index)		0x25
#define FIBER_ABN482_SET_NB_NORMAL(index)	0x22
#define FIBER_ABN482_SET_NB_NO_NORMAL(index)	0x23
#define FIBER_ABN482_GET_MODE_STS		0x26
#define FIBER_ABN482_GET_NB_BPE_STS		0x27
#define FIBER_ABN482_GET_BP_WDT_STS		0x26

#define FIBER_ABN482_WDT_LOAD_BIT		BIT0
#define FIBER_ABN482_WDT_CLEAR_BIT		BIT1
#define FIBER_ABN482_WDT_DIS_BIT		BIT2
#define FIBER_ABN482_WDT_STS_BIT		BIT3
#define FIBER_ABN482_BP_DIS_BIT			BIT4


#define FIBER_ABN482_CLR_BP_WDT			0xE5
#define FIBER_ABN482_REFRESH_BP_WDT		0xE6
#define FIBER_ABN482_DISABLE_WDT		0xEB

#define FIBER_ABN482_WDT_PERIOD_MAX		63		//63 sec is the max period

#ifdef MIPS_CAVIUM_TARGET
#define FIBER_ABN482_MICROCON_DELAY		2200		
#else 
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,4,0) )
#define FIBER_ABN482_MICROCON_DELAY     1530    //Increase delay time because I2C IO latency is decreased in kernel from kernel 3.4.0
#else
#define FIBER_ABN482_MICROCON_DELAY		30		//delay about 30 us for I2C cmd complete
#endif
#endif								//(but currently 30 ms is used) 

/* ############################  Define for ABN1004 family  ################################*/
#define FIBER_ABN1004_SEGB_STS_BIT0             BIT4
#define FIBER_ABN1004_SEGB_STS_BIT1             BIT5
#define FIBER_ABN1004_SEGA_STS_BIT0             BIT6
#define FIBER_ABN1004_SEGA_STS_BIT1             BIT7

#define FIBER_ABN1004_WDT_TIMEOUT_BIT		BIT3
#define FIBER_ABN1004_WDT_BP_STS_BIT		BIT4  

#define FIBER_ABN1004_SET_CMD_BITS_HIGH		0x40

//segment a,b cmd interval is 2
#define FIBER_ABN1004_SET_NORMAL(index)		(0x24+index*2)
#define FIBER_ABN1004_SET_NONORMAL(index)	(0x25+index*2)
#define FIBER_ABN1004_SET_OPEN(index)		(0x34+index*2)
#define FIBER_ABN1004_SET_BP(index)		(0x35+index*2)
#define FIBER_ABN1004_SET_NB_NORMAL(index)	(0x30+index*2)
#define FIBER_ABN1004_SET_NB_NO_NORMAL(index)	(0x31+index*2)
#define FIBER_ABN1004_GET_MODE_STS		0x38
#define FIBER_ABN1004_GET_NB_BPE_STS		0x39
#define FIBER_ABN1004_GET_BP_WDT_STS		0x3C

//In pca9555 IO10~IO17
#define FIBER_ABN1004_WDT_LOAD_BIT		BIT0
#define FIBER_ABN1004_WDT_CLEAR_BIT		BIT1
#define FIBER_ABN1004_WDT_DIS_BIT		BIT2
#define FIBER_ABN1004_WDT_STS_BIT		BIT3
#define FIBER_ABN1004_BP_DIS_BIT		BIT4

#define FIBER_ABN1004_CLR_BP_WDT		0x15
#define FIBER_ABN1004_REFRESH_BP_WDT		0x16
#define FIBER_ABN1004_DISABLE_WDT		0xFB

#define FIBER_ABN1004_WDT_PERIOD_MAX		63		//63 sec is the max period

#define FIBER_ABN1004_MICROCON_DELAY		30		//delay about 30 us for I2C cmd complete
						    		//but currentlt 30 ms is used

#ifndef __BYPASS_CONTROL_STRUC__
#define __BYPASS_CONTROL_STRUC__

typedef struct{
	u32 reg_offset; 
	u32 dir_mask;
	u32 cmd_mask;
}bp_pci_cmd;

typedef struct{
	uint16_t vid;	
	uint16_t did;	
	uint16_t svid;	
	uint16_t sdid;
	uint32_t io_bar_offset;
	uint32_t slot_chk_reg;
	uint32_t slot_chk_mask;
	u8 slot_dev_mask;
	uint32_t slot_id_reg0;
	uint32_t slot_id_reg1;
	uint32_t slot_id_mask0;
	uint32_t slot_id_mask1;
	
} bp_pci_info;

typedef struct{
	const bp_pci_info info;
	const bp_pci_cmd* refresh_wdt_cmds;
} bp_pci_initdata;

typedef struct{
	uint16_t ioaddr;
	uint16_t iodata;
	spinlock_t sdp_spin_lock;
} pci_ctrl_iface;


typedef struct {
	struct pci_dev* mac_dev;
	uint8_t slot_id;
	uint16_t ioaddr;
	uint16_t iodata;
	struct list_head dev_list;
}mac_list;

typedef struct {
	const bp_pci_info* type_info;
	mac_list mac_list;
	struct list_head type_list;
}bp_mac_type_list;


typedef struct _bp_cmd {
	u8 reg_in;
	u8 reg_out;
	u8 bit_high;
	u8 cmd;
} bp_cmd;

struct _bp_interface;
typedef struct _bp_interface bp_interface;

typedef struct _bp_seg {
	u8 name;
	u8 index;
	u8 type;
	u8 ctrl_addr;
	u8 wdt_addr;
	u8 period_addr;
	u8 eeprom_addr;
	u8 firmware_ver_addr;
	u8 reg_reposit;
	u16 delay_ms;
	//Status bit
	u8 bp_sts_bit0;
	u8 bp_sts_bit1;
	u8 wdt_en_bit;
	u8 wdt_tmout_bit;
	u8 wdt_period;
	u8 generation;    
	//Bypass cmd type and function reg
	bp_cmd set_normal;
	bp_cmd set_nonormal;
	bp_cmd set_bpe_open;
	bp_cmd set_bpe_bypass;
	bp_cmd set_nb_normal;
	bp_cmd set_nb_nonormal;
	bp_cmd wdt_disable;
	bp_cmd wdt_refresh;
	bp_cmd wdt_clear;
	bp_cmd set_period;
	bp_cmd get_bypass;
	bp_cmd get_nb;
	bp_cmd get_wdt;
	u16 auto_trigger_interval;
	struct timer_list trigger_timer;
	const bp_pci_cmd* pci_wdt_refresh;
	struct semaphore bp_sem;
	bp_interface* bp_iface;
} bp_seg;

struct _bp_interface {
	bp_seg seg[3];
	unsigned char *modelname;    
	int seg_no;
	struct pci_dev* mac_dev;
	pci_ctrl_iface pci_ctrl;
	u8 fw_version;
	unsigned char slot_no;
};

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

struct ca_bypass_data {
#ifdef DEF_FS_SYS_SUPPORT    
	struct i2c_client client;
	struct mutex update_lock;
#else
	u8 in0, in1, out0, out1, conf0, conf1, inv0, inv1;        /* Registers */
#endif
};

#endif /*__BYPASS_CONTROL_STRUC__*/

u8 copper_cpld_eeprom_read_nb(u8 addr, u8 index);
int copper_cpld_eeprom_write_nb(u8 addr, u8 index, u8 val);
u8 copper_cpld_eeprom_read_bpe(u8 addr, u8 index);
int copper_cpld_eeprom_write_bpe(u8 addr, u8 index, u8 val);
int bypass_check_cpld_ver_ABN482(struct i2c_client *client);
int bypass_check_cpld_ver_ABN484(struct i2c_client *client, unsigned short addr);
int find_client_bp_seg(bp_seg **seg, struct i2c_client *client, int index);
int find_client_bp_interface(bp_interface **interface, struct i2c_client *client, int index);
int find_slot_bp_interface(bp_interface **interface, uint8_t slot);
uint8_t get_bp_seg_ctrl_addr(uint8_t slot, uint8_t seg);
void bypass_reg_set_high_low(struct i2c_client *client, bp_cmd *bp_cmd, u8 reg_reposit, long delay);
void bypass_pci_set_high_low(pci_ctrl_iface* ctrl, const bp_pci_cmd* cmd, long delay);
void bypass_pci_set_high(pci_ctrl_iface* ctrl, const bp_pci_cmd* cmd, long delay);
int bypass_check_sdp(pci_ctrl_iface* ctrl, const bp_pci_cmd* cmd);
int client_is_bp_wdt(struct i2c_client *client, int interface);
int client_is_bp_eeprom(struct i2c_client *client);
int client_is_bp_ctrl(struct i2c_client *client, int interface);
int bypass_register_fs(struct miscdevice *mdev, int slot_no);
int bypass_setup_fs(struct miscdevice *mdev, int slot_no, int type);
int bypass_unsetup_fs(struct miscdevice *mdev, int slot_no);
int bypass_client_fill(struct i2c_client *client);
