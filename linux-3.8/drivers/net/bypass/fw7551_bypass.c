#include <linux/kernel.h>
#include <asm/io.h>
#include "fw7551_bypass.h"


static int get_pair_no_by_port(int port)
{
	int pair_no;

	/* ZEN 1K only 1Pair bypass (eth4, eth5) */
	if (port == 0)
		pair_no = BYPASS_PAIR_2;
	else
		pair_no = 0;

	return pair_no;
}

static void enter_w83627_config(void)
{
	outportb(INDEX_PORT, 0x87); // Must Do It Twice
	outportb(INDEX_PORT, 0x87);
	return;
}

static void exit_w83627_config(void)
{
	outportb(INDEX_PORT, 0xAA);
	return;
}

static unsigned char read_w83627_reg(int LDN, int reg)
{
	unsigned char tmp = 0;

	enter_w83627_config();

	outportb(INDEX_PORT, 0x07); // LDN Register
	outportb(DATA_PORT, LDN); // Select LDNx
	outportb(INDEX_PORT, reg); // Select Register
	tmp = inportb(DATA_PORT); // Read Register

	exit_w83627_config();

	return tmp;
}

static void write_w83627_reg(int LDN, int reg, int value)
{
	enter_w83627_config();

	outportb(INDEX_PORT, 0x07); // LDN Register
	outportb(DATA_PORT, LDN); // Select LDNx
	outportb(INDEX_PORT, reg); // Select Register
	outportb(DATA_PORT, value); // Write Register

	exit_w83627_config();
}

int w83627chip(int reg)
{
	unsigned short chipid = 0;

	enter_w83627_config();
	chipid = (read_w83627_reg(reg, 0x20) << 8);
	chipid |= read_w83627_reg(reg, 0x21);
	exit_w83627_config();

	if (chipid == W83627DHG_CHIP_ID)
		return 0;

	return -1;
}

static void set_bypass_enable_when_system_off(int pair_no)
{
	int reg_no, ldn_no;
	unsigned char tmp, bit_mask, en_data;

	reg_no = ldn_no = bit_mask = en_data = tmp = 0;

	switch (pair_no) {
		case BYPASS_PAIR_1:
			ldn_no = OFFMODE_BYPASS_PAIR1_LDN;
			reg_no = OFFMODE_BYPASS_PAIR1_REG;
			bit_mask = OFFMODE_BYPASS_PAIR1_BIT;
			en_data = OFFMODE_BYPASS_PAIR1_ENABLE;

			break;
		case BYPASS_PAIR_2:
			ldn_no = OFFMODE_BYPASS_PAIR2_LDN;
			reg_no = OFFMODE_BYPASS_PAIR2_REG;
			bit_mask = OFFMODE_BYPASS_PAIR2_BIT;
			en_data = OFFMODE_BYPASS_PAIR2_ENABLE;

			break;

		default:
			return;
	}

	tmp = read_w83627_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask);
	tmp |= en_data;
	write_w83627_reg(ldn_no, reg_no, tmp);

	/* BIOS off-mode bypass enable */
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS); 
	tmp = inportb(CMOS_DATA_PORT);
	if(pair_no == BYPASS_PAIR_1)
		tmp |= 0x04;
	else  // BYPASS_PAIR_2
		tmp |= 0x40;
	outportb(CMOS_DATA_PORT, tmp);
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS_CHK);
	outportb(CMOS_DATA_PORT, ~tmp);

	return;
}

