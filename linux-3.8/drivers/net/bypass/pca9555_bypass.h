/*
  * We define the pca9555->bypass module comd here
  */
#include <linux/synclink.h>

/*  Driver version definition */
#ifdef CAVIUM_BYPASS_ENABLE			
#define CA_BOARD_PATCH_VER_STR		"173_264_v1.0.1 (Feb 16 2009)"
#else
#define CA_BOARD_PATCH_VER_STR		"v1.0.1 (Mar 24 2009)"
#endif

#ifdef CONFIG_CA_BOARD_DEBUG
#define CA_BOARD_DEBUG(fmt,args...)	printk(fmt ,##args)
#else
#define CA_BOARD_DEBUG(fmt,args...)
#endif //CONFIG_CA_BOARD_DEBUG

#ifndef CA_NET_INTERFACE_NO
#define CA_NET_INTERFACE_NO	3
#define CA_BOARD_TYPE		"X86"
#endif

#define BP_INTERFACE_SEG_MAX	4
/* Addresses*/
static unsigned short i2c_ctrl_addr[]       = {0x24, 0x20, 0x25,0x21, 0x26, 0x22,0x27,0x23};
static unsigned short i2c_eeprom_addr[]     = {0x4c,0x4d,0x4e,0x4f};
u8 CA_BYPASS_TYPE[BP_INTERFACE_SEG_MAX]     = {0,0,0,0};
u8 CA_BYPASS_NAME[BP_INTERFACE_SEG_MAX]     = {0,0,0,0};
u8 CA_BYPASS_SEG_NO[BP_INTERFACE_SEG_MAX]   = {0,0,0,0};
u8 CA_BP_CTRL_ADDR[BP_INTERFACE_SEG_MAX*2]  = {0,0,0,0,0,0,0,0};
u8 CA_BP_WDT_ADDR[BP_INTERFACE_SEG_MAX*2]   = {0,0,0,0,0,0,0,0};
u8 CA_BP_PERIOD_ADDR[BP_INTERFACE_SEG_MAX*2]= {0,0,0,0,0,0,0,0};
u8 CA_BP_EEPROM_ADDR[BP_INTERFACE_SEG_MAX*2]= {0,0,0,0,0,0,0,0};

#define NAR5520 0x5520
#define NAR5530 0x5530
#define NAR5650 0x5650

#define BYPASS_5520_SEG_NO 2
static unsigned short i2c_5520_ctrl_addr[]       = {0x26, 0x21};
static unsigned short i2c_5520_eeprom_addr[]     = {0x4f,0x4f};

#define BYPASS_5530_SEG_NO 3
static unsigned short i2c_5530_ctrl_addr[]       = {0x26, 0x21,0x25};
static unsigned short i2c_5530_eeprom_addr[]     = {0x4f,0x4f,0x4f};

#define BYPASS_5650_SEG_NO 3
static unsigned short i2c_5650_ctrl_addr[]       = {0x27, 0x22,0x26};
static unsigned short i2c_5650_eeprom_addr[]     = {0x4f,0x4f,0x4f};

//Type list of ca board bypass
#define CA_BYPASS_COPPER_PIC	1
#define CA_BYPASS_FIBER_PIC	2
#define CA_BYPASS_COPPER_CPLD_V2P0 3

//Type list of ca board bypass
#define CA_BYPASS_MEB3920	1
#define CA_BYPASS_NAR5530	2
#define CA_BYPASS_ABN482	3
#define CA_BYPASS_ABN484	4
#define CA_BYPASS_NAR5650	5
#define CA_BYPASS_NAR5520	6

//The pca9555 to microcontroller i2c cmd type
#define CA_BYPASS_SEGA	0
#define CA_BYPASS_SEGB	1

//For sysfs cmd
#define SET_NORMAL			0
#define SET_OPEN			1
#define SET_BYPASS			2
#define SET_NB_NORMAL			0
#define SET_NB_NO_NORMAL		1
#define SET_BPE_OPEN			0
#define SET_BPE_BYPASS			1
#define WDT_DISABLE			0
#define WDT_ARM_REFRESH			1
#define WDT_CLEAR			2
#define WDT_ENABLE			3

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
#define COPPER_PIC_SET_BPE_BYPASS		0x89	//orig microcon cmd is 0x13
#define COPPER_PIC_SET_NORMAL			0x0A	//orig microcon cmd is 0x14
#define COPPER_PIC_SET_NONORMAL			0x8A	//orig microcon cmd is 0x15

#define COPPER_PIC_GET_MODE_STS			0x0B	//orig microcon cmd is 0x16
#define COPPER_PIC_GET_NB_BPE_STS		0x8B	//orig microcon cmd is 0x17
#define COPPER_PIC_GET_BP_WDT_STS		0x0C	//orig microcon cmd is 0x18

#define COPPER_PIC_BP_WDT_CLR			0x8C	//orig microcon cmd is 0x19
#define COPPER_PIC_BP_WDT_REFRESH		0x0D	//orig microcon cmd is 0x1A
#define COPPER_PIC_BP_WDT_DIS			0x8D	//orig microcon cmd is 0x1B

#define COPPER_PIC_SET_CMD_BITS_HIGH		0x8F	//orig microcon cmd is 0x1F

#define COPPER_PIC_MICROCON_DELAY		10	//delay about 1 ms for I2C cmd complete

#define COPPER_PIC_STS_BIT0			BIT4
#define COPPER_PIC_STS_BIT1			BIT5

