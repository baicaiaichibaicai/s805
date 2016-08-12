/*
  * We define general bypass definition here
  */

//The bypass segmentation index
#define BP_SEGA	0
#define BP_SEGB	1
#define BP_SEGC	2

//The bypass hardware generation
#define BP_GEN1	1
#define BP_GEN2	2

//Interface list of ca board bypass
#define BP_COPPER_PIC	1
#define BP_FIBER_PIC_ABN482	2
#define BP_FIBER_PIC_ABN1004	3
#define BP_COPPER_CPLDV2	4

//bypass interface contain segment number
#define BP_SEGS_1	1
#define BP_SEGS_2	2
#define BP_SEGS_3	3

//Define the max network interface
#define NET_INTERFACE_MAX	4

//Define the max segs in one interface
#define NET_INTERFACE_MAX_BP_SEGS	3

//Define the ATTR type list
#define DEFATTR 1
#define BYPASS0ATTR 2
#define BYPASS1ATTR 3

typedef struct _bp_unit
{
	unsigned char name;
	unsigned char type;
	unsigned char segs;
	unsigned char ctrl_addr[NET_INTERFACE_MAX_BP_SEGS];
	unsigned char wdt_addr[NET_INTERFACE_MAX_BP_SEGS];
	unsigned char period_addr[NET_INTERFACE_MAX_BP_SEGS];
	unsigned char eeprom_addr[NET_INTERFACE_MAX_BP_SEGS];
	unsigned char generation;
	unsigned char firmare_ver_addr;
}bp_unit;