static void set_bypass_disable_when_system_off(int pair_no)
{
	int reg_no, ldn_no;
	unsigned char tmp, bit_mask, en_data;

	reg_no = ldn_no = bit_mask = en_data = tmp = 0;

	switch (pair_no) {
		case BYPASS_PAIR_1:
			ldn_no = OFFMODE_BYPASS_PAIR1_LDN;
			reg_no = OFFMODE_BYPASS_PAIR1_REG;
			bit_mask = OFFMODE_BYPASS_PAIR1_BIT;
			en_data = OFFMODE_BYPASS_PAIR1_DISABLE;

			break;
		case BYPASS_PAIR_2:
			ldn_no = OFFMODE_BYPASS_PAIR2_LDN;
			reg_no = OFFMODE_BYPASS_PAIR2_REG;
			bit_mask = OFFMODE_BYPASS_PAIR2_BIT;
			en_data = OFFMODE_BYPASS_PAIR2_DISABLE;

			break;

		default:
			return;
	}

	tmp = read_w83627_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask);
	tmp |= en_data;
	write_w83627_reg(ldn_no, reg_no, tmp);

	/* BIOS off-mode bypass disable */
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS); 
	tmp = inportb(CMOS_DATA_PORT);
	if(pair_no == BYPASS_PAIR_1)
		tmp &= ~0x04;
	else  // BYPASS_PAIR_2
		tmp &= ~0x40;
	outportb(CMOS_DATA_PORT, tmp);
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS_CHK);
	outportb(CMOS_DATA_PORT, ~tmp);

	return;

}

static void set_runtime_bypass_enable(int pair_no)
{
	int reg_no, ldn_no;
	unsigned char tmp, bit_mask, en_data;

	reg_no = ldn_no = bit_mask = en_data = tmp = 0;

	set_bypass_enable_when_system_off(pair_no);

	switch (pair_no) {
		case BYPASS_PAIR_1:
			ldn_no = RUNTIME_BYPASS_PAIR1_LDN;
			reg_no = RUNTIME_BYPASS_PAIR1_REG;
			bit_mask = RUNTIME_BYPASS_PAIR1_BIT;
			en_data = RUNTIME_BYPASS_PAIR1_ENABLE;

			break;

		case BYPASS_PAIR_2:
			ldn_no = RUNTIME_BYPASS_PAIR2_LDN;
			reg_no = RUNTIME_BYPASS_PAIR2_REG;
			bit_mask = RUNTIME_BYPASS_PAIR2_BIT;
			en_data = RUNTIME_BYPASS_PAIR2_ENABLE;

			break;

		default:
			return;
	}

	tmp = read_w83627_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask);
	tmp |= en_data;
	write_w83627_reg(ldn_no, reg_no, tmp);
	
	/* BIOS runtime bypass enable */
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS);
	tmp = inportb(CMOS_DATA_PORT);
	if(pair_no == BYPASS_PAIR_1)
		tmp |= 0x01;
	else  // BYPASS_PAIR_2
		tmp |= 0x10;
	outportb(CMOS_DATA_PORT, tmp);
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS_CHK);
	outportb(CMOS_DATA_PORT, ~tmp);

	return;
}

static void set_runtime_bypass_disable_bios_set(int pair_no)
{
	int reg_no, ldn_no;
	unsigned char tmp, bit_mask, en_data;

	reg_no = ldn_no = bit_mask = en_data = tmp = 0;

	switch (pair_no) {
		case BYPASS_PAIR_1:
			ldn_no = RUNTIME_BYPASS_PAIR1_LDN;
			reg_no = RUNTIME_BYPASS_PAIR1_REG;
			bit_mask = RUNTIME_BYPASS_PAIR1_BIT;
			en_data = RUNTIME_BYPASS_PAIR1_DISABLE;

			break;

		case BYPASS_PAIR_2:
			ldn_no = RUNTIME_BYPASS_PAIR2_LDN;
			reg_no = RUNTIME_BYPASS_PAIR2_REG;
			bit_mask = RUNTIME_BYPASS_PAIR2_BIT;
			en_data = RUNTIME_BYPASS_PAIR2_DISABLE;

			break;

		default:
			return;
	}

	tmp = read_w83627_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask);
	tmp |= en_data;
	write_w83627_reg(ldn_no, reg_no, tmp);
	
	/* BIOS runtime bypass enable */
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS); 
	tmp = inportb(CMOS_DATA_PORT);
	if(pair_no == BYPASS_PAIR_1)
		tmp |= 0x01;
	else  // BYPASS_PAIR_2
		tmp |= 0x10;
	outportb(CMOS_DATA_PORT, tmp);
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS_CHK);
	outportb(CMOS_DATA_PORT, ~tmp);

	return;
}

