/*******************************************************************************

  CASwell(R) Network Bypass Linux driver
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

#ifndef _BYPASS_SLOT_H_
#define _BYPASS_SLOT_H_

#include "bypass_pci.h"

struct bypass_i2cmux{
	unsigned values[4];
	int n_values;
	unsigned gpios[2];      /* Relative to gpio_chip->base */
	int n_gpios;
};

struct bridge_dev{
	uint8_t bus;
	uint8_t devfn;
	uint8_t slotno;
	uint8_t extend;
	uint8_t i2cmux;
};

struct bypass_slot{
	unsigned char boardname[MB_NAME_LEN][MB_NAME_LEN];
	struct bridge_dev slot[PCI_ADDCARD_MAX_SLOTS][SLOT_BRIDGE_NUM];
	struct bypass_i2cmux i2cmux;
};


/****************************************
 *  Motherboard PCI slot configuration. *
 ****************************************/
const static struct bypass_slot mb_slots[]={
	{
		.boardname = {"NAR5650"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 1, .extend = 0, .i2cmux = 0},
			},{
				{ .bus = 0x00, .devfn = 0x30, .slotno = 2, .extend = 0, .i2cmux = 0},
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"NAR7090"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x20, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x28, .slotno = 1, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x30, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x38, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x10, .slotno = 3, .extend = 0, .i2cmux = 0},
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"NAR7100"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x18, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x20, .slotno = 1, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x28, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x30, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x38, .slotno = 3, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x40, .slotno = 3, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR2000"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 1, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR3000"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 1, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR3020"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x09, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x0A, .slotno = 1, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR3030"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 1, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR4000"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x18, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x20, .slotno = 1, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x28, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x30, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0xE0, .slotno = 3, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0xE4, .slotno = 3, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR4010"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x09, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x10, .slotno = 2, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR4020"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0xE0, .slotno = 1, .extend = 0, .i2cmux = 0},
			},{
				{ .bus = 0x00, .devfn = 0x09, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x0A, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 3, .extend = 0, .i2cmux = 0},
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR5010"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x18, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x20, .slotno = 1, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x28, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x30, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x38, .slotno = 3, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x40, .slotno = 3, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR5020"},
		.slot = {
			{
				{ .bus = 0x80, .devfn = 0x18, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x80, .devfn = 0x19, .slotno = 1, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x09, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x1A, .slotno = 3, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x1B, .slotno = 3, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR5030"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x09, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x1A, .slotno = 3, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x1B, .slotno = 3, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x10, .slotno = 4, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x11, .slotno = 4, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x80, .devfn = 0x18, .slotno = 5, .extend = 0, .i2cmux = 1},
				{ .bus = 0x80, .devfn = 0x19, .slotno = 5, .extend = 0, .i2cmux = 1}
			},{
				{ .bus = 0x80, .devfn = 0x08, .slotno = 7, .extend = 0, .i2cmux = 1},
				{ .bus = 0x80, .devfn = 0x09, .slotno = 7, .extend = 0, .i2cmux = 1}
			}
		},
		.i2cmux = {
			.values = { 0x01, 0x11 },
			.n_values = 2,
			.gpios = { 65, 64 },
			.n_gpios= 2,
		},
	},{
		.boardname = {"CPT3210","CAR4211C"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 1, .extend = 0, .i2cmux = 0},
			},{
				{ .bus = 0x00, .devfn = 0x09, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x0A, .slotno = 2, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CPT4010","CAR4018","CAR4212C"},
		.slot = {
			{
				{ .bus = 0x02, .devfn = 0x28, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x02, .devfn = 0x30, .slotno = 1, .extend = 0, .i2cmux = 0},
			},{
				{ .bus = 0x02, .devfn = 0x28, .slotno = 2, .extend = 1, .i2cmux = 0},
				{ .bus = 0x02, .devfn = 0x30, .slotno = 2, .extend = 1, .i2cmux = 0},
			},{
				{ .bus = 0x02, .devfn = 0x40, .slotno = 3, .extend = 1, .i2cmux = 0},
				{ .bus = 0x02, .devfn = 0x48, .slotno = 3, .extend = 1, .i2cmux = 0},
			},{
				{ .bus = 0x02, .devfn = 0x40, .slotno = 4, .extend = 0, .i2cmux = 0},
				{ .bus = 0x02, .devfn = 0x48, .slotno = 4, .extend = 0, .i2cmux = 0},
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"COS9200"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x09, .slotno = 1, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x10, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x11, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x12, .slotno = 3, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x13, .slotno = 3, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x18, .slotno = 4, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x19, .slotno = 4, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x80, .devfn = 0x08, .slotno = 5, .extend = 0, .i2cmux = 1},
				{ .bus = 0x80, .devfn = 0x09, .slotno = 5, .extend = 0, .i2cmux = 1}
			},{
				{ .bus = 0x80, .devfn = 0x10, .slotno = 6, .extend = 0, .i2cmux = 1},
				{ .bus = 0x80, .devfn = 0x11, .slotno = 6, .extend = 0, .i2cmux = 1}
			},{
				{ .bus = 0x80, .devfn = 0x12, .slotno = 7, .extend = 0, .i2cmux = 1},
				{ .bus = 0x80, .devfn = 0x13, .slotno = 7, .extend = 0, .i2cmux = 1}
			},{
				{ .bus = 0x80, .devfn = 0x18, .slotno = 8, .extend = 0, .i2cmux = 1},
				{ .bus = 0x80, .devfn = 0x19, .slotno = 8, .extend = 0, .i2cmux = 1}
			}
		},
		.i2cmux = {
			.values = { 0x00, 0x01 },
			.n_values = 2,
			.gpios = { 65, 64 },
			.n_gpios= 2,
		},
	},{
		.boardname = {"CAR3040"},
		.slot = {
			{
				{.bus = 0x00, .devfn = 0x08, .slotno = 1, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"COS5700"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x10, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x11, .slotno = 1, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x08, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x09, .slotno = 2, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
        .boardname = {"CAR2051"},
        .slot = {
            {
                { .bus = 0x00, .devfn = 0x20, .slotno = 3, .extend = 0, .i2cmux = 0},
            }
        },
        .i2cmux = {
            .n_values = 0,
        },
	},{
		.boardname = {"CAR3050"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x09, .slotno = 1, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x0A, .slotno = 1, .extend = 0, .i2cmux = 0},
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR5040"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x12, .slotno = 1, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x00, .devfn = 0x18, .slotno = 2, .extend = 0, .i2cmux = 0},
				{ .bus = 0x00, .devfn = 0x19, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x80, .devfn = 0x10, .slotno = 5, .extend = 0, .i2cmux = 1}
			},{
				{ .bus = 0x80, .devfn = 0x08, .slotno = 6, .extend = 0, .i2cmux = 1}
			},{
				{ .bus = 0x80, .devfn = 0x12, .slotno = 7, .extend = 0, .i2cmux = 1}
			},{
				{ .bus = 0x80, .devfn = 0x18, .slotno = 8, .extend = 0, .i2cmux = 1},
				{ .bus = 0x80, .devfn = 0x19, .slotno = 8, .extend = 0, .i2cmux = 1}
			}
		},
		.i2cmux = {
			.values = { 0x00, 0x01 },
			.n_values = 2,
			.gpios = { 1 },
			.n_gpios= 1,
		},
	},{
		.boardname = {"CAR3231C-LD"},
		.slot = {
			{
				{ .bus = 0x02, .devfn = 0x08, .slotno = 2, .extend = 0, .i2cmux = 0}
			},{
				{ .bus = 0x02, .devfn = 0x48, .slotno = 3, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	},{
		.boardname = {"CAR3042C-LD"},
		.slot = {
			{
				{ .bus = 0x00, .devfn = 0x09, .slotno = 2, .extend = 0, .i2cmux = 0}
			}
		},
		.i2cmux = {
			.n_values = 0,
		},
	}
};

#endif
