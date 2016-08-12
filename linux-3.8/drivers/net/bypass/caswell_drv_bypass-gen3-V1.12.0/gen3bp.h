/*******************************************************************************

  CASwell(R) Gen3 Bypass Linux driver
  Copyright(c) 2012 Zeno Lai <zeno.lai@cas-well.com>

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

*******************************************************************************/

#ifndef _GEN3BP_H_
#define _GEN3BP_H_

/** Define Generations **/
#define BPGEN30                   0x3000
#define BPGEN31                   0x3100

/** Gen3 Bypass Commands **/
#define GEN3BP_GET_BPE            0x300
#define GEN3BP_SET_BPE            0x301
#define GEN3BP_GET_NEXTBOOT       0x302
#define GEN3BP_SET_NEXTBOOT       0x303
#define GEN3BP_GET_FUNC           0x304
#define GEN3BP_SET_FUNC           0x305
#define GEN3BP_WDT_COUNTDOWN      0x306
#define GEN3BP_GET_RELAY          0x307
#define GEN3BP_SET_RELAY          0x308
#define GEN3BP_HW_ENA             0x309
#define GEN3BP_RESET              0x30A

/** Define Control OPCode **/
#define BP_OPCODE_LEN             3
#define BP_OPCODE_WRITE           0x02
#define BP_OPCODE_READ            0x03

#define BP_OPCODE_PERIOD          0x0000     /* Get and Set period */
#define BP_OPCODE_WDT_LOAD        0x0100
#define BP_OPCODE_WDT_DISABLE     0x0200
#define BP_OPCODE_WDT_STS         0x0300     /* Get and Reset WDT status */
#define BP_OPCODE_BPE             0x0400     /* Get and Set BPE */
#define BP_OPCODE_NEXTBOOT        0x0500     /* Get and Set Nextboot */
#define BP_OPCODE_BP_SET_NORM     0x0600
#define BP_OPCODE_BP_SET_NON      0x0700
#define BP_OPCODE_BP_STS          0x0800
#define BP_OPCODE_WDT_COUNTDOWN   0x0900     /* WDT countdown timer */
#define BP_OPCODE_ACT_WDT_STS     0x0A00     /* WDT current status */
#define BP_OPCODE_SW_ENABLE       0x0B00     /* Bypass functions EN/DIS */
#define BP_OPCODE_WDT_RS          0x0C00     /* WDT active */
#define BP_OPCODE_INTERN_RST      0x0C01     /* Internal Reset */
#define BP_OPCODE_RLYE_ENABLE     0x0D00     /* Relay EN/DIS (internal use) */
#define BP_OPCODE_HW_ENABLE       0x0E00     /* H/W jumper status */
#define BP_OPCODE_UPFMY           0x1000
#define BP_OPCODE_UPVER_MINOR     0x1100     /* Firmware minor version */
#define BP_OPCODE_ONBOARD         0x1200
#define BP_OPCODE_SLOT_ID         0x1300     /* (Reserved) */
#define BP_OPCODE_SEG_ID          0x1400     /* (Reserved) */
#define BP_OPCODE_LAN_TYPE        0x1500
#define BP_OPCODE_BPID            0x2000     /* Get and Set bypass id */
#define BP_OPCODE_UPVER_MAJOR     0x2100     /* Firmware major version */

/* CPLD setting */
#define BP_CPLD_CLK                1          /* microsecond */
#define BP_CPLD_ATTEMPTS           1000

/* MCU delay time */
#define BP_MCU_CLK                 2          /* microsecond */
#define BP_MCU_BYTE_DELAY          40         /* microsecond */
#define READ_DELAY                 4          /* microsecond */
#define WRITE_DELAY                4          /* microsecond */
#define EEPROM_DELAY               4          /* millisecond */
#define RELAY_DELAY	               7          /* millisecond */
#define RESET_DELAY                250        /* millisecond */

#define TWDMAX                     0xFF       /* Max Twd */

extern int gen3bp_init(void);
extern void gen3bp_exit(void);
extern void gen3bp_reboot_exit(void);

#endif
