/*******************************************************************************

  CASwell(R) Bypass Linux driver
  Copyright(c) 2014 Scott Jhuang <scott.jhuang@cas-well.com>

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

#ifndef _S_TYPE_IOCTL_H_
#define _S_TYPE_IOCTL_H_

#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,17))
	#include <linux/config.h>
#endif
#if defined(CONFIG_SMP) && ! defined(__SMP__)
	#define __SMP__
#endif

#include <linux/kernel.h> /* We're doing kernel work */
#include <linux/module.h> /* Specifically, a module */
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>
#include <asm/uaccess.h> /* for get_user and put_user */
#include <linux/ethtool.h>
#include <linux/kthread.h>

#define BPE_BYPASS      "1"
#define BPE_OPEN        "0"
#define NB_NON_NORMAL   "1"
#define NB_NORMAL       "0"
#define BP_FUNC_EN      "1"
#define BP_FUNC_DIS     "0"
#define NORMAL          "n"
#define BYPASS          "b"
#define OPEN            "o"
#define ON              "1"
#define OFF             "0"

#define BP_CAP                  0x01       //BIT_0
#define BP_STATUS_CAP           0x02       //BIT_1
#define BP_STATUS_CHANGE_CAP    0x04       //BIT_2
#define SW_CTL_CAP              0x08       //BIT_3
#define BP_DIS_CAP              0x10       //BIT_4
#define BP_DIS_STATUS_CAP       0x20       //BIT_5
#define STD_NIC_CAP             0x40       //BIT_6
#define BP_PWOFF_ON_CAP         0x80       //BIT_7
#define BP_PWOFF_OFF_CAP        0x0100     //BIT_8
#define BP_PWOFF_CTL_CAP        0x0200     //BIT_9
#define BP_PWUP_ON_CAP          0x0400     //BIT_10
#define BP_PWUP_OFF_CAP         0x0800     //BIT_11
#define BP_PWUP_CTL_CAP         0x1000     //BIT_12
#define WD_CTL_CAP              0x2000     //BIT_13
#define WD_STATUS_CAP           0x4000     //BIT_14
#define WD_TIMEOUT_CAP          0x8000     //BIT_15
#define TX_CTL_CAP              0x10000    //BIT_16
#define TX_STATUS_CAP           0x20000    //BIT_17
#define TAP_CAP                 0x40000    //BIT_18
#define TAP_STATUS_CAP          0x80000    //BIT_19
#define TAP_STATUS_CHANGE_CAP   0x100000   //BIT_20
#define TAP_DIS_CAP             0x200000   //BIT_21
#define TAP_DIS_STATUS_CAP      0x400000   //BIT_22
#define TAP_PWUP_ON_CAP         0x800000   //BIT_23
#define TAP_PWUP_OFF_CAP        0x1000000  //BIT_24
#define TAP_PWUP_CTL_CAP        0x2000000  //BIT_25
#define NIC_CAP_NEG             0x4000000  //BIT_26
#define TPL_CAP                 0x8000000  //BIT_27
#define DISC_CAP                0x10000000 //BIT_28
#define DISC_DIS_CAP            0x20000000 //BIT_29
#define DISC_PWUP_CTL_CAP       0x40000000 //BIT_30

#define TPL2_CAP_EX             0x01
#define DISC_PORT_CAP_EX        0x02

#if ( LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21) )
#ifndef to_net_dev
#define to_net_dev(cd) container_of(cd, struct net_device, class_dev)
#endif
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24))
#define FOR_EACH_NETDEV(DEV) for_each_netdev(&init_net, (DEV))
#elif defined(for_each_netdev)
#define FOR_EACH_NETDEV(DEV) for_each_netdev(DEV)
#else
#define FOR_EACH_NETDEV(DEV) for ((DEV)=dev_base; (DEV); (DEV)=(DEV)->next)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,21))
#define GET_NETDEV_DEV(net) ((net)->dev.parent)
#else
#define GET_NETDEV_DEV(net) ((net)->class_dev.dev)
#endif

