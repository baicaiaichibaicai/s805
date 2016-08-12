#ifndef _LANNER_PSU_READ_H
#define _LANNER_PSU_READ_H

#ifdef CONFIG_ARM
#include <asm/io.h>
#endif
#define inportb(x)  inb(x)
#define outportb(x, y)  outb(y, x)
#define inportw(x)  inw(x)
#define outportw(x, y)  outw(y, x)
#define inportl(x)  inl(x)
#define outportl(x, y)  outl(y, x)
#define delay(x)    msleep(x/10)
#define sleep(x)    msleep(x*1000)
#define PCI_INDEX               0xcf8
#define PCI_DATA                0xcfc
#define LANNER_PSU_DEBUG        0

/* Lanner PMBUS COMMAND */
#define READ_PIN                0x97
#define READ_VIN                0x88
#define STATUS_BYTE             0x78
#define STATUS_WORD             0x79
#define STATUS_VOUT             0x7a
#define STATUS_IOUT             0x7b
#define STATUS_INPUT            0x7c
#define STATUS_TEMPERATURE      0x7d
#define STATUS_CML              0x7e
#define STATUS_OTHER            0x7f

#define STATUS_MFR_SPECIFIC     0x80
#define STATUS_FANS_1_2         0x81
#define STATUS_FANs_3_4         0x82
/* Lanner 6K parameters */
#define PMBUS_REVISION          0x98
#define MFR_ID               	0x99
#define MFR_MODEL            	0x9a
#define MFR_REVISION         	0x9b
#define MFR_MODEL_OPTION     	0xd0
#define MFR_LOCATION         	0x9c
#define MFR_DATE             	0x9d
#define MFR_SERIAL           	0x9e
#define MFR_FW_ID            	0xae
#define MFR_FW_REVISION      	0xd2
#define MFR_FW_DATE          	0xd4
/*Lanner 3K parameters */
#define MFR_3K_FW_ID            0x5b
#define FW_3K_ID_START			0x4b
#define FW_3K_ID_END 			0x5a

#define VOUT_MODE   			0x20
#define READ_VOUT               0x8b
#define READ_IOUT               0x8c
#define READ_TEMPTURE_1         0x8d
#define READ_TEMPTURE_2         0x8e
#define READ_TEMPTURE_3         0x8f

#define READ_FAN_SPEED_1        0x90
#define READ_FAN_SPEED_2        0x91
#define READ_FAN_SPEED_3        0x92
#define READ_FAN_SPEED_4        0x93
#define READ_FREQUENCY          0x95
#define FAN_CONFIG_1_2          0x3a
#define FAN_COMMAND_1           0x3b
#define FAN_CONFIG_3_4          0x3d
#define MODULE_NUM              3   /* PSU1,PSU2,BP */
#define FW_3K_ID_LEN            16

enum {
	MFR_IS_ZEN3K=1,
	MFR_IS_ZEN6K,
};

unsigned int lanner_psu_get_gpiobase(void);
int lanner_psu_get_smbase(void);
int lanner_psu_write_byte(int cslaveadd, int byte_c1, int byted1);
int lanner_psu_read_byte(int cslaveadd, int byte_c);
int lanner_psu_read_word(int cslaveadd, int byte_c);
int lanner_psu_write_byte_pec(int cslaveadd, int byte_c1, int byted1, int pec_value);
int lanner_psu_value_convert(int word_value);
int lanner_psu_value_convert_pec(int word_value, char mode_byte);
#endif