static void set_runtime_bypass_disable(int pair_no)
{
	int reg_no, ldn_no;
	unsigned char tmp, bit_mask, en_data;

	reg_no = ldn_no = bit_mask = en_data = tmp = 0;

	switch (pair_no) {
		case BYPASS_PAIR_1:
			ldn_no = RUNTIME_BYPASS_PAIR1_LDN;
			reg_no = RUNTIME_BYPASS_PAIR1_REG;
			bit_mask = RUNTIME_BYPASS_PAIR1_BIT;
			en_data = RUNTIME_BYPASS_PAIR1_DISABLE;

			break;

		case BYPASS_PAIR_2:
			ldn_no = RUNTIME_BYPASS_PAIR2_LDN;
			reg_no = RUNTIME_BYPASS_PAIR2_REG;
			bit_mask = RUNTIME_BYPASS_PAIR2_BIT;
			en_data = RUNTIME_BYPASS_PAIR2_DISABLE;

			break;

		default:
			return;
	}

	tmp = read_w83627_reg(ldn_no, reg_no);
	tmp &= ~(bit_mask);
	tmp |= en_data;
	write_w83627_reg(ldn_no, reg_no, tmp);
	
	/* BIOS runtime bypass disable */
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS); 
	tmp = inportb(CMOS_DATA_PORT);
	if(pair_no == BYPASS_PAIR_1)
		tmp &= ~0x01;
	else  // BYPASS_PAIR_2
		tmp &= ~0x10;
	outportb(CMOS_DATA_PORT, tmp);
	outportb(CMOS_INDEX_PORT, CMOS_BYPASS_CHK);
	outportb(CMOS_DATA_PORT, ~tmp);

	return;
}

void init_7551_gpio(void)
{
	unsigned char tmp;

	/* Set W83627 multiplex pin to WDTO function */
	tmp=read_w83627_reg(0x00, 0x2d);
	tmp &= ~(0x01);
	write_w83627_reg(0x00, 0x2d, tmp);

	/* active GIPO4 group */
	tmp = read_w83627_reg(0x09, 0x30);
	tmp |= 4;
	write_w83627_reg(0x09, 0x30, tmp);

	/* Set GPIO40~45 to output mode */
	tmp = read_w83627_reg(0x09, 0xF4);
	tmp &= ~(SIO_GPIO_40_BIT + SIO_GPIO_41_BIT + SIO_GPIO_42_BIT + SIO_GPIO_43_BIT + SIO_GPIO_44_BIT + SIO_GPIO_45_BIT);
	write_w83627_reg(0x09, 0xF4, tmp);
}

void zen_7551_init(int port)
{
	int pair_no = 0;

	pair_no = get_pair_no_by_port(port);

	if (!pair_no)
		return;

	/* power on : pass, power off : bypass , power on : bypass from bios */
	set_runtime_bypass_disable_bios_set(pair_no);
	set_bypass_enable_when_system_off(pair_no);
}

void zen_7551_enable(int port)
{
	int pair_no = 0;

	pair_no = get_pair_no_by_port(port);

	if (!pair_no)
		return;

	/* power on : bypass, power off : bypass */
	set_runtime_bypass_enable(pair_no);
}

void zen_7551_disable(int port)
{
	int pair_no = 0;

	pair_no = get_pair_no_by_port(port);

	if (!pair_no)
		return;

	/* power on : pass, power off : pass */
	set_runtime_bypass_disable(pair_no);
	set_bypass_disable_when_system_off(pair_no);
}
