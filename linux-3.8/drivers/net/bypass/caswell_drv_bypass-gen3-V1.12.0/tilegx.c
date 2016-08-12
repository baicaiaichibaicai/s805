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
#include "tilera_defines.h"

gxio_gpio_context_t context;

int tilegx_gpio_init(char bp_idx_set)
{
	int ret, i;
	uint32_t spi_mask = 0x0, gpio_so = 0x0, spi_out = 0x0, spi_init = 0x0;

	for (i = 0; i < MAX_GX_GPIO_SET; i++) {
		if (bp_idx_set & (1 << i)) {
			spi_mask |= GX_SPI_MASK_SET[i];
			gpio_so |=  GX_GPIO_SO_SET[i];
			spi_out |= GX_SPI_OUT_SET[i];
			spi_init |= GX_SPI_INIT_SET[i];
		}
	}

	ret = gxio_gpio_init(&context, 0);
	if (ret)
		goto out;
	ret = gxio_gpio_attach(&context, spi_mask);
	if (ret)
		goto out;

	// initialize pins status
	gxio_gpio_set_dir(&context, 0, gpio_so, spi_out, 0);
#ifdef CONFIG_CW_TILE_DEBUG
	uint64_t dispin, inpin, outpin, odpin;
	gxio_gpio_get_dir(&context, &dispin, &inpin, &outpin, &odpin);
	CW_TILE_DEBUG("tilegx_gpio_init:input:%#016llx; out:%#016llx;\n",
	              inpin, outpin);
#endif
	gxio_gpio_set(&context, spi_init, spi_out);
	ret = 0;

out:
	return ret;
}

void tilegx_gpio_exit(void)
{
	gxio_gpio_destroy(&context);
}

u32 tilegx_gpio_read(u8 __iomem* hw_addr, u32 reg)
{
	return (u32)gxio_gpio_get(&context);
}

void tilegx_gpio_write(u8 __iomem* hw_addr, u32 reg, u32 value)
{
	gxio_gpio_set(&context, value, reg);
}

void tilegx_gpio_flush(u8 __iomem* hw_addr)
{
	gxio_gpio_get(&context);
}

