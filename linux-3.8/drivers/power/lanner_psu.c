#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include "lanner_psu.h"

extern int smbase, smb_err;
const int psu_exp[32] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 
	1024, 	2048, 4096, 8192, 16384, 32768, 65536, 32768, 16384, 
	8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2};

unsigned int lanner_psu_get_gpiobase(void)
{
	outportl(PCI_INDEX, (0x80000000+(0<<16)+(31<<11)+(0<<8)+0x48)); // Intel LPC bridge
	return (inportl(PCI_DATA));
}

int lanner_psu_get_smbase(void)
{
	outportl(PCI_INDEX, (0x80000000+(0<<16)+(31<<11)+(3<<8)+0x20));
	return ((inportw(PCI_DATA))&0xfffe);
}

int lanner_psu_write_byte(int cslaveadd, int byte_c1, int byted1)
{
	int i;
	smb_err = 0x00; // clear status flag
	for(i=0x00;i<100;i++) {
		outportb(smbase, 0xff); // clear status
		delay(10);
		if((inportb(smbase+0x00)&0x01)==0x00)
			break;
		if(i==0x99) {
			smb_err = 0x01;
#if LANNER_PSU_DEBUG
			printk("Slave Address=0x%x, device smbus access timeout.\n", cslaveadd);
#endif
		}
	}
	outportb(smbase+2, (inportb(smbase+2)&0x7F)); // disable pec first
	outportb(smbase+3, byte_c1); // byte N
	delay(5);
	outportb(smbase+4, cslaveadd&0xfe); // slave address and write
	delay(5);
	outportb(smbase+5, byted1); // byte data access
	delay(5);
	outportb(smbase+2, 0x48); // byte data access
	delay(2);

	if(((inportb(smbase))&0x04)==0x04) { // check status fail?
#if LANNER_PSU_DEBUG
		printk("Slave Address = 0x%x, device not exist or smbus access error.\n", cslaveadd);
#endif
		smb_err = 0x02;
	}
	if(smb_err!=0x00) {
#if LANNER_PSU_DEBUG
		printk("smbus access error exit.\n");
#endif
		return -1;
	}
	return (0x00);
}

int lanner_psu_read_byte(int cslaveadd, int byte_c)
{
	int i;
	smb_err = 0x00; // claer status flag
	for(i=0x00;i<100;i++) {
		outportb(smbase, 0xff); // clear status
		delay(10);
		if((inportb(smbase+0x00)&0x01)==0x00)
			break;
		if(i==0x99) {
			smb_err = 0x01;
#if LANNER_PSU_DEBUG
			printk("Slave Address = 0x%x, device smbus access timeout.\n", cslaveadd);
#endif
		}
	}

	outportb(smbase+2, (inportb(smbase+2)&0x7F)); // disable pec first
	outportb(smbase+3, byte_c); // byte N
	delay(5);
	outportb(smbase+4, cslaveadd|0x01); // slave address and read
	delay(5);
	outportb(smbase+2, 0x48); // byte data access
	delay(20);

	if(((inportb(smbase))&0x04)==0x04) { //check status fail?
#if LANNER_PSU_DEBUG
		printk("Slave Address = 0x%x, device not exist or smbus access error.\n", cslaveadd);
#endif
		smb_err = 0x02;
	}
	if(smb_err!=0x00) {
#if LANNER_PSU_DEBUG
		printk("smbus access error exit.\n");
#endif
		return -1;
	}

	return (inportb(smbase+5)); // byte data access
}

int lanner_psu_read_word(int cslaveadd, int byte_c)
{
	int i;
	smb_err = 0x00; // clear status flag

	for(i=0x00;i<100;i++) {
		outportb(smbase, 0xff); // clear status
		if((inportb(smbase+0x00)&0x01)==0x00)
			break;
		if(i==0x99) {
			smb_err = 0x01;
#if LANNER_PSU_DEBUG
			printk("Slave Address = 0x%x, device smbus access timeout.\n", cslaveadd);
#endif
		}
	}

	outportb(smbase+2, (inportb(smbase+2)&0x7F)); // disable pec first
	outportb(smbase+3, byte_c); // byte N
	delay(5);
	outportb(smbase+4, cslaveadd|0x01); // slave address and read
	delay(5);
	outportb(smbase+2, 0x4c); // byte data access
	delay(20);

	if(((inportb(smbase))&0x04)==0x04) { // check status fail?
#if LANNER_PSU_DEBUG
		printk("Slave Address = 0x%x, device not exist or smbus access error.\n", cslaveadd);
#endif
		smb_err = 0x02;
	}

	if(smb_err!=0x00) {
#if LANNER_PSU_DEBUG
		printk("smbus access error exit.\n");
#endif
		return -1;
	}

	return (inportw(smbase+5)); // byte data access
}

int lanner_psu_write_byte_pec(int cslaveadd, int byte_c1, int byted1, int pec_value)
{
	int i;
	smb_err = 0x00; // clear status flag

	for(i=0x00;i<100;i++) {
		outportb(smbase, 0xff); //clear status
		delay(10);
		if((inportb(smbase+0x00)&0x01) == 0x00)
			break;
		if(i==0x99) {
#if LANNER_PSU_DEBUG
			printk("Slave Address = 0x%x, device smbus access timeout.\n", cslaveadd);
#endif
			smb_err = 0x01;
		}
	}

	outportb(smbase+2, (inportb(smbase+2)|0x80)); // enable pec
	outportb(smbase+0x0D, 0x01);
	outportb(smbase+8, pec_value); // pec crc data
	outportb(smbase+3, byte_c1); // byte N
	delay(5);
	outportb(smbase+4, cslaveadd&0xfe); // slave address and write
	delay(5);
	outportb(smbase+5, byted1); // byte data access
	delay(5);
	outportb(smbase+2, 0x48|0x80); // byte data access
	delay(20);

	if(((inportb(smbase))&0x04)==0x4) { // check status fail?
#if LANNER_PSU_DEBUG
		printk("Slave Address = 0x%x, device not exist or smbus access error.\n", cslaveadd);
#endif
		smb_err = 0x02;
	}

	outportb((smbase + 0x00), 0xff);
	outportb((smbase + 0x0c), 0x01);
	outportb((smbase + 0x0d), 0x00);

	return 0;
}

int lanner_psu_value_convert(int word_value)
{
	int d = (word_value&0x03ff);
	if(word_value&0x400)
		d = -d;
	if(word_value&0x8000)
		return ((d * 1000) / (psu_exp[(word_value>>11)&0x001f]));
	return (d * (psu_exp[(word_value>>11)&0x001f]) * 1000);
}

int lanner_psu_value_convert_pec(int word_value, char mode_byte)
{
	int d = word_value;
	if(mode_byte & 0x10)
		return ((d * 1000) / (1 << (~(mode_byte -1) & 0x0f)));
	return ((d * 1000) * (1 << (mode_byte & 0x0f)));
}
