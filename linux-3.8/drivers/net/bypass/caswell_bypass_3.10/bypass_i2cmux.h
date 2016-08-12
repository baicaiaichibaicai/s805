#include "bypass_pci.h"
//#include "bypass_slot.h"

/* Intel SB GPIO definition */

#define GPIO_BAR_OFFSET 0x48
/* GPIO register memory size in bytes */
#define GPIO_MEM_SIZE 64

#define  GPIO_USE_SEL   0x00
#define  GP_IO_SEL      0x04
#define  GP_LVL         0x0c
#define  GPO_BLINK      0x18
#define  GPI_INV        0x2c
#define  GPIO_USE_SEL2  0x30
#define  GP_IO_SEL2 	0x34
#define  GP_LVL2    	0x38
#define  GPIO_USE_SEL3  0x40
#define  GP_IO_SEL3 	0x44
#define  GP_LVL3    	0x48

/*Intel SB GPIO definition -- end*/

struct bypass_i2cmux_priv {
	const struct bypass_slot *mb_conf;
	u32 mem_base;
};

void bypass_switch_i2cmux(uint8_t slotno);
int bypas_mux_init(char *board);
void bypass_mux_remove(void);
