/*******************************************************************************

  CASwell(R) Gen3 Bypass Linux driver
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

#ifndef _TILERA_DEFINES_H_
#define _TILERA_DEFINES_H_

#include "gxio/gpio.h"
#include <gxio/mpipe.h>

#ifdef CONFIG_CW_TILE_DEBUG
#define CW_TILE_DEBUG(fmt,args...) printk(fmt, ##args)
#else
#define CW_TILE_DEBUG(fmt,args...)
#endif

#define TILEGX_MAX_XAUI        2
#define TILEGX_MAX_CHAN        16
#define TILEGX_XAUI_PORTS  	   40
#define MAX_GX_GPIO_SET        2

/***********************
 *  Registers          *
 ***********************/

// SPI Interface
#define GX_GPIO_SS_0        0x0000000000000100           // GPIO_8
#define GX_GPIO_SK_0        0x0000000000000200           // GPIO_9
#define GX_GPIO_SI_0        0x0000000000000400           // GPIO_10
#define GX_GPIO_SO_0        0x0000000000000800           // GPIO_11
#define GX_GPIO_SS_1        0x0000000000001000           // GPIO_12
#define GX_GPIO_SK_1        0x0000000000002000           // GPIO_13
#define GX_GPIO_SI_1        0x0000000000004000           // GPIO_14
#define GX_GPIO_SO_1        0x0000000000008000           // GPIO_15

// Mask
#define GX_SPI_MASK_0       0x0000000000000F00
#define GX_SPI_OUT_0        0x0000000000000700
#define GX_SPI_INIT_0       0x0000000000000100
#define GX_SPI_MASK_1       0x000000000000F000
#define GX_SPI_OUT_1        0x0000000000007000
#define GX_SPI_INIT_1       0x0000000000001000

const static u32 GX_GPIO_SS_SET[] = { GX_GPIO_SS_0, GX_GPIO_SS_1 };
const static u32 GX_GPIO_SK_SET[] = { GX_GPIO_SK_0, GX_GPIO_SK_1 };
const static u32 GX_GPIO_SI_SET[] = { GX_GPIO_SI_0, GX_GPIO_SI_1 };
const static u32 GX_GPIO_SO_SET[] = { GX_GPIO_SO_0, GX_GPIO_SO_1 };

const static u32 GX_SPI_MASK_SET[] = { GX_SPI_MASK_0, GX_SPI_MASK_1 };
const static u32 GX_SPI_OUT_SET[]  = { GX_SPI_OUT_0,  GX_SPI_OUT_1  };
const static u32 GX_SPI_INIT_SET[] = { GX_SPI_INIT_0, GX_SPI_INIT_1 };


int tilegx_gpio_init(char bp_idx_set);
void tilegx_gpio_exit(void);
u32 tilegx_gpio_read(u8 __iomem* hw_addr, u32 reg);
void tilegx_gpio_write(u8 __iomem* hw_addr, u32 reg, u32 value);
void tilegx_gpio_flush(u8 __iomem* hw_addr);

#endif
