/*******************************************************************************

  CASwell(R) Gen3 Bypass Linux driver
  Copyright(c) 2012 Angus Cheng <angus.cheng@cas-well.com>
                    Zeno  Lai   <zeno.lai@cas-well.com>

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

#ifndef _CSW_GXPCI_H_
#define _CSW_GXPCI_H_

#define GXPCI_CAS_HOST_REGS_OFFSET  0x500000

#define GXPCI_CAS_MMI_REGS_OFFSET   0x508000

#define GXPCI_CAS_BP_OFFSET			0x530000

#define PCIE_HOST_INTR_CPL_AVAIL	       0x1

/**
 * The maximum number of PCIE ports in one PCI domain,
 * including the root-complex port.
 */
#define MAX_PCIE_PORTS_PER_DOMAIN   64

enum TILE_FUNC_STAT {
	TILE_FUNC_UNREG,
	TILE_FUNC_REG
};

struct cas_host_regs_t {
	uint64_t status;
};

enum CAS_CMD_BP {
	CAS_CMD_BP_READ_OPT,
	CAS_CMD_BP_WRITE_OPT,
	CAS_CMD_BP_GET_STATUS,
	CAS_CMD_BP_SET_STATUS,
	CAS_CMD_BP_GET_INFO
};

/** Possible values of gxpci_host_regs::status. */
enum pcie_status {
  PCIE_STATUS_NOT_READY,         /**< Chip is not ready */
  PCIE_STATUS_READY,             /**< Chip ready */
  PCIE_STATUS_RUNNING,           /**< EP driver is running */
  PCIE_STATUS_RESET_CHIP,        /**< Reset the chip */
  PCIE_STATUS_HALT_CHIP,         /**< Halt the chip (cannot reboot). */
};

struct cas_bp_cmd_t {
	uint8_t cwid;
	uint16_t cmd;
};

struct tilera_bypass_info_t {
	uint8_t instance;
	uint8_t channel;
};

struct cas_cmd_t {
	uint64_t complete;
	uint64_t opcmd;
	uint8_t h2t_data[128];
	uint8_t t2h_data[2048];
	uint64_t h2t_size;
	uint64_t t2h_size;
};

struct cas_disp_priv_t {
	struct cas_host_regs_t __iomem *host_regs;
	uint64_t __iomem *irq;
	struct cas_cmd_t __iomem *cmd;
	spinlock_t lock;
};

int csw_tilecard_init(void);
void csw_tilecard_exit(void);

#endif

