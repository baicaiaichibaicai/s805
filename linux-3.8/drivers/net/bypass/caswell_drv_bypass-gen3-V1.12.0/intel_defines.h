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

#ifndef _INTEL_DEFINES_H_
#define _INTEL_DEFINES_H_


/***********************
 *  Registers          *
 ***********************/

// Generic register
#define INIC_CTRL             0x00000
#define INIC_STATUS           0x00008
#define INIC_CTRL_EXT         0x00018

// E1000 register
#define E1000_EECD            0x00010
#define E1000_FLA             0x0001C
#define E1000_EXTCNF_CTRL     0x00F00
#define E1000_SWSM            0x05B50
#define E1000_SW_FW_SYNC      0x05B5C

// ixgbe register
#define IXGBE_ESDP            0x00020
#define IXGBE_EECD            0x10010
#define IXGBE_EERD            0x10014
#define IXGBE_FLA             0x1001C
#define IXGBE_SWSM            0x10140
#define IXGBE_GSSR            0x10160


/***********************
 *  Control bitmask    *
 ***********************/

#define E1000_CTRL_EXT_EE_RST                  0x00002000
#define E1000_SWSM_SMBI                        0x00000001
#define E1000_SWSM_SWESMBI                     0x00000002
#define E1000_EXTCNF_CTRL_MDIO_SW_OWNERSHIP    0x00000020
#define E1000_SWFW_FLA_SM                      0x0010
#define E1000_SWFW_EEC_SM                      0x0001

#define IXGBE_SWSM_SMBI                        0x00000001
#define IXGBE_SWSM_SWESMBI                     0x00000002
#define IXGBE_GSSR_EEP_SM                      0x0001
#define IXGBE_GSSR_FLASH_SM                    0x0010
#define IXGBE_SWFW_REGSMP                      0x80000000 /* Register Semaphore bit 31 */
#define IXGBE_GSSR_SW_MNG_SM                   0x0400

/* Error Codes */
#define IXGBE_ERR_EEPROM         -1
#define IXGBE_ERR_SWFW_SYNC      -16

// EEPROM Reset bit

// SPI interface
#define INIC_EECD_SK             0x00000001
#define INIC_EECD_CS             0x00000002
#define INIC_EECD_DI             0x00000004
#define INIC_EECD_DO             0x00000008
#define INIC_EECD_REQ            0x00000040
#define INIC_EECD_GNT            0x00000080
#define INIC_EECD_PRES           0x00000100
#define INIC_EECD_BLOCKED        0x00008000  // Bit banging access blocked flag
#define INIC_FLA_SCK             0x00000001
#define INIC_FLA_CE              0x00000002
#define INIC_FLA_SI              0x00000004
#define INIC_FLA_SO              0x00000008
#define INIC_FLA_REQ             0x00000010
#define INIC_FLA_GNT             0x00000020


/***********************
 * Parameter           *
 ***********************/
#define HW_SM_ATTEMPTS                         200
#define MDIO_OWNERSHIP_TIMEOUT                 10
#define NIC_SPI_GRANT_ATTEMPTS                 1000
#define NIC_SUCCESS                            0
#define NIC_ERR                                EBUSY

static inline void intel_flush(u8 __iomem* hw_addr)
{
	readl(hw_addr + INIC_STATUS);
}

#endif
