#ifndef __PRODUCT_H__
#define __PRODUCT_H__

/* Machine Type */
#define X86_MACHINE     		"d"
#define MIPS_MACHINE    		"g"


#ifdef CONFIG_OCTEON_FUTURE_BOARD
/* Hardware Model */
#define GATE100         2
#define GATE1000        1
#define GATE2000        4
#define GATE2500        5
#define GATE3000        6
#define GATE6000        7
#define GATE10000       8
#define GATE2003        9
#define GATE1500        10


/* Revision Number for Gate100 */
#define G100_STD        2       /* Basic Model 3port, flash-4M */
#define G100_HUB        3       /* with Hub, 4port, flash-16M */
#define G100_RENO       4       /* RenoGate(3 or 4port) */
#define G100_EZLOCK     4       /* EzLock */
#define G100_QUAD       5       /* 4port */
#define G100_STD2       6       /* Basic Model 3port, flash-16M */
#define G100_MARV       7       /* Marvell switch */
#define G100_AP         7       /* wlan Access Point */
#define G100_8PORT      0x11    /* 405EP 8 PORT */
#define G100_ADSL       0x12    /* 4port with ADSL */
#define RENO_ADSL       0x13    /* 4port with ADSL */
#define G100_SMB        0x14    /* 7port PSTN and M-System */
#define G100_IPV6       0x15    /* G100 for IPV6 */
#define G100_X4BIZ      0x16    /* X4BIZ Gate : 2port ADSL */

struct gate_hw_info {           // Gate Hardware Information Block
	short       model;          // Hardware Model
	short       hw_revision;    // Hardware Revision
	unsigned    ram_start;      //
	unsigned    ram_end;        //
	//    unsigned long ram_size;   //
	long long ram_size; //
	unsigned    sram_start;     //
	unsigned    sram_end;       //
	unsigned    sram_size;      //
	unsigned    rom_start;      //
	unsigned    rom_end;        //
	unsigned    rom_size;       //
	unsigned    start;          // start time(utc time)
	unsigned    session;        // 동시 지원 세션 수
	unsigned    user;           // 동시 지원 사용자 수
	unsigned    crypto;         // Crypto Accelerator
	unsigned    kmem;           // Kernel Memory
	unsigned    umem;           // User Memory
	unsigned    aux_port;       // for ICCard, LCD controller..
	unsigned    no_rtc;         // has not RTC chip
	unsigned    ips_mem;
	//    unsigned  auth_user;
} __attribute__ ((packed));
#endif

/* interface zone */
#define IF_MODE_NOT_SET         0
#define IF_MODE_INTERNAL        1
#define IF_MODE_EXTERNAL        2
#define IF_MODE_DMZ             3
#define IF_MODE_PAN             4
#define IF_MODE_MGT             5

#ifdef CONFIG_OCTEON_FUTURE_BOARD
extern int hw_model;
extern int hw_rev_id;
extern int hw_sub_rev;
extern void *g_end_of_dram;     // redefine

extern struct gate_hw_info gate_hw;
#endif

/*
 * 04/05/19 : ramdisk image에도 version정보 때문에 header가 들어간다.
 * moved to this place from rd.c
 * by scsuh.
 */
#define IH_MAGIC				0x27051956     // mkimage use this magic code!!
#define IH_NMLEN				32             // Image Name Length
#define IH_PRELEN				32
#define IH_VERLEN				64
#define IH_RESLEN				(256-IH_PRELEN-IH_VERLEN-4-64)
typedef struct image_header_uboot {
	unsigned int 		ih_magic;
	unsigned int 		ih_hcrc;
	unsigned int 		ih_time;
	unsigned int 		ih_size;
	unsigned int 		ih_load;
	unsigned int 		ih_ep;
	unsigned int 		ih_dcrc;
	unsigned char  		ih_os;
	unsigned char  		ih_arch;
	unsigned char  		ih_type;
	unsigned char  		ih_comp;
	unsigned char  		ih_name[IH_NMLEN];
	unsigned int 		ih_id;
	unsigned char  		ih_product[IH_PRELEN];
	unsigned char  		ih_ver_str[IH_VERLEN];
	unsigned char  		ih_reserverd[IH_RESLEN];
} image_header_uboot_t;




#endif
