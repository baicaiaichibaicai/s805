#include "ms96d9_bypass.h"
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/delay.h>

extern int __cpuinit get_model_name(struct cpuinfo_x86 *c);
static volatile unsigned long gpio_ba = 0;
static int _dev_init=0;
enum { 
    UNKNOWN_CORE = 0,
    ATOM_CORE, 
};

int is_atom_core(void)
{
    int res;
    struct cpuinfo_x86 c;
    get_model_name(&c);
    if (strstr(c.x86_model_id, ATOM_PROCESSOR))
	res = ATOM_CORE;
    else
	res = UNKNOWN_CORE;
    return res;
}

static void bypass_signal(void)
{
    int gp_lvl;
    gp_lvl = inl(gpio_ba + GP_LVL);

    gp_lvl |= (0x01<<SEND_BIT);
    outl(gp_lvl, gpio_ba + GP_LVL);
    __udelay(1000*150);	
    gp_lvl &= ~(0x01<<SEND_BIT);
    outl(gp_lvl, gpio_ba + GP_LVL);
    __udelay(1000*50);	
}

void ms96d9_bypass_init(int port)
{
    unsigned long gp_lvl;
    char gp_lvl2;
    char gpio_conf = 0x10;

    /* LAN pair 1 behavior on:Bypass/off:Bypass/Watchdog:Bypass */
    gp_lvl = inl(gpio_ba + GP_LVL);
    gp_lvl2 = inb(gpio_ba + GP_LVL2);
    clear_bit(gpio_conf, &gp_lvl);

    gp_lvl2 |= (0x01 << PAIR3_BIT);
    gp_lvl2 |= (0x01 << PAIR2_BIT);
    gp_lvl2 &= ~(0x01 << CFG_1_BIT);
    gp_lvl2 &= ~(0x01 << CFG_2_BIT);
    gp_lvl2 &= ~(0x01 << CFG_3_BIT);

    gp_lvl &= ~(0x01 << PAIR1_BIT);

    outl(gp_lvl, gpio_ba + GP_LVL);
    outb(gp_lvl2, gpio_ba + GP_LVL2);

    bypass_signal();
}

void ms96d9_bypass_enable(int port)
{
    unsigned long gp_lvl;
    char gp_lvl2;
    char gpio_conf = 0x10;

    /* LAN pair 1 behavior on:Pass/off:Bypass/Watchdog:Bypass */
    gp_lvl = inl(gpio_ba + GP_LVL);
    clear_bit(gpio_conf, &gp_lvl);

    gp_lvl2 = inb(gpio_ba + GP_LVL2);
    gp_lvl2 |= (0x01 << PAIR3_BIT);
    gp_lvl2 |= (0x01 << PAIR2_BIT);
    gp_lvl2 &= ~(0x01 << CFG_1_BIT);
    gp_lvl2 &= ~(0x01 << CFG_2_BIT);
    gp_lvl2 |= (0x01 << CFG_3_BIT);

    gp_lvl &= ~(0x01 << PAIR1_BIT);
    outl(gp_lvl, gpio_ba + GP_LVL);
    outb(gp_lvl2, gpio_ba + GP_LVL2);

    bypass_signal();
}

void ms96d9_bypass_disable(int port)
{
    unsigned long gp_lvl;
    char gp_lvl2 = 0x01;

    /* LAN pair 1-2 behavior on:Bypass/off:Bypass/Watchdog:Bypass */
    gp_lvl = inl(gpio_ba + GP_LVL);
    clear_bit(0x10, &gp_lvl);
    outl(gp_lvl, gpio_ba + GP_LVL);
    outb(gp_lvl2, gpio_ba + GP_LVL2);

    bypass_signal();
}

static int __devinit
ms96d9_init_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{ 
    unsigned long gpio_phy_bar = 0;
    int err;

    if (!is_atom_core()) {
	printk("Not supported bypass of ms96d9 in this core\n");
	return 1;
    }

    err = pci_enable_device(pdev);
    if (err)
	printk("pci_enable_failed in bypass\n");

    pci_read_config_dword (pdev, LPC_CONFIG_BASE + GPIO_BA_OFFSET, (unsigned int *)&gpio_phy_bar); 

    if (!gpio_phy_bar) { 
	printk("Can't read pci config register : 0x%x\n", LPC_CONFIG_BASE + GPIO_BA_OFFSET);
	return 1;
    }

    gpio_ba = (unsigned long)ioremap(gpio_phy_bar, SIZE_LPC_REG);

    if (!gpio_ba)
	printk("failed to ioremap gpio interface\n");

    return 0;
}

static void __devexit
ms96d9_remove(struct pci_dev *pdev)
{
    pci_disable_device(pdev);
    iounmap(&gpio_ba);
    return ;
}

static struct pci_device_id msi_ich4_id_table[] = {
    {	PCI_VENDOR_ID_INTEL,
	PCI_DEVICE_ID_INTEL_ICH8_4,
	PCI_ANY_ID,
	PCI_ANY_ID,
	0, 0, 0	},
    {0},
};
MODULE_DEVICE_TABLE(pci, msi_ich4_id_table);

static struct pci_driver ich4_lpc_if_driver = { 
    .name = 	DRV_NAME,
    .id_table =	msi_ich4_id_table,
    .probe =	ms96d9_init_probe,
    .remove =	ms96d9_remove,
};

static int __init ms96d9_lpc_dev_init(void)
{
    if (_dev_init)
	return 0;
    _dev_init = 1;
    pci_register_driver(&ich4_lpc_if_driver);
    return 0;
}

static void __exit ms96d9_lpc_cleanup_module(void)
{
    if (!_dev_init)
	return ;
    _dev_init = 0;
    pci_unregister_driver(&ich4_lpc_if_driver);
}
module_init(ms96d9_lpc_dev_init);
module_exit(ms96d9_lpc_cleanup_module);
