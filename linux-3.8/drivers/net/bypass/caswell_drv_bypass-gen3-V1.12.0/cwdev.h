/*******************************************************************************

  CASwell(R) Gen3 Bypass Linux driver
  Copyright(c) 2011 Zeno Lai <zeno.lai@cas-well.com>

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

#ifndef _CWDEV_H_
#define _CWDEV_H_

#define PCI_VENDOR_ID_CASWELL                0x15BB
#define PCI_VENDOR_ID_TILERA                 0x1A41

/***********************
 *  Chip ID            *
 ***********************/

#define E1000_DEV_ID_82583V                  0x150C
#define E1000_DEV_ID_82580_COPPER            0x150E
#define E1000_DEV_ID_82580_FIBER             0x150F
#define E1000_DEV_ID_82580_SERDES            0x1510
#define E1000_DEV_ID_82580_SGMII             0x1511
#define E1000_DEV_ID_82580_COPPER_DUAL       0x1516
#define E1000_DEV_ID_I350_COPPER             0x1521
#define E1000_DEV_ID_I350_FIBER              0x1522
#define E1000_DEV_ID_DH89XXCC_SGMII          0x0438
#define IXGBE_DEV_ID_82599_SFP               0x10FB
#define E1000_DEV_ID_I210_COPPER             0x1533
#define E1000_DEV_ID_X540_COPPER             0x1528
#define E1000_DEV_ID_I354_SGMII              0x1F41
#define GX36_DEV_ID_CANNON                   0x0200
#define GX36_PCI_IFACE                       0x0004
#define GX36_CSW_ID                          0x0100
#define GX36_CSW_BPTYPE                      0x3000
#define RTL8168_DEV_ID                       0x8168

#endif

