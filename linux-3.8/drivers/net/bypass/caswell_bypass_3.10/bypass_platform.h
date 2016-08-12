#ifndef _BYPASS_PLATFORM_H_
#define _BYPASS_PLATFORM_H__

/*
  * We define the platform dependence config here
  */
#include "bypass_def.h"
#include "bypass_platform_type.h"
#include "bypass_pciwdt.h"
#include "bypass_add-on.h"

//Type list of ca board bypass
#define BP_MB_MAX		27
#define BP_MB_CAR2000	0	
#define BP_MB_CAR3000	1
#define BP_MB_NAR7090	2
#define BP_MB_NAR7100	3
#define BP_MB_CAR4000	4
#define BP_MB_CAR5000	5
#define BP_MB_CAR5010	6
#define BP_MB_CAR4010	7
#define BP_MB_CAR3020	8
#define BP_MB_CAR5020	9
#define BP_MB_CAR3030	10
#define BP_MB_CAR5030	11
#define BP_MB_CPT4010	12
#define BP_MB_CPT3210	13
#define BP_MB_CAR4018	14
#define BP_MB_CAR4211C	15
#define BP_MB_CAR4212C	16
#define BP_MB_COS9200	17
#define BP_MB_NAR5520	18
#define BP_MB_CAR3040	19
#define BP_MB_COS5700	20
#define BP_MB_CAR4020	21
#define BP_MB_CAR2051	22
#define BP_MB_CAR3050	23
#define BP_MB_CAR5040	24
#define BP_MB_CAR3231C_LD	25
#define BP_MB_CAR3042C_LD	26

#ifndef  __INCLUDE_PLATEFORM_DATA__

unsigned char *BP_MB_NAME[BP_MB_MAX]={"CAR2000","CAR3000","NAR7090",\
                                      "NAR7100","CAR4000","CAR5000",\
                                      "CAR5010","CAR4010","CAR3020",\
                                      "CAR5020","CAR3030","CAR5030",\
                                      "CPT4010","CPT3210","CAR4018",\
                                      "CAR4211C","CAR4212C","COS9200",\
                                      "NAR5520","CAR3040","COS5700",\
                                      "CAR4020","CAR2051","CAR3050",\
                                      "CAR5040","CAR3231C-LD","CAR3042C-LD"};

bp_unit mb_bypass_info[BP_MB_MAX]={
	/*{ board index, bypass type, total segs, ctrl addr, wdt addr, period addr, 9559 addr, generation 
	  pci ini data}*/
	/*set CAR-2000 motherboard configuration*/	
	{BP_MB_CAR2000, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x22}, {0x0,0x0}, {0x26,0x26}, {0x4f,0x4f}, 0, BP_GEN2, NULL},
		
	/*set CAR-3000 motherboard configuration*/	
	{BP_MB_CAR3000, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x22}, {0x0,0x0}, {0x26,0x26}, {0x4f,0x4f}, 0, BP_GEN2, NULL},
	{},
	{},
	{},
	{},
	{},
	/*set CAR4010 motherboard configuration*/
	{BP_MB_CAR4010, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x22}, {0x0,0x0}, {0x26,0x26}, {0x4f,0x4f}, 0, BP_GEN2, NULL},
	/*set CAR3020 motherboard configuration*/
	{BP_MB_CAR3020, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x22}, {0x0,0x0}, {0x26,0x26}, {0x4f,0x4f}, 0, BP_GEN2, NULL},
	{},
	/*set CAR3030 motherboard configuration*/
	{BP_MB_CAR3030, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x22}, {0x0,0x0}, {0x26,0x26}, {0x4f,0x4f}, 0, BP_GEN2, NULL},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	/*set NAR5520 motherboard configuration*/
	{BP_MB_NAR5520, BP_COPPER_CPLDV2, BP_SEGS_2, \
	{0x26, 0x21}, {0x0, 0x0}, {0x26, 0x26}, {0x4f, 0x4f}, 0, BP_GEN2, NULL},
	{},
	{},
	{},
	{},
	{},
	{},
	{},
	{}
};

int gen2bp_init(char *board, int *card_conf);

#endif	/*__INCLUDE_PLATEFORM_DATA__*/

#endif
