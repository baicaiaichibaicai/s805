
#ifndef __BYPASS_KVERSION_HEAD__
#define __BYPASS_KVERSION_HEAD__

#ifdef linux
#undef  linux
#endif	/*ifdef linux*/


#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )
#define	SEMAPHORE_HEAD			    <linux/semaphore.h>
#else
#define	SEMAPHORE_HEAD			    <asm/semaphore.h>
#endif /* ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) )*/

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,4,29) )
#define	DELAY_MSLEEP_HEAD		    "bypass_msleep.h"
#else
#define	DELAY_MSLEEP_HEAD		    <linux/delay.h>
#endif /*( LINUX_VERSION_CODE < KERNEL_VERSION(2,4,29) )*/

#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) )
#define pci_scan_subsys                     pci_get_subsys
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,19) )
#define	pci_scan_bus_and_slot		    pci_get_bus_and_slot
#else
#define	pci_scan_bus_and_slot		    pci_find_slot
#endif /*( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30) )*/
#define PCI_REG_HEAD                        <linux/pci_regs.h>
#define USER_INTERFACE_HEAD		    <linux/hwmon-sysfs.h>
#define MODULE_PARM_CHAR(param)		    module_param(param, charp, S_IRUGO)
#else
#define pci_scan_subsys                     pci_find_subsys
#define	pci_scan_bus_and_slot		    pci_find_slot
#define pci_dev_put(dev)				    /*There is no such function in Kernel 2.4*/
#define PCI_REG_HEAD			    <linux/pci.h>
#define USER_INTERFACE_HEAD		    <linux/i2c-proc.h>
#define MODULE_PARM_CHAR(param)		    MODULE_PARM(board, "s")
#endif /*( LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0) )*/

/* init_MUTEX macro was removed in 2.6.37 */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 36) && !defined(init_MUTEX)
#define init_MUTEX(sem) sema_init(sem, 1)
#endif 

#endif /*__BYPASS_KVERSION_HEAD__*/


