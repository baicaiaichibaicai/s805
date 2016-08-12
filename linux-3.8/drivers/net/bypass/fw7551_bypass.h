#ifndef __FW7551_BYPASS_H__
#define __FW7551_BYPASS_H__

#define W83627DHG_CHIP_ID		0xB073

#define W83627_CTL_REG			0x00

/* device depend definitions */
#define INDEX_PORT				0x2E
#define DATA_PORT   			0x2F
#define GPIO_PORT				0x500

#define CMOS_INDEX_PORT			0x70
#define CMOS_DATA_PORT 			0x71
#define CMOS_BYPASS				0x50
#define CMOS_BYPASS_CHK			0x51

#define SIO_GPIO_2X				1
#define SIO_GPIO_23_BIT			(1<<3)

#define SIO_GPIO_40_BIT			(1<<0)
#define SIO_GPIO_41_BIT			(1<<1)
#define SIO_GPIO_42_BIT			(1<<2)
#define SIO_GPIO_43_BIT			(1<<3)
#define SIO_GPIO_44_BIT			(1<<4)
#define SIO_GPIO_45_BIT			(1<<5)

/* Runtime bypass definitions */
#define RUNTIME_BYPASS_PAIR1_LDN	(9)
#define RUNTIME_BYPASS_PAIR1_REG	(0xF5)
#define RUNTIME_BYPASS_PAIR1_BIT	(SIO_GPIO_40_BIT)
#define RUNTIME_BYPASS_PAIR1_ENABLE	(SIO_GPIO_40_BIT)
#define RUNTIME_BYPASS_PAIR1_DISABLE	(0)

#define RUNTIME_BYPASS_PAIR2_LDN	(9)
#define RUNTIME_BYPASS_PAIR2_REG	(0xF5)
#define RUNTIME_BYPASS_PAIR2_BIT	(SIO_GPIO_41_BIT)
#define RUNTIME_BYPASS_PAIR2_ENABLE	(SIO_GPIO_41_BIT)
#define RUNTIME_BYPASS_PAIR2_DISABLE	(0)

/* Offmode bypass definitions */
#define OFFMODE_BYPASS_PAIR1_LDN	(9)
#define OFFMODE_BYPASS_PAIR1_REG	(0xF5)
#define OFFMODE_BYPASS_PAIR1_BIT	(SIO_GPIO_42_BIT | SIO_GPIO_43_BIT)
#define OFFMODE_BYPASS_PAIR1_ENABLE	SIO_GPIO_42_BIT
#define OFFMODE_BYPASS_PAIR1_DISABLE	SIO_GPIO_43_BIT

#define OFFMODE_BYPASS_PAIR2_LDN	(9)
#define OFFMODE_BYPASS_PAIR2_REG	(0xF5)
#define OFFMODE_BYPASS_PAIR2_BIT	(SIO_GPIO_44_BIT | SIO_GPIO_45_BIT)
#define OFFMODE_BYPASS_PAIR2_ENABLE	SIO_GPIO_44_BIT
#define OFFMODE_BYPASS_PAIR2_DISABLE	SIO_GPIO_45_BIT

/* Pair definitions */
#define BYPASS_PAIR_1			(0x1 << 4)
#define BYPASS_PAIR_2			(0x2 << 4)
#define BYPASS_PAIR_3			(0x4 << 4)
#define BYPASS_PAIR_4			(0x8 << 4)
#define BYPASS_PAIR_5			(0x10 << 4)
#define BYPASS_PAIR_6			(0x20 << 4)

#define outportb(x, y) outb_p(y, x)
#define inportb(x) inb_p(x)

void init_7551(void);
void zen_7551_init(int port);
void zen_7551_enable(int port);
void zen_7551_disable(int port);
void init_7551_gpio(void);

int w83627chip(int reg);
#endif	// __FW7551_BYPASS_H__
