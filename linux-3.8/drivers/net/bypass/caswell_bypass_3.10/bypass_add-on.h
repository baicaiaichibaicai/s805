#ifndef _BYPASS_ADD_ON_H_
#define _BYPASS_ADD_ON_H_

#include "bypass_pci.h"

#define BP_CARD_ABN484  0
#define BP_CARD_ABN482  1
#define BP_CARD_ABN484L 2

#define BP_CARD_ABN1010 0
#define BP_CARD_ABN1004 1
#define BP_CARD_ABN1000 2

#ifndef __INCLUDE_PLATEFORM_DATA__
#if (defined(X86_TARGET) || defined(MIPS_CAVIUM_TARGET))

/* The bypass add-card definition */
#define PCI_BP_CARD_MAX             7
#define BP_CARD_ABN484  0
#define BP_CARD_ABN482  1
#define BP_CARD_ABN484L 2
#define BP_CARD_NIP5514 3
#define BP_CARD_ABN426  4
#define BP_CARD_NIP52240 5
#define BP_CARD_NIP54121 6

unsigned char * PCI_BP_CARD_NAME[PCI_BP_CARD_MAX] = {"ABN484", "ABN482", "ABN484L","NIP5514","ABN426","NIP52240","NIP54121"};

bp_unit pci_addcard_bypass_info[PCI_ADDCARD_MAX_SLOTS][PCI_BP_CARD_MAX] = {
	/*{ board index, bypass type, total segs, ctrl addr, wdt addr, period addr, 9559 addr, generation }*/
        {
                /*set add-on-card configuration if slot is 0*/
                {BP_CARD_ABN484, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x20, 0x24}, {0x0,0x0},{0x20,0x20}, {0x4c,0x4c}, 0x20, BP_GEN2, & abn484_bp_pci_initdata},

                {BP_CARD_ABN482, BP_FIBER_PIC_ABN482, BP_SEGS_1, \
                {0x20}, {0x24}, {0x24}, {0x0}, 0, BP_GEN2, & abn482_bp_pci_initdata},

                {BP_CARD_ABN484L, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x20, 0x24},{0x0,0x0},{0x20,0x20},{0x4c,0x4c}, 0x20, BP_GEN2, NULL},

                {BP_CARD_NIP5514, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x0, 0x24},{0x0,0x0},{0x20,0x20},{0x4c,0x4c}, 0x20, BP_GEN2, NULL},

                {BP_CARD_ABN426, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x24, 0x21}, {0x0,0x0},{0x24,0x24}, {0x4c,0x4c}, 0x24, BP_GEN2, NULL},

		{BP_CARD_NIP52240, BP_FIBER_PIC_ABN482, BP_SEGS_2, \
		{0x20, 0x74},{0x24,0x74},{0x24,0x24},{0x0,0x0}, 0x0, BP_GEN2, NULL},

		{BP_CARD_NIP54121, BP_COPPER_CPLDV2, BP_SEGS_1, \
		{0x20, 0x24},{0x0,0x0},{0x20,0x20},{0x4c,0x4c}, 0x20, BP_GEN2, NULL},
        }, {
                /*set add-on-card configuration if slot is 1*/
                {BP_CARD_ABN484, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x21, 0x25}, {0x0,0x0},{0x21,0x21}, {0x4d,0x4d}, 0x21, BP_GEN2, & abn484_bp_pci_initdata},

				{BP_CARD_ABN482, BP_FIBER_PIC_ABN482, BP_SEGS_1, \
                {0x21}, {0x25}, {0x25}, {0x0}, 0, BP_GEN2, & abn482_bp_pci_initdata},

                {BP_CARD_ABN484L, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x21, 0x25},{0x0,0x0},{0x21,0x21},{0x4d,0x4d}, 0x21, BP_GEN2, NULL},

                {BP_CARD_NIP5514, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x0, 0x25},{0x0,0x0},{0x21,0x21},{0x4d,0x4d}, 0x21, BP_GEN2, NULL},

                {BP_CARD_ABN426, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x24, 0x21}, {0x0,0x0},{0x24,0x24}, {0x4c,0x4c}, 0x24, BP_GEN2, NULL},
		
		{BP_CARD_NIP52240, BP_FIBER_PIC_ABN482, BP_SEGS_2, \
		{0x21, 0x75},{0x25,0x75},{0x25,0x25},{0x0,0x0}, 0x0, BP_GEN2, NULL},
		
		{BP_CARD_NIP54121, BP_COPPER_CPLDV2, BP_SEGS_1, \
		{0x21, 0x25},{0x0,0x0},{0x21,0x21},{0x4d,0x4d}, 0x21, BP_GEN2, NULL},
        }, {
                /*set add-on-card configuration if slot is 2*/
                {BP_CARD_ABN484, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x22, 0x26}, {0x0,0x0},{0x22,0x22}, {0x4e,0x4e}, 0x22, BP_GEN2, & abn484_bp_pci_initdata},

                {BP_CARD_ABN482, BP_FIBER_PIC_ABN482, BP_SEGS_1, \
                {0x22}, {0x26}, {0x26}, {0x0}, 0, BP_GEN2, & abn482_bp_pci_initdata},

                {BP_CARD_ABN484L, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x22, 0x26},{0x0,0x0},{0x22,0x22},{0x4e,0x4e}, 0x22, BP_GEN2, NULL},

                {BP_CARD_NIP5514, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x0, 0x26},{0x0,0x0},{0x22,0x22},{0x4e,0x4e}, 0x22, BP_GEN2, NULL},

                {BP_CARD_ABN426, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x24, 0x21}, {0x0,0x0},{0x24,0x24}, {0x4c,0x4c}, 0x24, BP_GEN2, NULL},
		
		{BP_CARD_NIP52240, BP_FIBER_PIC_ABN482, BP_SEGS_2, \
		{0x22, 0x76},{0x26,0x76},{0x26,0x26},{0x0,0x0}, 0x0, BP_GEN2, NULL},

		{BP_CARD_NIP54121, BP_COPPER_CPLDV2, BP_SEGS_1, \
		{0x22, 0x26},{0x0,0x0},{0x22,0x22},{0x4e,0x4e}, 0x22, BP_GEN2, NULL},
        }, {
                /*set add-on-card configuration if slot is 3*/
                {BP_CARD_ABN484, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x23, 0x27}, {0x0,0x0},{0x23,0x23}, {0x4f,0x4f}, 0x23, BP_GEN2, & abn484_bp_pci_initdata},

                {BP_CARD_ABN482, BP_FIBER_PIC_ABN482, BP_SEGS_1, \
                {0x23}, {0x27}, {0x27}, {0x0}, 0, BP_GEN2, & abn482_bp_pci_initdata},

                {BP_CARD_ABN484L, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x23, 0x27},{0x0,0x0},{0x23,0x23},{0x4f,0x4f}, 0x23, BP_GEN2, NULL},

                {BP_CARD_NIP5514, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x0, 0x27},{0x0,0x0},{0x23,0x23},{0x4f,0x4f}, 0x23, BP_GEN2, NULL},

                {BP_CARD_ABN426, BP_COPPER_CPLDV2, BP_SEGS_2, \
                {0x24, 0x21}, {0x0,0x0},{0x24,0x24}, {0x4c,0x4c}, 0x24, BP_GEN2, NULL},

		{BP_CARD_NIP52240, BP_FIBER_PIC_ABN482, BP_SEGS_2, \
		{0x23, 0x77},{0x27,0x77},{0x27,0x27},{0x0,0x0}, 0x0, BP_GEN2, NULL},

		{BP_CARD_NIP54121, BP_COPPER_CPLDV2, BP_SEGS_1, \
		{0x23, 0x27},{0x0,0x0},{0x23,0x23},{0x4f,0x4f}, 0x23, BP_GEN2, NULL},
       }

};
#endif  /*X86_TARGET || MIPS_CAVIUM_TARGET*/

#ifdef X86_TARGET
#define ADDCARD_MAX_SLOTS           PCI_ADDCARD_MAX_SLOTS
#define BP_CARD_MAX_PLATEFORM	    PCI_BP_CARD_MAX
#define BP_CARD_NAME		    PCI_BP_CARD_NAME
#define ADDCARD_BYPASS_INFO	    pci_addcard_bypass_info
#endif	/*X86_TARGET*/

#endif /*__INCLUDE_PLATEFORM_DATA__*/

#endif