#define COPPER_PIC_WDT_TIMEOUT_BIT		BIT4
#define COPPER_PIC_WDT_STS_BIT			BIT5

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define COPPER_CPLD_V2P0_SET_NORMAL		0xFD	//orig CPLD cmd is 0xFD
#define COPPER_CPLD_V2P0_SET_NONORMAL           0xFB    //orig CPLD cmd is 0xFB
#define COPPER_CPLD_V2P0_SET_BPE_OPEN		0xEF    //orig CPLD cmd is 0xEF
#define COPPER_CPLD_V2P0_SET_BPE_BYPASS		0x10    //orig CPLD cmd is 0x10
#define COPPER_CPLD_V2P0_CLR_BP_WDT		0xFE	//orig CPLD cmd is 0x01 for wdt clear
#define COPPER_CPLD_V2P0_REFRESH_BP_WDT		0xFE	//orig CPLD cmd is 0x01 for wdt arm refresh
#define COPPER_CPLD_V2P0_DISABLE_WDT            0xFD	//orig CPLD cmd is 0x02 for wdt disable
#define COPPER_CPLD_V2P0_SET_BP_PERIOD(sec)     (sec & 0x3f) << 2;   
#define COPPER_CPLD_V2P0_WDT_PERIOD_MAX		63

#define COPPER_CPLD_V2P0_MICROCON_DELAY		10

#define COPPER_CPLD_V2P0_STS_BIT0		BIT6
#define COPPER_CPLD_V2P0_STS_BIT1		BIT4

#define COPPER_CPLD_V2P0_WDT_TIMEOUT_BIT	BIT7
#define COPPER_CPLD_V2P0_WDT_STS_BIT		BIT3

//Fiber sreial have different cmd and status return type, see the ref.
#ifdef CAVIUM_BYPASS_ENABLE	
#define FIBER_SEGB_STS_BIT0			BIT6
#define FIBER_SEGB_STS_BIT1			BIT7
#else
#define FIBER_SEGB_STS_BIT0			BIT4
#define FIBER_SEGB_STS_BIT1			BIT5
#endif
#define FIBER_SEGA_STS_BIT0			BIT6
#define FIBER_SEGA_STS_BIT1			BIT7

#define FIBER_WDT_TIMEOUT_BIT			BIT3
#define FIBER_WDT_BP_STS_BIT			BIT4  

#define FIBER_SET_CMD_BITS_HIGH			0x40
//segment a,b cmd interval is 2
#ifdef CAVIUM_BYPASS_ENABLE	
#define FIBER_SET_NORMAL(index)			(0x24+index*2)
#define FIBER_SET_NONORMAL(index)		(0x25+index*2)
#define FIBER_SET_OPEN(index)			(0x34+index*2)
#define FIBER_SET_BYPASS(index)			(0x35+index*2)
#define FIBER_SET_NB_NORMAL(index)		(0x30+index*2)
#define FIBER_SET_NB_NO_NORMAL(index)		(0x31+index*2)
#define FIBER_GET_MODE_STS			0x38
#define FIBER_GET_NB_BPE_STS			0x39
#define FIBER_GET_BP_WDT_STS			0x3C
#else
#define FIBER_SET_NORMAL(index)			0x20
#define FIBER_SET_NONORMAL(index)		0x21
#define FIBER_SET_OPEN(index)			0x24
#define FIBER_SET_BYPASS(index)			0x25
#define FIBER_SET_NB_NORMAL(index)		0x22
#define FIBER_SET_NB_NO_NORMAL(index)		0x23
#define FIBER_GET_MODE_STS			0x26
#define FIBER_GET_NB_BPE_STS			0x27
#define FIBER_GET_BP_WDT_STS			0x26
#endif
//In pca9555 IO10~IO17
#define FIBER_WDT_LOAD_BIT			BIT0
#define FIBER_WDT_CLEAR_BIT			BIT1
#define FIBER_WDT_DIS_BIT			BIT2
#define FIBER_WDT_STS_BIT			BIT3
#define FIBER_BP_DIS_BIT			BIT4
#ifdef CAVIUM_BYPASS_ENABLE
#define FIBER_CLR_BP_WDT			0x15
#define FIBER_REFRESH_BP_WDT			0x16
#define FIBER_DISABLE_WDT			0xFB
#else
#define FIBER_CLR_BP_WDT			0xE5
#define FIBER_REFRESH_BP_WDT		0xE6
#define FIBER_DISABLE_WDT			0xEB
#endif
#define FIBER_WDT_PERIOD_MAX			63		//63 sec is the max period

#define FIBER_MICROCON_DELAY			30		//delay about 30 ms for I2C cmd complete

typedef struct _bp_cmd {
	u8 reg_in;
	u8 reg_out;
	u8 bit_high;
	u8 cmd;
} bp_cmd;

typedef struct _bp_seg {
	u8 name;
	u8 index;
	u8 type;
	u8 bp_ctrl_addr;
	u8 bp_wdt_addr;
	u8 bp_period_addr;
	u8 bp_eeprom_addr;
	u8 reg_reposit;
	u8 delay_ms;
	//Status bit
	u8 bp_sts_bit0;
	u8 bp_sts_bit1;
	u8 wdt_en_bit;
	u8 wdt_tmout_bit;
	u8 wdt_period;
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
} bp_seg;

typedef struct _bp_interface {
	bp_seg seg[BP_INTERFACE_SEG_MAX];
	int seg_no;
} bp_interface;

