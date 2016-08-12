#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/pci.h>

#include "bypass_control.h"
#include "bypass_slot.h"
#include "bypass_i2cmux.h"

static struct bypass_i2cmux_priv i2cmux_priv;
struct bypass_slot mb_slots_init = {
	.i2cmux = {
		.n_values = 0,
		.n_gpios= 0,
	}
};

struct semaphore i2cmux_sem;

static void enable_gpio(int pin){
	u32 val;
	int bit, reg;

	bit = pin & 31;
	switch (pin >> 5){
	case 0:
		reg = GPIO_USE_SEL;
		break;
	case 1:
		reg = GPIO_USE_SEL2;
		break;
	case 2:
		reg = GPIO_USE_SEL3;
		break;
	default:
		reg = GPIO_USE_SEL;
	}

	val = inl(i2cmux_priv.mem_base + reg);

	set_bit(bit, (void *)&val);

	outl(val, i2cmux_priv.mem_base + reg);
}

static void set_gpio_output(int pin){
	u32 val;
	int bit, reg;

	bit = pin & 31;
	switch (pin >> 5){
	case 0:
		reg = GP_IO_SEL;
		break;
	case 1:
		reg = GP_IO_SEL2;
		break;
	case 2:
		reg = GP_IO_SEL3;
		break;
	default:
		reg = GP_IO_SEL;
	}

	val = inl(i2cmux_priv.mem_base + reg);

	clear_bit(bit, (void *)&val);

	outl(val, i2cmux_priv.mem_base + reg);
}

static void set_gpio_level(int cmd, int pin){
	u32 val;
	int bit, reg;

	bit = pin & 31;
	switch (pin >> 5){
	case 0:
		reg = GP_LVL;
		break;
	case 1:
		reg = GP_LVL2;
		break;
	case 2:
		reg = GP_LVL3;
		break;
	default:
		reg = GP_LVL;
	}

	val = inl(i2cmux_priv.mem_base + reg);

	if (cmd)
		set_bit(bit, (void *)&val);
	else
		clear_bit(bit, (void *)&val);

	outl(val, i2cmux_priv.mem_base + reg);
}

uint8_t get_slot_i2cmux(uint8_t slotno){
	int i;

	for (i = 0; i < PCI_ADDCARD_MAX_SLOTS; i++){
		if (i2cmux_priv.mb_conf->slot[i][0].slotno == (slotno + 1))
			return i2cmux_priv.mb_conf->slot[i][0].i2cmux;
	}

	return -ENODEV;
}

void bypass_switch_i2cmux(uint8_t slotno){
	int i;
	uint8_t mux;
	u8 val;

	if (i2cmux_priv.mb_conf->i2cmux.n_values > 0) {
		mux = get_slot_i2cmux(slotno);

		for (i = 0; i < i2cmux_priv.mb_conf->i2cmux.n_gpios; i++){
			val = (i2cmux_priv.mb_conf->i2cmux.values[mux] >> (i * 4)) & 0xf;
			set_gpio_level(val, i2cmux_priv.mb_conf->i2cmux.gpios[i2cmux_priv.mb_conf->i2cmux.n_gpios - 1 - i]);
		}
	}
}

int gpio_mux_init(void){
	struct pci_dev *pdev = NULL;	
	int i;

	pdev = pci_get_bus_and_slot(0, 0xF8);
	if (pdev == NULL) {
		printk("Can't find GPIO device !!\n");
		return -ENODEV;
	}

	pci_read_config_dword(pdev, GPIO_BAR_OFFSET, &(i2cmux_priv.mem_base));

	i2cmux_priv.mem_base &= 0x0000ff80;

	/* release reference to device */
	pci_dev_put(pdev);

	/* obtain exclusive access to GPIO memory space */
	request_region(i2cmux_priv.mem_base, GPIO_MEM_SIZE, DRV_NAME);

	/* Initialize GPIO pins */
	for (i = 0; i < i2cmux_priv.mb_conf->i2cmux.n_gpios; i++){
		enable_gpio(i2cmux_priv.mb_conf->i2cmux.gpios[i]);
		set_gpio_output(i2cmux_priv.mb_conf->i2cmux.gpios[i]);
	}

	return 0;
}

int bypas_mux_init(char *board){
	int mb_idx, mb_cnt;
	int ret = -ENODEV;

	for (mb_idx = 0; mb_idx < sizeof(mb_slots) / sizeof(struct bypass_slot); mb_idx++){
		for(mb_cnt = 0; mb_cnt < MB_NAME_LEN; mb_cnt++){
			if(strcmp(board, mb_slots[mb_idx].boardname[mb_cnt]) == 0){
				ret = 0;
				break;
			}
		}
		if (ret == 0)
			break;
	}

	init_MUTEX (&i2cmux_sem);

	if (ret) {
		i2cmux_priv.mb_conf = &mb_slots_init;
		ret = 0;
	} else {
		i2cmux_priv.mb_conf = &mb_slots[mb_idx];
		if (i2cmux_priv.mb_conf->i2cmux.n_gpios > 0){
			ret = gpio_mux_init();
		}
	}

	return ret;
}

void bypass_mux_remove(void){
	if (i2cmux_priv.mb_conf->i2cmux.n_gpios > 0){
		release_region(i2cmux_priv.mem_base, GPIO_MEM_SIZE);
	}
}
