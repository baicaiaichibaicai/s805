/* F71889E-Chip
 *  * Super-I/O constants and functions 
 *   */
#ifndef F71889E_H
#define F71889E_H

/* default */
#define DEVNAME	"F71889E"

#define F71889E_INDEX		0x4e
#define F71889E_DATA		0x4f
/* Pull down the SOUT1 pin */
#define SOUT1_F71889E_INDEX	0x2e
#define SOUT1_F71889E_DATA	0x2f

#define F71889E_LD_GPIO          0x06

#define F71889E_REG_LDSEL           0x07    /* Logical device select */
#define F71889E_REG_DEVID           0x20    /* Device ID (2 bytes) */
#define F71889E_REG_DEVREV          0x22    /* Device revision */
#define F71889E_REG_MANID           0x23    /* Fintek ID (2 bytes) */
#define F71889E_REG_FNSEL1          0x29    /* Multi Function Select 1 (F71872F) */
#define F71889E_REG_ENABLE          0x30    /* Logical device enable */
#define F71889E_REG_ADDR            0x60    /* Logical device address (2 bytes) */

#define F71889E_SIO_FINTEK_ID           0x1934
#define F71889E_SIO_F71889E_ID          0x1005

#define REGION_LENGTH		8

#define F71889E_GPIO_47		7
#define F71889E_GPIO_46		6
#define F71889E_GPIO_45		5
#define F71889E_GPIO_44		4
#define F71889E_GPIO_43		3
#define F71889E_GPIO_42		2
#define F71889E_GPIO_41		1
#define F71889E_GPIO_40		0

#define CPLD_TEST	F71889E_GPIO_40
#define CPLD_PAIR3	F71889E_GPIO_41
#define CPLD_PAIR2	F71889E_GPIO_42
#define CPLD_PAIR1	F71889E_GPIO_43
#define CPLD_CFG3	F71889E_GPIO_44
#define CPLD_CFG2	F71889E_GPIO_45
#define CPLD_CFG1	F71889E_GPIO_46
#define CPLD_SENDBIT	F71889E_GPIO_47

#define F71889E_REG_GPIO_EN	0xb0
#define F71889E_REG_GPIO_DATA	0xb1
#define F71889E_REG_GPIO_STATUS	0xb2


#endif