#define is_pci_device(dev) ((dev)->bus == &pci_bus_type)

#define WD_MIN_TIME_MASK(val)       (val & 0xf)
#define WD_STEP_COUNT_MASK(val)     ((val & 0xf) << 5)
#define WDT_STEP_TIME               0x10    //BIT_4

#define WD_MIN_TIME_GET(desc)       (desc & 0xf)
#define WD_STEP_COUNT_GET(desc)     (desc>>5) & 0xf


typedef enum {
	IF_SCAN ,
	GET_DEV_NUM ,
	IS_BYPASS ,
	GET_BYPASS_SLAVE ,
	GET_BYPASS_CAPS ,
	GET_WD_SET_CAPS ,
	SET_BYPASS ,
	GET_BYPASS ,
	GET_BYPASS_CHANGE ,
	SET_BYPASS_WD ,
	GET_BYPASS_WD ,
	GET_WD_EXPIRE_TIME ,
	RESET_BYPASS_WD_TIMER ,
	SET_DIS_BYPASS ,
	GET_DIS_BYPASS ,
	SET_BYPASS_PWOFF ,
	GET_BYPASS_PWOFF ,
	SET_BYPASS_PWUP ,
	GET_BYPASS_PWUP ,
	SET_STD_NIC ,
	GET_STD_NIC ,
	SET_TX ,
	GET_TX ,
	SET_TAP ,
	GET_TAP ,
	GET_TAP_CHANGE ,
	SET_DIS_TAP ,
	GET_DIS_TAP ,
	SET_TAP_PWUP , 
	GET_TAP_PWUP ,
	SET_WD_EXP_MODE ,
	GET_WD_EXP_MODE ,
	SET_WD_AUTORESET ,
	GET_WD_AUTORESET ,
	SET_TPL ,
	GET_TPL ,
	SET_DISC ,
	GET_DISC ,
	GET_DISC_CHANGE ,
	SET_DIS_DISC ,
	GET_DIS_DISC ,
	SET_DISC_PWUP ,
	GET_DISC_PWUP ,
	GET_BYPASS_INFO=100 ,
	GET_BP_WAIT_AT_PWUP ,
	SET_BP_WAIT_AT_PWUP ,
	GET_BP_HW_RESET ,
	SET_BP_HW_RESET ,
	SET_DISC_PORT ,
	GET_DISC_PORT ,
	SET_DISC_PORT_PWUP ,
	GET_DISC_PORT_PWUP ,
#ifdef BP_SELF_TEST
	SET_BP_SELF_TEST=200 ,
	GET_BP_SELF_TEST ,
#endif

} CMND_TYPE_SD ;

/*
* The major device number. We can't rely on dynamic
* registration any more, because ioctls need to know
* it.
*/


#define MAGIC_NUM 'J'

struct bp_dev_info {
	int                 ifindex;
	int                 dev_number;
	uint8_t             bus;
	uint8_t             slot;
	uint8_t             func;
	uint8_t             current_mode;
	struct pci_dev      *pdev;
	struct csw_device   *csw_dev;
	struct task_struct  *wdt_thread;
	struct timer_list   bp_timer;
	int                 period_value;
	int                 wdt_reset_time;
	int                 dis_bypass;
	int                 dis_open;
	int                 bypass_change;
	int                 open_change;
};

/* for passing single values */
struct bpctl_cmd {
	int status;
	int data[8];
	int in_param[8];
	int out_param[8];
};

void wdt_auto_retrigger(unsigned long param);
int bypass_wdt_auto_init (struct bp_dev_info *bp_dev);
void bypass_wdt_auto_exit(void);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30))
int device_ioctl(struct inode *inode, struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
#else
long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);
#endif

#define IOCTL_TX_MSG(cmd) _IOWR(MAGIC_NUM, cmd, struct bpctl_cmd)

#define DEVICE_NODE "/dev/bpctl0"
#define DEVICE_NAME "bpctl0"


#endif


