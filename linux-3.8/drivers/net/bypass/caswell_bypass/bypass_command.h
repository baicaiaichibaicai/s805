/* PCA9555 default setting */
//If the bypass type is fiber pic and 9555 usage is bypass wdt
#define FIBER_PIC_ABN482_WDT_DR0_DF	0x00
#define FIBER_PIC_ABN482_WDT_DR1_DF	0x08
#define FIBER_PIC_ABN482_WDT_OUT1_DF	0xE7	//init BP_Disa

//If the bypass type is fiber pic and 9555 usage is bypass controlling
#define FIBER_PIC_ABN482_CTRL_DR0_DF	0x80
#define FIBER_PIC_ABN482_CTRL_DR1_DF	0xC0

#define FIBER_PIC_ABN482_BP_GROUP_NUM	1	//define the bypass segment numbers a PCA9555 handling

//If the bypass type is copper pic and 9555 usage is bypass controlling
#define COPPER_PIC_CTRL_DR0_DF	0x70
#define COPPER_PIC_CTRL_DR1_DF	0x70

//If the bypass type is copper cpld and 9555 usage is bypass controlling
#define COPPER_CPLD_V2P0_CTRL_DR0_DF	0x00
#define COPPER_CPLD_V2P0_CTRL_DR1_DF	0xC0

//If the bypass type is copper cpld and 9555 usage is bypass controlling
//usage get BPE from PCA9559 for synchronization of 9555_BPE and 9559_BPE
#define COPPER_CPLD_V2P0_CTRL_OUT0_DF	0xFF
#define COPPER_CPLD_V2P0_CTRL_OUT1_DF	0x2F
