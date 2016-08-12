/*******************************************************************************

  CASwell(R) Ethernet Bypass Linux driver
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

#ifndef _CSW_SPI_H_
#define _CSW_SPI_H_

#include <linux/types.h>
#ifdef __KERNEL__

#ifdef CONFIG_CW_SPI_DEBUG
#define CW_SPI_DEBUG(fmt,args...) printk(fmt, ##args)
#else
#define CW_SPI_DEBUG(fmt,args...)
#endif

#ifdef CONFIG_CW_SPICLK_DEBUG
#define CW_SPICLK_DEBUG(fmt,args...) printk(fmt, ##args)
#else
#define CW_SPICLK_DEBUG(fmt,args...)
#endif

struct csw_spi_master;

struct csw_spi_cpld {
	u16 clock;
	u8  segcode;
	u8  busy;
	u32 attempts;
	u8  protocol;
};

struct csw_spi_config {
	/* SPI control */
	u16 clk_usec;
	u16 mux_delay;
	u16 byte_delay;
	u8  sck;
};

struct csw_spi_pin {
	u32 mask;
	u32 reg;
};

struct csw_spi_ctrl {
	struct csw_spi_pin spi_sck;
	struct csw_spi_pin spi_ss;
	struct csw_spi_pin spi_mosi;
	struct csw_spi_pin spi_miso;
	struct csw_spi_pin spi_rqst;
	struct csw_spi_pin spi_gnt;
	u16    spi_swfw_sm;
};

struct csw_spi_operations {
	u32  (*read_reg)(u8 __iomem*, u32);
	void (*write_reg)(u8 __iomem*, u32, u32);
	void (*wflush)(u8 __iomem*);
	s32  (*acquire)(struct csw_spi_master *);
	void (*release)(struct csw_spi_master *);
};

struct csw_spi_master {
	u8 __iomem                *hw_addr;
	struct csw_spi_cpld       cpld;
	struct csw_spi_config     cfg;
	struct csw_spi_ctrl       ifspi;
	struct csw_spi_operations ops;
	struct mutex              *sem;
};


#endif  /* __KERNEL__ */
#endif  /* _CSW_SPI_H_ */
