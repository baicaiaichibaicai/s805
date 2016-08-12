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

#ifndef _AMD_DEFINES_H_
#define _AMD_DEFINES_H_

#define PM_INDEX                 0xCD6
#define PM_DATA                  0xCD7
#define A50M_GPIO_BAR            0x100
#define A50M_IOMUX_BAR           0xD00
#define A50M_GPIO_MEMSIZE        0x100

/***********************
 *  Registers          *
 ***********************/

// Generic register
#define ACPI_MMIO_EN             0x24

#define A50M_SS_REG              201
#define A50M_SK_REG              202
#define A50M_SI_REG              203
#define A50M_SO_REG              204

/***********************
 *  Control bitmask    *
 ***********************/

#define ACPI_MMIO_DECODE_EN      0x1
#define ACPI_MMIO_SEL            0x2
// SPI interface
#define A50M_GPIO_OUT_ENB        0x20
#define A50M_GPIO_OUT            0x40
#define A50M_GPIO_IN             0x80


static inline void amd_flush(u8 __iomem* hw_addr)
{
	readl(hw_addr);
}

#endif
