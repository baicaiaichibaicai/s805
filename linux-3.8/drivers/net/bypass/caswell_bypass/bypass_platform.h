/*
  * We define the platform dependence config here
  */
#include "bypass_def.h"

#define X86_TARGET	1

//Now, the network bypass add-card could use 3 slots at most
#define ADDCARD_MAX_SLOTS	3

//Type list of ca board bypass
#define BP_MB_MAX		12
#define BP_MB_NAR5530	0
#define BP_MB_NAR5650	1
#define BP_MB_NAR5520	2
#define BP_MB_CAR2000	3	
#define BP_MB_CAR3000	4
#define BP_MB_NAD2100L	5
#define BP_MB_NAR2200	6
#define BP_MB_NAR5630	7
#define BP_MB_NAR4060	8
#define BP_MB_NAR7090	9
#define BP_MB_NAR7100	10
#define BP_MB_CAR4000	11

#ifndef  INCLUDE_PLATEFORM_MARCO
unsigned char *BP_MB_NAME[BP_MB_MAX]={"NAR5530", "NAR5650","NAR5520", "CAR2000", "CAR3000","NAD2100L",\
                                      "NAR2200","NAR5630","NAR4060","NAR7090","NAR7100","CAR4000"};

bp_unit mb_bypass_info[BP_MB_MAX]={
	//{ board index, bypass type, total segs, ctrl addr, wdt addr, period addr, 9559 addr, generation }
	/*set NAR5530 motherboard configuration*/
	{BP_MB_NAR5530, BP_COPPER_CPLDV2, BP_SEGS_3, \
         {0x26, 0x21,0x25}, {0x0,0x0,0x0}, {0x0,0x0,0x0}, {0x4f,0x4f,0x4f}, BP_GEN2},
	
	/*set NAR5650 motherboard configuration*/	
	{BP_MB_NAR5650, BP_COPPER_CPLDV2, BP_SEGS_3, \
	{0x26, 0x22,0x27}, {0x0,0x0,0x0}, {0x0,0x0,0x0}, {0x4f,0x4f,0x4f}, BP_GEN2},
	
	/*set NAR5520 motherboard configuration*/	
	{BP_MB_NAR5520, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x21}, {0x0,0x0},{0x0,0x0},{0x4f,0x4f}, BP_GEN2},
	
	/*set CAR-2000 motherboard configuration*/	
	{BP_MB_CAR2000, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x22}, {0x0,0x0}, {0x0,0x0}, {0x4f,0x4f}, BP_GEN2},
		
	/*set CAR-3000 motherboard configuration*/	
	{BP_MB_CAR3000, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x22}, {0x0,0x0}, {0x0,0x0}, {0x4f,0x4f}, BP_GEN2},

	/*set NAD2100L motherboard configuration*/	
	{BP_MB_NAD2100L, BP_COPPER_CPLDV2, BP_SEGS_1, \
	{0x26}, {0x0,0x0}, {0x0,0x0}, {0x4f}, BP_GEN2},	

	/*set NAR2200 motherboard configuration*/	
	{BP_MB_NAR2200, BP_COPPER_CPLDV2, BP_SEGS_1, \
	{0x26}, {0x0,0x0}, {0x0,0x0}, {0x4f}, BP_GEN2},		

	/*set NAR5630 motherboard configuration*/	
	{BP_MB_NAR5630, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x21}, {0x0,0x0},{0x0,0x0},{0x4f,0x4f}, BP_GEN2},    

	/*set NAR4060 motherboard configuration*/	
	{BP_MB_NAR4060, BP_COPPER_CPLDV2, BP_SEGS_1, \
	{0x26}, {0x0,0x0}, {0x0,0x0}, {0x4f}, BP_GEN2},			
	{},
	{},
	{},
};
#endif	/*INCLUDE_PLATEFORM_MARCO*/

/* The bypass add-card definition */
#define BP_CARD_MAX		3
#define BP_CARD_ABN484	0
#define BP_CARD_ABN482	1
#define BP_CARD_ABN484L	2

#ifndef  INCLUDE_PLATEFORM_MARCO
unsigned char *BP_CARD_NAME[BP_CARD_MAX]={"ABN484", "ABN482", "ABN484L"};

bp_unit addcard_bypass_info[ADDCARD_MAX_SLOTS][BP_CARD_MAX]={
	//{ board index, bypass type, total segs, ctrl addr, wdt addr, period addr, 9559 addr, generation }
	{
		/*set add-on-card configuration if slot is 0*/
		{BP_CARD_ABN484, BP_COPPER_CPLDV2, BP_SEGS_2, \
		{0x20, 0x24}, {0x0,0x0},{0x0,0x0}, {0x4c,0x4c}, BP_GEN2, 0x20},

		{BP_CARD_ABN482, BP_FIBER_PIC_ABN482, BP_SEGS_1, \
		{0x20}, {0x24}, {0x24}, {0x0}, BP_GEN2},

		{BP_CARD_ABN484L, BP_COPPER_CPLDV2, BP_SEGS_2, \
		{0x20, 0x24},{0x0,0x0},{0x0,0x0},{0x4c,0x4c}, BP_GEN2, 0x20},
	}, {
		/*set add-on-card configuration if slot is 1*/
		{BP_CARD_ABN484, BP_COPPER_CPLDV2, BP_SEGS_2, \
		{0x21, 0x25}, {0x0,0x0},{0x0,0x0}, {0x4d,0x4d}, BP_GEN2, 0x21},

		{BP_CARD_ABN482, BP_FIBER_PIC_ABN482, BP_SEGS_1, \
		{0x21}, {0x25}, {0x25}, {0x0}, BP_GEN2},

		{BP_CARD_ABN484L, BP_COPPER_CPLDV2, BP_SEGS_2, \
		{0x21, 0x25},{0x0,0x0},{0x0,0x0},{0x4d,0x4d}, BP_GEN2, 0x21},
	
	}, {
		/*set add-on-card configuration if slot is 2*/
		{BP_CARD_ABN484, BP_COPPER_CPLDV2, BP_SEGS_2, \
		{0x22, 0x26}, {0x0,0x0},{0x0,0x0}, {0x4e,0x4e}, BP_GEN2, 0x22},

		{BP_CARD_ABN482, BP_FIBER_PIC_ABN482, BP_SEGS_1, \
		{0x22}, {0x26}, {0x26}, {0x0}, BP_GEN2},
	
		{BP_CARD_ABN484L, BP_COPPER_CPLDV2, BP_SEGS_2, \
		{0x22, 0x26},{0x0,0x0},{0x0,0x0},{0x4e,0x4e}, BP_GEN2, 0x22},
	}
};
#endif	/*INCLUDE_PLATEFORM_MARCO*/
