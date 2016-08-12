/*******************************************************************************

  CASwell(R) Bypass Linux driver
  Copyright(c) 2012 Zeno Lai <zeno.lai@cas-well.com>

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

#ifndef _BYPASS_H_
#define _BYPASS_H_

/* BYPASS COMMANDS */
#define BP_GET_STS               0x01
#define BP_SET_NORM              0x02
#define BP_SET_NON               0x03
#define BP_WDT_RS                0x04
#define BP_WDT_LOAD              0x05
#define BP_WDT_DISABLE           0x06
#define BP_PERIOD                0x07
#define BP_WDT_STS               0x08
#define BP_BPID                  0x09
#define BP_FWVER                 0x0A
#define BP_ONBOARD               0x0B
#define BP_LAN_TYPE              0x0C

#define BPDEV_STR_LEN            20
#define BYPASS_DEVICE_NAME_SIZE  10
#define BYPASS_MODULE_NAME_SIZE  20

extern struct class bypass_class;
struct bypass_device;

struct bypass_info {
	/* bypass information */
	u16  gen;
	u16  bp_type;
	u8   seg_idx;
	u8   upver_minor;
	u8   upver_major;
	u8   onboard;
	u16  twdmax;
	char fwver[BPDEV_STR_LEN];
	char lan_type[BPDEV_STR_LEN];
};

struct bypass_operations {
	s32  (*read)(struct bypass_device *, u16, char *);
	s32  (*write)(struct bypass_device *, u16, u16);
};

struct bypass_device {
	char                     name[BYPASS_DEVICE_NAME_SIZE];
	char                     module[BYPASS_MODULE_NAME_SIZE];

	struct device            dev;
	struct list_head         dev_list;

	struct hlist_node        name_hlist;
	struct hlist_node        index_hlist;

	struct bypass_info       info;
	struct bypass_operations ops;

	struct device            *port0;
	struct device            *port1;

	TYPE_PREFIX struct attribute_group   *sysfs_groups[3];

};

struct bypass {
	atomic_t                 count;
	struct list_head         list;
	struct list_head         dev_base_head;
	struct hlist_head        *dev_name_head;
	struct hlist_head        *dev_index_head;
};


#define SET_BPDEV_DEV(bp, d)      ((bp)->dev.parent = (d))
#define to_bypass_dev(d) container_of(d, struct bypass_device, dev)

extern s32  bypass_register_device(struct bypass_device *bp);
extern void bypass_unregister_device(struct bypass_device *bp);

extern int bypass_class_init(void);
extern void bypass_class_exit(void);

#endif
