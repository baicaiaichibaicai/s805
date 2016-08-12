/*******************************************************************************

  
  Copyright(c) 1999 - 2006 Intel Corporation. All rights reserved.
  
  This program is free software; you can redistribute it and/or modify it 
  under the terms of the GNU General Public License as published by the Free 
  Software Foundation; either version 2 of the License, or (at your option) 
  any later version.
  
  This program is distributed in the hope that it will be useful, but WITHOUT 
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for 
  more details.
  
  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59 
  Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  
  The full GNU General Public License is included in this distribution in the
  file called LICENSE.
  
  Contact Information:
  Linux NICS <linux.nics@intel.com>
  bp_addressing-devel Mailing List <bp_addressing-devel@lists.sourceforge.net>
  Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497

*******************************************************************************/

/* bp_addressing_hw.c
 * Shared functions for accessing and configuring the MAC
 */

#include "addressing_hw.h"
static int32_t bp_addressing_write_eeprom_spi(struct bp_addressing_hw *hw, uint16_t offset,
                                      uint16_t words, uint16_t *data);
static int32_t bp_addressing_write_eeprom_microwire(struct bp_addressing_hw *hw,
                                            uint16_t offset, uint16_t words,
                                            uint16_t *data);
static int32_t bp_addressing_spi_eeprom_ready(struct bp_addressing_hw *hw);
static void bp_addressing_raise_ee_clk(struct bp_addressing_hw *hw, uint32_t *eecd);
static void bp_addressing_lower_ee_clk(struct bp_addressing_hw *hw, uint32_t *eecd);
static void bp_addressing_shift_out_ee_bits(struct bp_addressing_hw *hw, uint16_t data,
                                    uint16_t count);
static uint16_t bp_addressing_shift_in_ee_bits(struct bp_addressing_hw *hw, uint16_t count);
static int32_t bp_addressing_acquire_eeprom(struct bp_addressing_hw *hw);
static void bp_addressing_release_eeprom(struct bp_addressing_hw *hw);
static void bp_addressing_standby_eeprom(struct bp_addressing_hw *hw);
static uint8_t bp_addressing_arc_subsystem_valid(struct bp_addressing_hw *hw);
static int32_t bp_addressing_commit_shadow_ram(struct bp_addressing_hw *hw);
static int32_t bp_addressing_get_hw_eeprom_semaphore(struct bp_addressing_hw *hw);
static boolean_t bp_addressing_is_onboard_nvm_eeprom(struct bp_addressing_hw *hw);
static int32_t bp_addressing_poll_eerd_eewr_done(struct bp_addressing_hw *hw, int eerd);
static void bp_addressing_put_hw_eeprom_semaphore(struct bp_addressing_hw *hw);
static int32_t bp_addressing_read_eeprom_eerd(struct bp_addressing_hw *hw, uint16_t offset,
				      uint16_t words, uint16_t *data);
static int32_t bp_addressing_erase_ich8_4k_segment(struct bp_addressing_hw *hw,
					   uint32_t segment);
static int32_t bp_addressing_get_software_flag(struct bp_addressing_hw *hw);
static int32_t bp_addressing_get_software_semaphore(struct bp_addressing_hw *hw);
static int32_t bp_addressing_read_eeprom_ich8(struct bp_addressing_hw *hw, uint16_t offset,
				      uint16_t words, uint16_t *data);
static int32_t bp_addressing_read_ich8_byte(struct bp_addressing_hw *hw, uint32_t index,
				    uint8_t* data);
static int32_t bp_addressing_read_ich8_word(struct bp_addressing_hw *hw, uint32_t index,
				    uint16_t *data);
static void bp_addressing_release_software_flag(struct bp_addressing_hw *hw);
static int32_t bp_addressing_verify_write_ich8_byte(struct bp_addressing_hw *hw,
					    uint32_t index, uint8_t byte);
static int32_t bp_addressing_write_eeprom_ich8(struct bp_addressing_hw *hw, uint16_t offset,
				       uint16_t words, uint16_t *data);
static int32_t bp_addressing_write_ich8_byte(struct bp_addressing_hw *hw, uint32_t index,
				     uint8_t data);

//jason
/******************************************************************************
 * Set the mac type member in the hw struct.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
int32_t
bp_addressing_set_mac_type(struct bp_addressing_hw *hw)
{
    DEBUGFUNC("bp_addressing_set_mac_type");

    switch (hw->device_id) {
    case BP_ADDRESSING_DEV_ID_82542:
        switch (hw->revision_id) {
        case BP_ADDRESSING_82542_2_1_REV_ID:
            hw->mac_type = bp_addressing_82542_rev2_1;
            break;
        default:
            /* Invalid 82542 revision ID */
            return -BP_ADDRESSING_ERR_MAC_TYPE;
        }
        break;
    case BP_ADDRESSING_DEV_ID_82543GC_FIBER:
    case BP_ADDRESSING_DEV_ID_82543GC_COPPER:
        hw->mac_type = bp_addressing_82543;
        break;
    case BP_ADDRESSING_DEV_ID_82544EI_COPPER:
    case BP_ADDRESSING_DEV_ID_82544EI_FIBER:
    case BP_ADDRESSING_DEV_ID_82544GC_COPPER:
    case BP_ADDRESSING_DEV_ID_82544GC_LOM:
        hw->mac_type = bp_addressing_82544;
        break;
    case BP_ADDRESSING_DEV_ID_82540EM:
    case BP_ADDRESSING_DEV_ID_82540EM_LOM:
    case BP_ADDRESSING_DEV_ID_82540EP:
    case BP_ADDRESSING_DEV_ID_82540EP_LOM:
    case BP_ADDRESSING_DEV_ID_82540EP_LP:
        hw->mac_type = bp_addressing_82540;
        break;
    case BP_ADDRESSING_DEV_ID_82545EM_COPPER:
    case BP_ADDRESSING_DEV_ID_82545EM_FIBER:
        hw->mac_type = bp_addressing_82545;
        break;
    case BP_ADDRESSING_DEV_ID_82545GM_COPPER:
    case BP_ADDRESSING_DEV_ID_82545GM_FIBER:
    case BP_ADDRESSING_DEV_ID_82545GM_SERDES:
        hw->mac_type = bp_addressing_82545_rev_3;
        break;
    case BP_ADDRESSING_DEV_ID_82546EB_COPPER:
    case BP_ADDRESSING_DEV_ID_82546EB_FIBER:
    case BP_ADDRESSING_DEV_ID_82546EB_QUAD_COPPER:
        hw->mac_type = bp_addressing_82546;
        break;
    case BP_ADDRESSING_DEV_ID_82546GB_COPPER:
    case BP_ADDRESSING_DEV_ID_82546GB_FIBER:
    case BP_ADDRESSING_DEV_ID_82546GB_SERDES:
    case BP_ADDRESSING_DEV_ID_82546GB_PCIE:
    case BP_ADDRESSING_DEV_ID_82546GB_QUAD_COPPER:
    case BP_ADDRESSING_DEV_ID_82546GB_QUAD_COPPER_KSP3:
        hw->mac_type = bp_addressing_82546_rev_3;
        break;
    case BP_ADDRESSING_DEV_ID_82541EI:
    case BP_ADDRESSING_DEV_ID_82541EI_MOBILE:
    case BP_ADDRESSING_DEV_ID_82541ER_LOM:
        hw->mac_type = bp_addressing_82541;
        break;
    case BP_ADDRESSING_DEV_ID_82541ER:
    case BP_ADDRESSING_DEV_ID_82541GI:
    case BP_ADDRESSING_DEV_ID_82541GI_LF:
    case BP_ADDRESSING_DEV_ID_82541GI_MOBILE:
        hw->mac_type = bp_addressing_82541_rev_2;
        break;
    case BP_ADDRESSING_DEV_ID_82547EI:
    case BP_ADDRESSING_DEV_ID_82547EI_MOBILE:
        hw->mac_type = bp_addressing_82547;
        break;
    case BP_ADDRESSING_DEV_ID_82547GI:
        hw->mac_type = bp_addressing_82547_rev_2;
        break;
    case BP_ADDRESSING_DEV_ID_82571EB_COPPER:
    case BP_ADDRESSING_DEV_ID_82571EB_FIBER:
    case BP_ADDRESSING_DEV_ID_82571EB_SERDES:
            hw->mac_type = bp_addressing_82571;
        break;
    case BP_ADDRESSING_DEV_ID_82572EI_COPPER:
    case BP_ADDRESSING_DEV_ID_82572EI_FIBER:
    case BP_ADDRESSING_DEV_ID_82572EI_SERDES:
    case BP_ADDRESSING_DEV_ID_82572EI:
        hw->mac_type = bp_addressing_82572;
        break;
    case BP_ADDRESSING_DEV_ID_82573E:
    case BP_ADDRESSING_DEV_ID_82573E_IAMT:
    case BP_ADDRESSING_DEV_ID_82573L:
        hw->mac_type = bp_addressing_82573;
        break;
    case BP_ADDRESSING_DEV_ID_80003ES2LAN_COPPER_SPT:
    case BP_ADDRESSING_DEV_ID_80003ES2LAN_SERDES_SPT:
    case BP_ADDRESSING_DEV_ID_80003ES2LAN_COPPER_DPT:
    case BP_ADDRESSING_DEV_ID_80003ES2LAN_SERDES_DPT:
        hw->mac_type = bp_addressing_80003es2lan;
        break;
    case BP_ADDRESSING_DEV_ID_ICH8_IGP_M_AMT:
    case BP_ADDRESSING_DEV_ID_ICH8_IGP_AMT:
    case BP_ADDRESSING_DEV_ID_ICH8_IGP_C:
    case BP_ADDRESSING_DEV_ID_ICH8_IFE:
    case BP_ADDRESSING_DEV_ID_ICH8_IGP_M:
        hw->mac_type = bp_addressing_ich8lan;
        break;
    default:
        /* Should never have loaded on this device */
        return -BP_ADDRESSING_ERR_MAC_TYPE;
    }

    switch(hw->mac_type) {
    case bp_addressing_ich8lan:
        hw->swfwhw_semaphore_present = TRUE;
        hw->asf_firmware_present = TRUE;
        break;
    case bp_addressing_80003es2lan:
        hw->swfw_sync_present = TRUE;
        /* fall through */
    case bp_addressing_82571:
    case bp_addressing_82572:
    case bp_addressing_82573:
        hw->eeprom_semaphore_present = TRUE;
        /* fall through */
    case bp_addressing_82541:
    case bp_addressing_82547:
    case bp_addressing_82541_rev_2:
    case bp_addressing_82547_rev_2:
        hw->asf_firmware_present = TRUE;
        break;
    default:
        break;
    }

    return BP_ADDRESSING_SUCCESS;
}
//jason
/*****************************************************************************
 * Set media type and TBI compatibility.
 *
 * hw - Struct containing variables accessed by shared code
 * **************************************************************************/
void
bp_addressing_set_media_type(struct bp_addressing_hw *hw)
{
    uint32_t status;

    DEBUGFUNC("bp_addressing_set_media_type");

    if(hw->mac_type != bp_addressing_82543) {
        /* tbi_compatibility is only valid on 82543 */
        hw->tbi_compatibility_en = FALSE;
    }

    switch (hw->device_id) {
    case BP_ADDRESSING_DEV_ID_82545GM_SERDES:
    case BP_ADDRESSING_DEV_ID_82546GB_SERDES:
    case BP_ADDRESSING_DEV_ID_82571EB_SERDES:
    case BP_ADDRESSING_DEV_ID_82572EI_SERDES:
    case BP_ADDRESSING_DEV_ID_80003ES2LAN_SERDES_DPT:
        hw->media_type = bp_addressing_media_type_internal_serdes;
        break;
    default:
        switch (hw->mac_type) {
        case bp_addressing_ich8lan:
        case bp_addressing_82573:
            /* The STATUS_TBIMODE bit is reserved or reused for the this
             * device.
             */
            hw->media_type = bp_addressing_media_type_copper;
            break;
        default:
            status = BP_ADDRESSING_READ_REG(hw, STATUS);
            if (status & BP_ADDRESSING_STATUS_TBIMODE) {
                hw->media_type = bp_addressing_media_type_fiber;
                /* tbi_compatibility not valid on fiber */
                hw->tbi_compatibility_en = FALSE;
            } else {
                hw->media_type = bp_addressing_media_type_copper;
            }
            break;
        }
    }
}

static int32_t
bp_addressing_swfw_sync_acquire(struct bp_addressing_hw *hw, uint16_t mask)
{
    uint32_t swfw_sync = 0;
    uint32_t swmask = mask;
    uint32_t fwmask = mask << 16;
    int32_t timeout = 200;

    DEBUGFUNC("bp_addressing_swfw_sync_acquire");

    if (hw->swfwhw_semaphore_present)
        return bp_addressing_get_software_flag(hw);

    if (!hw->swfw_sync_present)
        return bp_addressing_get_hw_eeprom_semaphore(hw);

    while(timeout) {
            if (bp_addressing_get_hw_eeprom_semaphore(hw))
                return -BP_ADDRESSING_ERR_SWFW_SYNC;

            swfw_sync = BP_ADDRESSING_READ_REG(hw, SW_FW_SYNC);
            if (!(swfw_sync & (fwmask | swmask))) {
                break;
            }

            /* firmware currently using resource (fwmask) */
            /* or other software thread currently using resource (swmask) */
            bp_addressing_put_hw_eeprom_semaphore(hw);
            msec_delay_irq(5);
            timeout--;
    }

    if (!timeout) {
        DEBUGOUT("Driver can't access resource, SW_FW_SYNC timeout.\n");
        return -BP_ADDRESSING_ERR_SWFW_SYNC;
    }

    swfw_sync |= swmask;
    BP_ADDRESSING_WRITE_REG(hw, SW_FW_SYNC, swfw_sync);

    bp_addressing_put_hw_eeprom_semaphore(hw);
    return BP_ADDRESSING_SUCCESS;
}

static void
bp_addressing_swfw_sync_release(struct bp_addressing_hw *hw, uint16_t mask)
{
    uint32_t swfw_sync;
    uint32_t swmask = mask;

    DEBUGFUNC("bp_addressing_swfw_sync_release");

    if (hw->swfwhw_semaphore_present) {
        bp_addressing_release_software_flag(hw);
        return;
    }

    if (!hw->swfw_sync_present) {
        bp_addressing_put_hw_eeprom_semaphore(hw);
        return;
    }

    /* if (bp_addressing_get_hw_eeprom_semaphore(hw))
     *    return -BP_ADDRESSING_ERR_SWFW_SYNC; */
    while (bp_addressing_get_hw_eeprom_semaphore(hw) != BP_ADDRESSING_SUCCESS);
        /* empty */

    swfw_sync = BP_ADDRESSING_READ_REG(hw, SW_FW_SYNC);
    swfw_sync &= ~swmask;
    BP_ADDRESSING_WRITE_REG(hw, SW_FW_SYNC, swfw_sync);

    bp_addressing_put_hw_eeprom_semaphore(hw);
}








//jason
/******************************************************************************
 * Sets up eeprom variables in the hw struct.  Must be called after mac_type
 * is configured.  Additionally, if this is ICH8, the flash controller GbE
 * registers must be mapped, or this will crash.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
int32_t
bp_addressing_init_eeprom_params(struct bp_addressing_hw *hw)
{
    struct bp_addressing_eeprom_info *eeprom = &hw->eeprom;
    uint32_t eecd = BP_ADDRESSING_READ_REG(hw, EECD);
    int32_t ret_val = BP_ADDRESSING_SUCCESS;
    uint16_t eeprom_size;

    DEBUGFUNC("bp_addressing_init_eeprom_params");

    switch (hw->mac_type) {
    case bp_addressing_82540:
    case bp_addressing_82545:
    case bp_addressing_82545_rev_3:
    case bp_addressing_82546:
    case bp_addressing_82546_rev_3:
        eeprom->type = bp_addressing_eeprom_microwire;
        eeprom->opcode_bits = 3;
        eeprom->delay_usec = 50;
        if(eecd & BP_ADDRESSING_EECD_SIZE) {
            eeprom->word_size = 256;
            eeprom->address_bits = 8;
        } else {
            eeprom->word_size = 64;
            eeprom->address_bits = 6;
        }
        eeprom->use_eerd = FALSE;
        eeprom->use_eewr = FALSE;
        break;
    case bp_addressing_82541:
    case bp_addressing_82541_rev_2:
    case bp_addressing_82547:
    case bp_addressing_82547_rev_2:
        if (eecd & BP_ADDRESSING_EECD_TYPE) {
            eeprom->type = bp_addressing_eeprom_spi;
            eeprom->opcode_bits = 8;
            eeprom->delay_usec = 1;
            if (eecd & BP_ADDRESSING_EECD_ADDR_BITS) {
                eeprom->page_size = 32;
                eeprom->address_bits = 16;
            } else {
                eeprom->page_size = 8;
                eeprom->address_bits = 8;
            }
        } else {
            eeprom->type = bp_addressing_eeprom_microwire;
            eeprom->opcode_bits = 3;
            eeprom->delay_usec = 50;
            if (eecd & BP_ADDRESSING_EECD_ADDR_BITS) {
                eeprom->word_size = 256;
                eeprom->address_bits = 8;
            } else {
                eeprom->word_size = 64;
                eeprom->address_bits = 6;
            }
        }
        eeprom->use_eerd = FALSE;
        eeprom->use_eewr = FALSE;
        break;
    case bp_addressing_82571:
    case bp_addressing_82572:
        eeprom->type = bp_addressing_eeprom_spi;
        eeprom->opcode_bits = 8;
        eeprom->delay_usec = 1;
        if (eecd & BP_ADDRESSING_EECD_ADDR_BITS) {
            eeprom->page_size = 32;
            eeprom->address_bits = 16;
        } else {
            eeprom->page_size = 8;
            eeprom->address_bits = 8;
        }
        eeprom->use_eerd = FALSE;
        eeprom->use_eewr = FALSE;
        break;
    case bp_addressing_82573:
        eeprom->type = bp_addressing_eeprom_spi;
        eeprom->opcode_bits = 8;
        eeprom->delay_usec = 1;
        if (eecd & BP_ADDRESSING_EECD_ADDR_BITS) {
            eeprom->page_size = 32;
            eeprom->address_bits = 16;
        } else {
            eeprom->page_size = 8;
            eeprom->address_bits = 8;
        }
        eeprom->use_eerd = TRUE;
        eeprom->use_eewr = TRUE;
        if(bp_addressing_is_onboard_nvm_eeprom(hw) == FALSE) {
            eeprom->type = bp_addressing_eeprom_flash;
            eeprom->word_size = 2048;

            /* Ensure that the Autonomous FLASH update bit is cleared due to
             * Flash update issue on parts which use a FLASH for NVM. */
            eecd &= ~BP_ADDRESSING_EECD_AUPDEN;
            BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        }
        break;
    case bp_addressing_80003es2lan:
        eeprom->type = bp_addressing_eeprom_spi;
        eeprom->opcode_bits = 8;
        eeprom->delay_usec = 1;
        if (eecd & BP_ADDRESSING_EECD_ADDR_BITS) {
            eeprom->page_size = 32;
            eeprom->address_bits = 16;
        } else {
            eeprom->page_size = 8;
            eeprom->address_bits = 8;
        }
        eeprom->use_eerd = TRUE;
        eeprom->use_eewr = FALSE;
        break;
    case bp_addressing_ich8lan:
    {
        int32_t  i = 0;
        uint32_t flash_size = BP_ADDRESSING_READ_ICH8_REG(hw, ICH8_FLASH_GFPREG);

        eeprom->type = bp_addressing_eeprom_ich8;
        eeprom->use_eerd = FALSE;
        eeprom->use_eewr = FALSE;
        eeprom->word_size = BP_ADDRESSING_SHADOW_RAM_WORDS;

        /* Zero the shadow RAM structure. But don't load it from NVM
         * so as to save time for driver init */
        if (hw->eeprom_shadow_ram != NULL) {
            for (i = 0; i < BP_ADDRESSING_SHADOW_RAM_WORDS; i++) {
                hw->eeprom_shadow_ram[i].modified = FALSE;
                hw->eeprom_shadow_ram[i].eeprom_word = 0xFFFF;
            }
        }

        hw->flash_base_addr = (flash_size & ICH8_GFPREG_BASE_MASK) *
                              ICH8_FLASH_SECTOR_SIZE;

        hw->flash_bank_size = ((flash_size >> 16) & ICH8_GFPREG_BASE_MASK) + 1;
        hw->flash_bank_size -= (flash_size & ICH8_GFPREG_BASE_MASK);
        hw->flash_bank_size *= ICH8_FLASH_SECTOR_SIZE;
        hw->flash_bank_size /= 2 * sizeof(uint16_t);

        break;
    }
    default:
        break;
    }

    if (eeprom->type == bp_addressing_eeprom_spi) {
        /* eeprom_size will be an enum [0..8] that maps to eeprom sizes 128B to
         * 32KB (incremented by powers of 2).
         */
        if(hw->mac_type <= bp_addressing_82547_rev_2) {
            /* Set to default value for initial eeprom read. */
            eeprom->word_size = 64;
            ret_val = bp_addressing_read_eeprom(hw, EEPROM_CFG, 1, &eeprom_size);
            if(ret_val)
                return ret_val;
            eeprom_size = (eeprom_size & EEPROM_SIZE_MASK) >> EEPROM_SIZE_SHIFT;
            /* 256B eeprom size was not supported in earlier hardware, so we
             * bump eeprom_size up one to ensure that "1" (which maps to 256B)
             * is never the result used in the shifting logic below. */
            if(eeprom_size)
                eeprom_size++;
        } else {
            eeprom_size = (uint16_t)((eecd & BP_ADDRESSING_EECD_SIZE_EX_MASK) >>
                          BP_ADDRESSING_EECD_SIZE_EX_SHIFT);
        }

        eeprom->word_size = 1 << (eeprom_size + EEPROM_WORD_SIZE_SHIFT);
    }
    return ret_val;
}

/******************************************************************************
 * Raises the EEPROM's clock input.
 *
 * hw - Struct containing variables accessed by shared code
 * eecd - EECD's current value
 *****************************************************************************/
static void
bp_addressing_raise_ee_clk(struct bp_addressing_hw *hw,
                   uint32_t *eecd)
{
    /* Raise the clock input to the EEPROM (by setting the SK bit), and then
     * wait <delay> microseconds.
     */
    *eecd = *eecd | BP_ADDRESSING_EECD_SK;
    BP_ADDRESSING_WRITE_REG(hw, EECD, *eecd);
    BP_ADDRESSING_WRITE_FLUSH(hw);
    udelay(hw->eeprom.delay_usec);
}

/******************************************************************************
 * Lowers the EEPROM's clock input.
 *
 * hw - Struct containing variables accessed by shared code
 * eecd - EECD's current value
 *****************************************************************************/
static void
bp_addressing_lower_ee_clk(struct bp_addressing_hw *hw,
                   uint32_t *eecd)
{
    /* Lower the clock input to the EEPROM (by clearing the SK bit), and then
     * wait 50 microseconds.
     */
    *eecd = *eecd & ~BP_ADDRESSING_EECD_SK;
    BP_ADDRESSING_WRITE_REG(hw, EECD, *eecd);
    BP_ADDRESSING_WRITE_FLUSH(hw);
    udelay(hw->eeprom.delay_usec);
}
//jason
/******************************************************************************
 * Shift data bits out to the EEPROM.
 *
 * hw - Struct containing variables accessed by shared code
 * data - data to send to the EEPROM
 * count - number of bits to shift out
 *****************************************************************************/
static void
bp_addressing_shift_out_ee_bits(struct bp_addressing_hw *hw,
                        uint16_t data,
                        uint16_t count)
{
    struct bp_addressing_eeprom_info *eeprom = &hw->eeprom;
    uint32_t eecd;
    uint32_t mask;

    /* We need to shift "count" bits out to the EEPROM. So, value in the
     * "data" parameter will be shifted out to the EEPROM one bit at a time.
     * In order to do this, "data" must be broken down into bits.
     */
    mask = 0x01 << (count - 1);
    eecd = BP_ADDRESSING_READ_REG(hw, EECD);
    if (eeprom->type == bp_addressing_eeprom_microwire) {
        eecd &= ~BP_ADDRESSING_EECD_DO;
    } else if (eeprom->type == bp_addressing_eeprom_spi) {
        eecd |= BP_ADDRESSING_EECD_DO;
    }
    do {
        /* A "1" is shifted out to the EEPROM by setting bit "DI" to a "1",
         * and then raising and then lowering the clock (the SK bit controls
         * the clock input to the EEPROM).  A "0" is shifted out to the EEPROM
         * by setting "DI" to "0" and then raising and then lowering the clock.
         */
        eecd &= ~BP_ADDRESSING_EECD_DI;

        if(data & mask)
            eecd |= BP_ADDRESSING_EECD_DI;

        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);

        udelay(eeprom->delay_usec);

        bp_addressing_raise_ee_clk(hw, &eecd);
        bp_addressing_lower_ee_clk(hw, &eecd);

        mask = mask >> 1;

    } while(mask);

    /* We leave the "DI" bit set to "0" when we leave this routine. */
    eecd &= ~BP_ADDRESSING_EECD_DI;
    BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
}
//jason
/******************************************************************************
 * Shift data bits in from the EEPROM
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static uint16_t
bp_addressing_shift_in_ee_bits(struct bp_addressing_hw *hw,
                       uint16_t count)
{
    uint32_t eecd;
    uint32_t i;
    uint16_t data;

    /* In order to read a register from the EEPROM, we need to shift 'count'
     * bits in from the EEPROM. Bits are "shifted in" by raising the clock
     * input to the EEPROM (setting the SK bit), and then reading the value of
     * the "DO" bit.  During this "shifting in" process the "DI" bit should
     * always be clear.
     */

    eecd = BP_ADDRESSING_READ_REG(hw, EECD);

    eecd &= ~(BP_ADDRESSING_EECD_DO | BP_ADDRESSING_EECD_DI);
    data = 0;

    for(i = 0; i < count; i++) {
        data = data << 1;
        bp_addressing_raise_ee_clk(hw, &eecd);

        eecd = BP_ADDRESSING_READ_REG(hw, EECD);

        eecd &= ~(BP_ADDRESSING_EECD_DI);
        if(eecd & BP_ADDRESSING_EECD_DO)
            data |= 1;

        bp_addressing_lower_ee_clk(hw, &eecd);
    }

    return data;
}
//jason
/******************************************************************************
 * Prepares EEPROM for access
 *
 * hw - Struct containing variables accessed by shared code
 *
 * Lowers EEPROM clock. Clears input pin. Sets the chip select pin. This
 * function should be called before issuing a command to the EEPROM.
 *****************************************************************************/
static int32_t
bp_addressing_acquire_eeprom(struct bp_addressing_hw *hw)
{
    struct bp_addressing_eeprom_info *eeprom = &hw->eeprom;
    uint32_t eecd, i=0;

    DEBUGFUNC("bp_addressing_acquire_eeprom");

    if (bp_addressing_swfw_sync_acquire(hw, BP_ADDRESSING_SWFW_EEP_SM))
        return -BP_ADDRESSING_ERR_SWFW_SYNC;
    eecd = BP_ADDRESSING_READ_REG(hw, EECD);

    if (hw->mac_type != bp_addressing_82573) {
        /* Request EEPROM Access */
        if(hw->mac_type > bp_addressing_82544) {
            eecd |= BP_ADDRESSING_EECD_REQ;
            BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
            eecd = BP_ADDRESSING_READ_REG(hw, EECD);
            while((!(eecd & BP_ADDRESSING_EECD_GNT)) &&
                  (i < BP_ADDRESSING_EEPROM_GRANT_ATTEMPTS)) {
                i++;
                udelay(5);
                eecd = BP_ADDRESSING_READ_REG(hw, EECD);
            }
            if(!(eecd & BP_ADDRESSING_EECD_GNT)) {
                eecd &= ~BP_ADDRESSING_EECD_REQ;
                BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
                DEBUGOUT("Could not acquire EEPROM grant\n");
                bp_addressing_swfw_sync_release(hw, BP_ADDRESSING_SWFW_EEP_SM);
                return -BP_ADDRESSING_ERR_EEPROM;
            }
        }
    }

    /* Setup EEPROM for Read/Write */

    if (eeprom->type == bp_addressing_eeprom_microwire) {
        /* Clear SK and DI */
        eecd &= ~(BP_ADDRESSING_EECD_DI | BP_ADDRESSING_EECD_SK);
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);

        /* Set CS */
        eecd |= BP_ADDRESSING_EECD_CS;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
    } else if (eeprom->type == bp_addressing_eeprom_spi) {
        /* Clear SK and CS */
        eecd &= ~(BP_ADDRESSING_EECD_CS | BP_ADDRESSING_EECD_SK);
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        udelay(1);
    }

    return BP_ADDRESSING_SUCCESS;
}
//jason
/******************************************************************************
 * Returns EEPROM to a "standby" state
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static void
bp_addressing_standby_eeprom(struct bp_addressing_hw *hw)
{
    struct bp_addressing_eeprom_info *eeprom = &hw->eeprom;
    uint32_t eecd;

    eecd = BP_ADDRESSING_READ_REG(hw, EECD);

    if(eeprom->type == bp_addressing_eeprom_microwire) {
        eecd &= ~(BP_ADDRESSING_EECD_CS | BP_ADDRESSING_EECD_SK);
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);
        udelay(eeprom->delay_usec);

        /* Clock high */
        eecd |= BP_ADDRESSING_EECD_SK;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);
        udelay(eeprom->delay_usec);

        /* Select EEPROM */
        eecd |= BP_ADDRESSING_EECD_CS;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);
        udelay(eeprom->delay_usec);

        /* Clock low */
        eecd &= ~BP_ADDRESSING_EECD_SK;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);
        udelay(eeprom->delay_usec);
    } else if(eeprom->type == bp_addressing_eeprom_spi) {
        /* Toggle CS to flush commands */
        eecd |= BP_ADDRESSING_EECD_CS;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);
        udelay(eeprom->delay_usec);
        eecd &= ~BP_ADDRESSING_EECD_CS;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);
        udelay(eeprom->delay_usec);
    }
}
//jason
/******************************************************************************
 * Terminates a command by inverting the EEPROM's chip select pin
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static void
bp_addressing_release_eeprom(struct bp_addressing_hw *hw)
{
    uint32_t eecd;

    DEBUGFUNC("bp_addressing_release_eeprom");

    eecd = BP_ADDRESSING_READ_REG(hw, EECD);

    if (hw->eeprom.type == bp_addressing_eeprom_spi) {
        eecd |= BP_ADDRESSING_EECD_CS;  /* Pull CS high */
        eecd &= ~BP_ADDRESSING_EECD_SK; /* Lower SCK */

        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);

        udelay(hw->eeprom.delay_usec);
    } else if(hw->eeprom.type == bp_addressing_eeprom_microwire) {
        /* cleanup eeprom */

        /* CS on Microwire is active-high */
        eecd &= ~(BP_ADDRESSING_EECD_CS | BP_ADDRESSING_EECD_DI);

        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);

        /* Rising edge of clock */
        eecd |= BP_ADDRESSING_EECD_SK;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);
        udelay(hw->eeprom.delay_usec);

        /* Falling edge of clock */
        eecd &= ~BP_ADDRESSING_EECD_SK;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
        BP_ADDRESSING_WRITE_FLUSH(hw);
        udelay(hw->eeprom.delay_usec);
    }

    /* Stop requesting EEPROM access */
    if(hw->mac_type > bp_addressing_82544) {
        eecd &= ~BP_ADDRESSING_EECD_REQ;
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd);
    }

    bp_addressing_swfw_sync_release(hw, BP_ADDRESSING_SWFW_EEP_SM);
}
//jason
/******************************************************************************
 * Reads a 16 bit word from the EEPROM.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
int32_t
bp_addressing_spi_eeprom_ready(struct bp_addressing_hw *hw)
{
    uint16_t retry_count = 0;
    uint8_t spi_stat_reg;

    DEBUGFUNC("bp_addressing_spi_eeprom_ready");

    /* Read "Status Register" repeatedly until the LSB is cleared.  The
     * EEPROM will signal that the command has been completed by clearing
     * bit 0 of the internal status register.  If it's not cleared within
     * 5 milliseconds, then error out.
     */
    retry_count = 0;
    do {
        bp_addressing_shift_out_ee_bits(hw, EEPROM_RDSR_OPCODE_SPI,
                                hw->eeprom.opcode_bits);
        spi_stat_reg = (uint8_t)bp_addressing_shift_in_ee_bits(hw, 8);
        if (!(spi_stat_reg & EEPROM_STATUS_RDY_SPI))
            break;

        udelay(5);
        retry_count += 5;

        bp_addressing_standby_eeprom(hw);
    } while(retry_count < EEPROM_MAX_RETRY_SPI);

    /* ATMEL SPI write time could vary from 0-20mSec on 3.3V devices (and
     * only 0-5mSec on 5V devices)
     */
    if(retry_count >= EEPROM_MAX_RETRY_SPI) {
        DEBUGOUT("SPI EEPROM Status error\n");
        return -BP_ADDRESSING_ERR_EEPROM;
    }

    return BP_ADDRESSING_SUCCESS;
}
//jason 
/******************************************************************************
 * Reads a 16 bit word from the EEPROM.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset of  word in the EEPROM to read
 * data - word read from the EEPROM
 * words - number of words to read
 *****************************************************************************/
int32_t
bp_addressing_read_eeprom(struct bp_addressing_hw *hw,
                  uint16_t offset,
                  uint16_t words,
                  uint16_t *data)
{
    struct bp_addressing_eeprom_info *eeprom = &hw->eeprom;
    uint32_t i = 0;
    int32_t ret_val;

    DEBUGFUNC("bp_addressing_read_eeprom");

    /* A check for invalid values:  offset too large, too many words, and not
     * enough words.
     */
    if((offset >= eeprom->word_size) || (words > eeprom->word_size - offset) ||
       (words == 0)) {
        DEBUGOUT("\"words\" parameter out of bounds\n");
        return -BP_ADDRESSING_ERR_EEPROM;
    }

    /* FLASH reads without acquiring the semaphore are safe */
    if (bp_addressing_is_onboard_nvm_eeprom(hw) == TRUE &&
    hw->eeprom.use_eerd == FALSE) {
        switch (hw->mac_type) {
        case bp_addressing_80003es2lan:
            break;
        default:
            /* Prepare the EEPROM for reading  */
            if (bp_addressing_acquire_eeprom(hw) != BP_ADDRESSING_SUCCESS)
                return -BP_ADDRESSING_ERR_EEPROM;
            break;
        }
    }

    if (eeprom->use_eerd == TRUE) {
        ret_val = bp_addressing_read_eeprom_eerd(hw, offset, words, data);
        if ((bp_addressing_is_onboard_nvm_eeprom(hw) == TRUE) ||
            (hw->mac_type != bp_addressing_82573))
            bp_addressing_release_eeprom(hw);
        return ret_val;
    }

    if (eeprom->type == bp_addressing_eeprom_ich8)
        return bp_addressing_read_eeprom_ich8(hw, offset, words, data);

    if (eeprom->type == bp_addressing_eeprom_spi) {
        uint16_t word_in;
        uint8_t read_opcode = EEPROM_READ_OPCODE_SPI;

        if(bp_addressing_spi_eeprom_ready(hw)) {
            bp_addressing_release_eeprom(hw);
            return -BP_ADDRESSING_ERR_EEPROM;
        }

        bp_addressing_standby_eeprom(hw);

        /* Some SPI eeproms use the 8th address bit embedded in the opcode */
        if((eeprom->address_bits == 8) && (offset >= 128))
            read_opcode |= EEPROM_A8_OPCODE_SPI;

        /* Send the READ command (opcode + addr)  */
        bp_addressing_shift_out_ee_bits(hw, read_opcode, eeprom->opcode_bits);
        bp_addressing_shift_out_ee_bits(hw, (uint16_t)(offset*2), eeprom->address_bits);

        /* Read the data.  The address of the eeprom internally increments with
         * each byte (spi) being read, saving on the overhead of eeprom setup
         * and tear-down.  The address counter will roll over if reading beyond
         * the size of the eeprom, thus allowing the entire memory to be read
         * starting from any offset. */
        for (i = 0; i < words; i++) {
            word_in = bp_addressing_shift_in_ee_bits(hw, 16);
            data[i] = (word_in >> 8) | (word_in << 8);
        }
    } else if(eeprom->type == bp_addressing_eeprom_microwire) {
        for (i = 0; i < words; i++) {
            /* Send the READ command (opcode + addr)  */
            bp_addressing_shift_out_ee_bits(hw, EEPROM_READ_OPCODE_MICROWIRE,
                                    eeprom->opcode_bits);
            bp_addressing_shift_out_ee_bits(hw, (uint16_t)(offset + i),
                                    eeprom->address_bits);

            /* Read the data.  For microwire, each word requires the overhead
             * of eeprom setup and tear-down. */
            data[i] = bp_addressing_shift_in_ee_bits(hw, 16);
            bp_addressing_standby_eeprom(hw);
        }
    }

    /* End this read operation */
    bp_addressing_release_eeprom(hw);

    return BP_ADDRESSING_SUCCESS;
}
//jason
/******************************************************************************
 * Reads a 16 bit word from the EEPROM using the EERD register.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset of  word in the EEPROM to read
 * data - word read from the EEPROM
 * words - number of words to read
 *****************************************************************************/
static int32_t
bp_addressing_read_eeprom_eerd(struct bp_addressing_hw *hw,
                  uint16_t offset,
                  uint16_t words,
                  uint16_t *data)
{
    uint32_t i, eerd = 0;
    int32_t error = 0;

    for (i = 0; i < words; i++) {
        eerd = ((offset+i) << BP_ADDRESSING_EEPROM_RW_ADDR_SHIFT) +
                         BP_ADDRESSING_EEPROM_RW_REG_START;

        BP_ADDRESSING_WRITE_REG(hw, EERD, eerd);
        error = bp_addressing_poll_eerd_eewr_done(hw, BP_ADDRESSING_EEPROM_POLL_READ);

        if(error) {
            break;
        }
        data[i] = (BP_ADDRESSING_READ_REG(hw, EERD) >> BP_ADDRESSING_EEPROM_RW_REG_DATA);

    }

    return error;
}

/******************************************************************************
 * Writes a 16 bit word from the EEPROM using the EEWR register.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset of  word in the EEPROM to read
 * data - word read from the EEPROM
 * words - number of words to read
 *****************************************************************************/
static int32_t
bp_addressing_write_eeprom_eewr(struct bp_addressing_hw *hw,
                   uint16_t offset,
                   uint16_t words,
                   uint16_t *data)
{
    uint32_t    register_value = 0;
    uint32_t    i              = 0;
    int32_t     error          = 0;

    if (bp_addressing_swfw_sync_acquire(hw, BP_ADDRESSING_SWFW_EEP_SM))
        return -BP_ADDRESSING_ERR_SWFW_SYNC;

    for (i = 0; i < words; i++) {
        register_value = (data[i] << BP_ADDRESSING_EEPROM_RW_REG_DATA) |
                         ((offset+i) << BP_ADDRESSING_EEPROM_RW_ADDR_SHIFT) |
                         BP_ADDRESSING_EEPROM_RW_REG_START;

        error = bp_addressing_poll_eerd_eewr_done(hw, BP_ADDRESSING_EEPROM_POLL_WRITE);
        if(error) {
            break;
        }

        BP_ADDRESSING_WRITE_REG(hw, EEWR, register_value);

        error = bp_addressing_poll_eerd_eewr_done(hw, BP_ADDRESSING_EEPROM_POLL_WRITE);

        if(error) {
            break;
        }
    }

    bp_addressing_swfw_sync_release(hw, BP_ADDRESSING_SWFW_EEP_SM);
    return error;
}

/******************************************************************************
 * Polls the status bit (bit 1) of the EERD to determine when the read is done.
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
static int32_t
bp_addressing_poll_eerd_eewr_done(struct bp_addressing_hw *hw, int eerd)
{
    uint32_t attempts = 100000;
    uint32_t i, reg = 0;
    int32_t done = BP_ADDRESSING_ERR_EEPROM;

    for(i = 0; i < attempts; i++) {
        if(eerd == BP_ADDRESSING_EEPROM_POLL_READ)
            reg = BP_ADDRESSING_READ_REG(hw, EERD);
        else
            reg = BP_ADDRESSING_READ_REG(hw, EEWR);

        if(reg & BP_ADDRESSING_EEPROM_RW_REG_DONE) {
            done = BP_ADDRESSING_SUCCESS;
            break;
        }
        udelay(5);
    }

    return done;
}
//jaosn
/***************************************************************************
* Description:     Determines if the onboard NVM is FLASH or EEPROM.
*
* hw - Struct containing variables accessed by shared code
****************************************************************************/
static boolean_t
bp_addressing_is_onboard_nvm_eeprom(struct bp_addressing_hw *hw)
{
    uint32_t eecd = 0;

    DEBUGFUNC("bp_addressing_is_onboard_nvm_eeprom");

    if (hw->mac_type == bp_addressing_ich8lan)
        return FALSE;

    if (hw->mac_type == bp_addressing_82573) {
        eecd = BP_ADDRESSING_READ_REG(hw, EECD);

        /* Isolate bits 15 & 16 */
        eecd = ((eecd >> 15) & 0x03);

        /* If both bits are set, device is Flash type */
        if(eecd == 0x03) {
            return FALSE;
        }
    }
    return TRUE;
}
//jason
/******************************************************************************
 * Verifies that the EEPROM has a valid checksum
 *
 * hw - Struct containing variables accessed by shared code
 *
 * Reads the first 64 16 bit words of the EEPROM and sums the values read.
 * If the the sum of the 64 16 bit words is 0xBABA, the EEPROM's checksum is
 * valid.
 *****************************************************************************/
int32_t
bp_addressing_validate_eeprom_checksum(struct bp_addressing_hw *hw)
{
    uint16_t checksum = 0;
    uint16_t i, eeprom_data;

    DEBUGFUNC("bp_addressing_validate_eeprom_checksum");

    if ((hw->mac_type == bp_addressing_82573) &&
        (bp_addressing_is_onboard_nvm_eeprom(hw) == FALSE)) {
        /* Check bit 4 of word 10h.  If it is 0, firmware is done updating
         * 10h-12h.  Checksum may need to be fixed. */
        bp_addressing_read_eeprom(hw, 0x10, 1, &eeprom_data);
        if ((eeprom_data & 0x10) == 0) {
            /* Read 0x23 and check bit 15.  This bit is a 1 when the checksum
             * has already been fixed.  If the checksum is still wrong and this
             * bit is a 1, we need to return bad checksum.  Otherwise, we need
             * to set this bit to a 1 and update the checksum. */
            bp_addressing_read_eeprom(hw, 0x23, 1, &eeprom_data);
            if ((eeprom_data & 0x8000) == 0) {
                eeprom_data |= 0x8000;
                bp_addressing_write_eeprom(hw, 0x23, 1, &eeprom_data);
                bp_addressing_update_eeprom_checksum(hw);
            }
        }
    }

    if (hw->mac_type == bp_addressing_ich8lan) {
        /* Drivers must allocate the shadow ram structure for the
         * EEPROM checksum to be updated.  Otherwise, this bit as well
         * as the checksum must both be set correctly for this
         * validation to pass.
         */
        bp_addressing_read_eeprom(hw, 0x19, 1, &eeprom_data);
        if ((eeprom_data & 0x40) == 0) {
            eeprom_data |= 0x40;
            bp_addressing_write_eeprom(hw, 0x19, 1, &eeprom_data);
            bp_addressing_update_eeprom_checksum(hw);
        }
    }

    for (i = 0; i < (EEPROM_CHECKSUM_REG + 1); i++) {
        if (bp_addressing_read_eeprom(hw, i, 1, &eeprom_data) < 0) {
            DEBUGOUT("EEPROM Read Error\n");
            return -BP_ADDRESSING_ERR_EEPROM;
        }
        checksum += eeprom_data;
    }

    if(checksum == (uint16_t) EEPROM_SUM)
        return BP_ADDRESSING_SUCCESS;
    else {
        DEBUGOUT("EEPROM Checksum Invalid\n");
        return -BP_ADDRESSING_ERR_EEPROM;
    }
}
//jason
/******************************************************************************
 * Calculates the EEPROM checksum and writes it to the EEPROM
 *
 * hw - Struct containing variables accessed by shared code
 *
 * Sums the first 63 16 bit words of the EEPROM. Subtracts the sum from 0xBABA.
 * Writes the difference to word offset 63 of the EEPROM.
 *****************************************************************************/
int32_t
bp_addressing_update_eeprom_checksum(struct bp_addressing_hw *hw)
{
    uint32_t ctrl_ext;
    uint16_t checksum = 0;
    uint16_t i, eeprom_data;

    DEBUGFUNC("bp_addressing_update_eeprom_checksum");

    for(i = 0; i < EEPROM_CHECKSUM_REG; i++) {
        if(bp_addressing_read_eeprom(hw, i, 1, &eeprom_data) < 0) {
            DEBUGOUT("EEPROM Read Error\n");
            return -BP_ADDRESSING_ERR_EEPROM;
        }
        checksum += eeprom_data;
    }
    checksum = (uint16_t) EEPROM_SUM - checksum;
    if(bp_addressing_write_eeprom(hw, EEPROM_CHECKSUM_REG, 1, &checksum) < 0) {
        DEBUGOUT("EEPROM Write Error\n");
        return -BP_ADDRESSING_ERR_EEPROM;
    } else if (hw->eeprom.type == bp_addressing_eeprom_flash) {
        bp_addressing_commit_shadow_ram(hw);
    } else if (hw->eeprom.type == bp_addressing_eeprom_ich8) {
        bp_addressing_commit_shadow_ram(hw);
        /* Reload the EEPROM, or else modifications will not appear
         * until after next adapter reset. */
        ctrl_ext = BP_ADDRESSING_READ_REG(hw, CTRL_EXT);
        ctrl_ext |= BP_ADDRESSING_CTRL_EXT_EE_RST;
        BP_ADDRESSING_WRITE_REG(hw, CTRL_EXT, ctrl_ext);
        msec_delay(10);
    }
    return BP_ADDRESSING_SUCCESS;
}
//jason
/******************************************************************************
 * Parent function for writing words to the different EEPROM types.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset within the EEPROM to be written to
 * words - number of words to write
 * data - 16 bit word to be written to the EEPROM
 *
 * If bp_addressing_update_eeprom_checksum is not called after this function, the
 * EEPROM will most likely contain an invalid checksum.
 *****************************************************************************/
int32_t
bp_addressing_write_eeprom(struct bp_addressing_hw *hw,
                   uint16_t offset,
                   uint16_t words,
                   uint16_t *data)
{
    struct bp_addressing_eeprom_info *eeprom = &hw->eeprom;
    int32_t status = 0;

    DEBUGFUNC("bp_addressing_write_eeprom");

    /* A check for invalid values:  offset too large, too many words, and not
     * enough words.
     */
    if((offset >= eeprom->word_size) || (words > eeprom->word_size - offset) ||
       (words == 0)) {
        DEBUGOUT("\"words\" parameter out of bounds\n");
        return -BP_ADDRESSING_ERR_EEPROM;
    }

    /* 82573 writes only through eewr */
    if(eeprom->use_eewr == TRUE)
        return bp_addressing_write_eeprom_eewr(hw, offset, words, data);

    if (eeprom->type == bp_addressing_eeprom_ich8)
        return bp_addressing_write_eeprom_ich8(hw, offset, words, data);

    /* Prepare the EEPROM for writing  */
    if (bp_addressing_acquire_eeprom(hw) != BP_ADDRESSING_SUCCESS)
        return -BP_ADDRESSING_ERR_EEPROM;

    if(eeprom->type == bp_addressing_eeprom_microwire) {
        status = bp_addressing_write_eeprom_microwire(hw, offset, words, data);
    } else {
        status = bp_addressing_write_eeprom_spi(hw, offset, words, data);
        msec_delay(10);
    }

    /* Done with writing */
    bp_addressing_release_eeprom(hw);

    return status;
}

/******************************************************************************
 * Writes a 16 bit word to a given offset in an SPI EEPROM.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset within the EEPROM to be written to
 * words - number of words to write
 * data - pointer to array of 8 bit words to be written to the EEPROM
 *
 *****************************************************************************/
int32_t
bp_addressing_write_eeprom_spi(struct bp_addressing_hw *hw,
                       uint16_t offset,
                       uint16_t words,
                       uint16_t *data)
{
    struct bp_addressing_eeprom_info *eeprom = &hw->eeprom;
    uint16_t widx = 0;

    DEBUGFUNC("bp_addressing_write_eeprom_spi");

    while (widx < words) {
        uint8_t write_opcode = EEPROM_WRITE_OPCODE_SPI;

        if(bp_addressing_spi_eeprom_ready(hw)) return -BP_ADDRESSING_ERR_EEPROM;

        bp_addressing_standby_eeprom(hw);

        /*  Send the WRITE ENABLE command (8 bit opcode )  */
        bp_addressing_shift_out_ee_bits(hw, EEPROM_WREN_OPCODE_SPI,
                                    eeprom->opcode_bits);

        bp_addressing_standby_eeprom(hw);

        /* Some SPI eeproms use the 8th address bit embedded in the opcode */
        if((eeprom->address_bits == 8) && (offset >= 128))
            write_opcode |= EEPROM_A8_OPCODE_SPI;

        /* Send the Write command (8-bit opcode + addr) */
        bp_addressing_shift_out_ee_bits(hw, write_opcode, eeprom->opcode_bits);

        bp_addressing_shift_out_ee_bits(hw, (uint16_t)((offset + widx)*2),
                                eeprom->address_bits);

        /* Send the data */

        /* Loop to allow for up to whole page write (32 bytes) of eeprom */
        while (widx < words) {
            uint16_t word_out = data[widx];
            word_out = (word_out >> 8) | (word_out << 8);
            bp_addressing_shift_out_ee_bits(hw, word_out, 16);
            widx++;

            /* Some larger eeprom sizes are capable of a 32-byte PAGE WRITE
             * operation, while the smaller eeproms are capable of an 8-byte
             * PAGE WRITE operation.  Break the inner loop to pass new address
             */
            if((((offset + widx)*2) % eeprom->page_size) == 0) {
                bp_addressing_standby_eeprom(hw);
                break;
            }
        }
    }

    return BP_ADDRESSING_SUCCESS;
}

/******************************************************************************
 * Writes a 16 bit word to a given offset in a Microwire EEPROM.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset within the EEPROM to be written to
 * words - number of words to write
 * data - pointer to array of 16 bit words to be written to the EEPROM
 *
 *****************************************************************************/
int32_t
bp_addressing_write_eeprom_microwire(struct bp_addressing_hw *hw,
                             uint16_t offset,
                             uint16_t words,
                             uint16_t *data)
{
    struct bp_addressing_eeprom_info *eeprom = &hw->eeprom;
    uint32_t eecd;
    uint16_t words_written = 0;
    uint16_t i = 0;

    DEBUGFUNC("bp_addressing_write_eeprom_microwire");

    /* Send the write enable command to the EEPROM (3-bit opcode plus
     * 6/8-bit dummy address beginning with 11).  It's less work to include
     * the 11 of the dummy address as part of the opcode than it is to shift
     * it over the correct number of bits for the address.  This puts the
     * EEPROM into write/erase mode.
     */
    bp_addressing_shift_out_ee_bits(hw, EEPROM_EWEN_OPCODE_MICROWIRE,
                            (uint16_t)(eeprom->opcode_bits + 2));

    bp_addressing_shift_out_ee_bits(hw, 0, (uint16_t)(eeprom->address_bits - 2));

    /* Prepare the EEPROM */
    bp_addressing_standby_eeprom(hw);

    while (words_written < words) {
        /* Send the Write command (3-bit opcode + addr) */
        bp_addressing_shift_out_ee_bits(hw, EEPROM_WRITE_OPCODE_MICROWIRE,
                                eeprom->opcode_bits);

        bp_addressing_shift_out_ee_bits(hw, (uint16_t)(offset + words_written),
                                eeprom->address_bits);

        /* Send the data */
        bp_addressing_shift_out_ee_bits(hw, data[words_written], 16);

        /* Toggle the CS line.  This in effect tells the EEPROM to execute
         * the previous command.
         */
        bp_addressing_standby_eeprom(hw);

        /* Read DO repeatedly until it is high (equal to '1').  The EEPROM will
         * signal that the command has been completed by raising the DO signal.
         * If DO does not go high in 10 milliseconds, then error out.
         */
        for(i = 0; i < 200; i++) {
            eecd = BP_ADDRESSING_READ_REG(hw, EECD);
            if(eecd & BP_ADDRESSING_EECD_DO) break;
            udelay(50);
        }
        if(i == 200) {
            DEBUGOUT("EEPROM Write did not complete\n");
            return -BP_ADDRESSING_ERR_EEPROM;
        }

        /* Recover from write */
        bp_addressing_standby_eeprom(hw);

        words_written++;
    }

    /* Send the write disable command to the EEPROM (3-bit opcode plus
     * 6/8-bit dummy address beginning with 10).  It's less work to include
     * the 10 of the dummy address as part of the opcode than it is to shift
     * it over the correct number of bits for the address.  This takes the
     * EEPROM out of write/erase mode.
     */
    bp_addressing_shift_out_ee_bits(hw, EEPROM_EWDS_OPCODE_MICROWIRE,
                            (uint16_t)(eeprom->opcode_bits + 2));

    bp_addressing_shift_out_ee_bits(hw, 0, (uint16_t)(eeprom->address_bits - 2));

    return BP_ADDRESSING_SUCCESS;
}

/******************************************************************************
 * Flushes the cached eeprom to NVM. This is done by saving the modified values
 * in the eeprom cache and the non modified values in the currently active bank
 * to the new bank.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset of  word in the EEPROM to read
 * data - word read from the EEPROM
 * words - number of words to read
 *****************************************************************************/
static int32_t
bp_addressing_commit_shadow_ram(struct bp_addressing_hw *hw)
{
    uint32_t attempts = 100000;
    uint32_t eecd = 0;
    uint32_t flop = 0;
    uint32_t i = 0;
    int32_t error = BP_ADDRESSING_SUCCESS;
    uint32_t old_bank_offset = 0;
    uint32_t new_bank_offset = 0;
    uint32_t sector_retries = 0;
    uint8_t low_byte = 0;
    uint8_t high_byte = 0;
    uint8_t temp_byte = 0;
    boolean_t sector_write_failed = FALSE;

    if (hw->mac_type == bp_addressing_82573) {
        /* The flop register will be used to determine if flash type is STM */
        flop = BP_ADDRESSING_READ_REG(hw, FLOP);
        for (i=0; i < attempts; i++) {
            eecd = BP_ADDRESSING_READ_REG(hw, EECD);
            if ((eecd & BP_ADDRESSING_EECD_FLUPD) == 0) {
                break;
            }
            udelay(5);
        }

        if (i == attempts) {
            return -BP_ADDRESSING_ERR_EEPROM;
        }

        /* If STM opcode located in bits 15:8 of flop, reset firmware */
        if ((flop & 0xFF00) == BP_ADDRESSING_STM_OPCODE) {
            BP_ADDRESSING_WRITE_REG(hw, HICR, BP_ADDRESSING_HICR_FW_RESET);
        }

        /* Perform the flash update */
        BP_ADDRESSING_WRITE_REG(hw, EECD, eecd | BP_ADDRESSING_EECD_FLUPD);

        for (i=0; i < attempts; i++) {
            eecd = BP_ADDRESSING_READ_REG(hw, EECD);
            if ((eecd & BP_ADDRESSING_EECD_FLUPD) == 0) {
                break;
            }
            udelay(5);
        }

        if (i == attempts) {
            return -BP_ADDRESSING_ERR_EEPROM;
        }
    }

    if (hw->mac_type == bp_addressing_ich8lan && hw->eeprom_shadow_ram != NULL) {
        /* We're writing to the opposite bank so if we're on bank 1,
         * write to bank 0 etc.  We also need to erase the segment that
         * is going to be written */
        if (!(BP_ADDRESSING_READ_REG(hw, EECD) & BP_ADDRESSING_EECD_SEC1VAL)) {
            new_bank_offset = hw->flash_bank_size * 2;
            old_bank_offset = 0;
            bp_addressing_erase_ich8_4k_segment(hw, 1);
        } else {
            old_bank_offset = hw->flash_bank_size * 2;
            new_bank_offset = 0;
            bp_addressing_erase_ich8_4k_segment(hw, 0);
        }

        do {
            sector_write_failed = FALSE;
            /* Loop for every byte in the shadow RAM,
             * which is in units of words. */
            for (i = 0; i < BP_ADDRESSING_SHADOW_RAM_WORDS; i++) {
                /* Determine whether to write the value stored
                 * in the other NVM bank or a modified value stored
                 * in the shadow RAM */
                if (hw->eeprom_shadow_ram[i].modified == TRUE) {
                    low_byte = (uint8_t)hw->eeprom_shadow_ram[i].eeprom_word;
                    bp_addressing_read_ich8_byte(hw, (i << 1) + old_bank_offset,
                                         &temp_byte);
                    udelay(100);
                    error = bp_addressing_verify_write_ich8_byte(hw,
                                                 (i << 1) + new_bank_offset,
                                                 low_byte);
                    if (error != BP_ADDRESSING_SUCCESS)
                        sector_write_failed = TRUE;
                    high_byte =
                        (uint8_t)(hw->eeprom_shadow_ram[i].eeprom_word >> 8);
                    bp_addressing_read_ich8_byte(hw, (i << 1) + old_bank_offset + 1,
                                         &temp_byte);
                    udelay(100);
                } else {
                    bp_addressing_read_ich8_byte(hw, (i << 1) + old_bank_offset,
                                         &low_byte);
                    udelay(100);
                    error = bp_addressing_verify_write_ich8_byte(hw,
                                 (i << 1) + new_bank_offset, low_byte);
                    if (error != BP_ADDRESSING_SUCCESS)
                        sector_write_failed = TRUE;
                    bp_addressing_read_ich8_byte(hw, (i << 1) + old_bank_offset + 1,
                                         &high_byte);
                }

                /* If the word is 0x13, then make sure the signature bits
                 * (15:14) are 11b until the commit has completed.
                 * This will allow us to write 10b which indicates the
                 * signature is valid.  We want to do this after the write
                 * has completed so that we don't mark the segment valid
                 * while the write is still in progress */
                if (i == BP_ADDRESSING_ICH8_NVM_SIG_WORD)
                    high_byte = BP_ADDRESSING_ICH8_NVM_SIG_MASK | high_byte;

                error = bp_addressing_verify_write_ich8_byte(hw,
                             (i << 1) + new_bank_offset + 1, high_byte);
                if (error != BP_ADDRESSING_SUCCESS)
                    sector_write_failed = TRUE;

                if (sector_write_failed == FALSE) {
                    /* Clear the now not used entry in the cache */
                    hw->eeprom_shadow_ram[i].modified = FALSE;
                    hw->eeprom_shadow_ram[i].eeprom_word = 0xFFFF;
                }
            }

            /* Don't bother writing the segment valid bits if sector
             * programming failed. */
            if (sector_write_failed == FALSE) {
                /* Finally validate the new segment by setting bit 15:14
                 * to 10b in word 0x13 , this can be done without an
                 * erase as well since these bits are 11 to start with
                 * and we need to change bit 14 to 0b */
                bp_addressing_read_ich8_byte(hw,
                    BP_ADDRESSING_ICH8_NVM_SIG_WORD * 2 + 1 + new_bank_offset,
                    &high_byte);
                high_byte &= 0xBF;
                error = bp_addressing_verify_write_ich8_byte(hw,
                            BP_ADDRESSING_ICH8_NVM_SIG_WORD * 2 + 1 + new_bank_offset,
                            high_byte);
                if (error != BP_ADDRESSING_SUCCESS)
                    sector_write_failed = TRUE;

                /* And invalidate the previously valid segment by setting
                 * its signature word (0x13) high_byte to 0b. This can be
                 * done without an erase because flash erase sets all bits
                 * to 1's. We can write 1's to 0's without an erase */
                error = bp_addressing_verify_write_ich8_byte(hw,
                            BP_ADDRESSING_ICH8_NVM_SIG_WORD * 2 + 1 + old_bank_offset,
                            0);
                if (error != BP_ADDRESSING_SUCCESS)
                    sector_write_failed = TRUE;
            }
        } while (++sector_retries < 10 && sector_write_failed == TRUE);
    }

    return error;
}
//jason
/******************************************************************************
 * Reads the adapter's part number from the EEPROM
 *
 * hw - Struct containing variables accessed by shared code
 * part_num - Adapter's part number
 *****************************************************************************/
int32_t
bp_addressing_read_part_num(struct bp_addressing_hw *hw,
                    uint32_t *part_num)
{
    uint16_t offset = EEPROM_PBA_BYTE_1;
    uint16_t eeprom_data;

    DEBUGFUNC("bp_addressing_read_part_num");

    /* Get word 0 from EEPROM */
    if(bp_addressing_read_eeprom(hw, offset, 1, &eeprom_data) < 0) {
        DEBUGOUT("EEPROM Read Error\n");
        return -BP_ADDRESSING_ERR_EEPROM;
    }
    /* Save word 0 in upper half of part_num */
    *part_num = (uint32_t) (eeprom_data << 16);

    /* Get word 1 from EEPROM */
    if(bp_addressing_read_eeprom(hw, ++offset, 1, &eeprom_data) < 0) {
        DEBUGOUT("EEPROM Read Error\n");
        return -BP_ADDRESSING_ERR_EEPROM;
    }
    /* Save word 1 in lower half of part_num */
    *part_num |= eeprom_data;

    return BP_ADDRESSING_SUCCESS;
}
//jason
/******************************************************************************
 * Reads the adapter's MAC address from the EEPROM and inverts the LSB for the
 * second function of dual function devices
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
int32_t
bp_addressing_read_mac_addr(struct bp_addressing_hw * hw)
{
    uint16_t offset;
    uint16_t eeprom_data, i;

    DEBUGFUNC("bp_addressing_read_mac_addr");

    for(i = 0; i < NODE_ADDRESS_SIZE; i += 2) {
        offset = i >> 1;
        if(bp_addressing_read_eeprom(hw, offset, 1, &eeprom_data) < 0) {
            DEBUGOUT("EEPROM Read Error\n");
            return -BP_ADDRESSING_ERR_EEPROM;
        }
        hw->perm_mac_addr[i] = (uint8_t) (eeprom_data & 0x00FF);
        hw->perm_mac_addr[i+1] = (uint8_t) (eeprom_data >> 8);
    }

    switch (hw->mac_type) {
    default:
        break;
    case bp_addressing_82546:
    case bp_addressing_82546_rev_3:
    case bp_addressing_82571:
    case bp_addressing_80003es2lan:
        if(BP_ADDRESSING_READ_REG(hw, STATUS) & BP_ADDRESSING_STATUS_FUNC_1)
            hw->perm_mac_addr[5] ^= 0x01;
        break;
    }

    for(i = 0; i < NODE_ADDRESS_SIZE; i++)
        hw->mac_addr[i] = hw->perm_mac_addr[i];
    return BP_ADDRESSING_SUCCESS;
}

//jason
/******************************************************************************
 * Gets the current PCI bus type, speed, and width of the hardware
 *
 * hw - Struct containing variables accessed by shared code
 *****************************************************************************/
void
bp_addressing_get_bus_info(struct bp_addressing_hw *hw)
{
    uint32_t status;

    switch (hw->mac_type) {
    case bp_addressing_82572:
    case bp_addressing_82573:
        hw->bus_type = bp_addressing_bus_type_pci_express;
        hw->bus_speed = bp_addressing_bus_speed_2500;
        hw->bus_width = bp_addressing_bus_width_pciex_1;
        break;
    case bp_addressing_82571:
    case bp_addressing_ich8lan:
    case bp_addressing_80003es2lan:
        hw->bus_type = bp_addressing_bus_type_pci_express;
        hw->bus_speed = bp_addressing_bus_speed_2500;
        hw->bus_width = bp_addressing_bus_width_pciex_4;
        break;
    default:
        status = BP_ADDRESSING_READ_REG(hw, STATUS);
        hw->bus_type = (status & BP_ADDRESSING_STATUS_PCIX_MODE) ?
                       bp_addressing_bus_type_pcix : bp_addressing_bus_type_pci;

        if(hw->device_id == BP_ADDRESSING_DEV_ID_82546EB_QUAD_COPPER) {
            hw->bus_speed = (hw->bus_type == bp_addressing_bus_type_pci) ?
                            bp_addressing_bus_speed_66 : bp_addressing_bus_speed_120;
        } else if(hw->bus_type == bp_addressing_bus_type_pci) {
            hw->bus_speed = (status & BP_ADDRESSING_STATUS_PCI66) ?
                            bp_addressing_bus_speed_66 : bp_addressing_bus_speed_33;
        } else {
            switch (status & BP_ADDRESSING_STATUS_PCIX_SPEED) {
            case BP_ADDRESSING_STATUS_PCIX_SPEED_66:
                hw->bus_speed = bp_addressing_bus_speed_66;
                break;
            case BP_ADDRESSING_STATUS_PCIX_SPEED_100:
                hw->bus_speed = bp_addressing_bus_speed_100;
                break;
            case BP_ADDRESSING_STATUS_PCIX_SPEED_133:
                hw->bus_speed = bp_addressing_bus_speed_133;
                break;
            default:
                hw->bus_speed = bp_addressing_bus_speed_reserved;
                break;
            }
        }
        hw->bus_width = (status & BP_ADDRESSING_STATUS_BUS64) ?
                        bp_addressing_bus_width_64 : bp_addressing_bus_width_32;
        break;
    }
}

//jason
/******************************************************************************
 * Verifies the hardware needs to allow ARPs to be processed by the host
 *
 * hw - Struct containing variables accessed by shared code
 *
 * returns: - TRUE/FALSE
 *
 *****************************************************************************/
uint32_t
bp_addressing_enable_mng_pass_thru(struct bp_addressing_hw *hw)
{
    uint32_t manc;
    uint32_t fwsm, factps;

    if (hw->asf_firmware_present) {
        manc = BP_ADDRESSING_READ_REG(hw, MANC);

        if (!(manc & BP_ADDRESSING_MANC_RCV_TCO_EN) ||
            !(manc & BP_ADDRESSING_MANC_EN_MAC_ADDR_FILTER))
            return FALSE;
        if (bp_addressing_arc_subsystem_valid(hw) == TRUE) {
            fwsm = BP_ADDRESSING_READ_REG(hw, FWSM);
            factps = BP_ADDRESSING_READ_REG(hw, FACTPS);

            if (((fwsm & BP_ADDRESSING_FWSM_MODE_MASK) ==
                (bp_addressing_mng_mode_pt << BP_ADDRESSING_FWSM_MODE_SHIFT)) &&
                (factps & BP_ADDRESSING_FACTPS_MNGCG))
                return TRUE;
        } else
            if ((manc & BP_ADDRESSING_MANC_SMBUS_EN) && !(manc & BP_ADDRESSING_MANC_ASF_EN))
                return TRUE;
    }
    return FALSE;
}


/***************************************************************************
 *
 * Using the combination of SMBI and SWESMBI semaphore bits when resetting
 * adapter or Eeprom access.
 *
 * hw: Struct containing variables accessed by shared code
 *
 * returns: - BP_ADDRESSING_ERR_EEPROM if fail to access EEPROM.
 *            BP_ADDRESSING_SUCCESS at any other case.
 *
 ***************************************************************************/
static int32_t
bp_addressing_get_hw_eeprom_semaphore(struct bp_addressing_hw *hw)
{
    int32_t timeout;
    uint32_t swsm;

    DEBUGFUNC("bp_addressing_get_hw_eeprom_semaphore");

    if(!hw->eeprom_semaphore_present)
        return BP_ADDRESSING_SUCCESS;

    if (hw->mac_type == bp_addressing_80003es2lan) {
        /* Get the SW semaphore. */
        if (bp_addressing_get_software_semaphore(hw) != BP_ADDRESSING_SUCCESS)
            return -BP_ADDRESSING_ERR_EEPROM;
    }

    /* Get the FW semaphore. */
    timeout = hw->eeprom.word_size + 1;
    while(timeout) {
        swsm = BP_ADDRESSING_READ_REG(hw, SWSM);
        swsm |= BP_ADDRESSING_SWSM_SWESMBI;
        BP_ADDRESSING_WRITE_REG(hw, SWSM, swsm);
        /* if we managed to set the bit we got the semaphore. */
        swsm = BP_ADDRESSING_READ_REG(hw, SWSM);
        if(swsm & BP_ADDRESSING_SWSM_SWESMBI)
            break;

        udelay(50);
        timeout--;
    }

    if(!timeout) {
        /* Release semaphores */
        bp_addressing_put_hw_eeprom_semaphore(hw);
        DEBUGOUT("Driver can't access the Eeprom - SWESMBI bit is set.\n");
        return -BP_ADDRESSING_ERR_EEPROM;
    }

    return BP_ADDRESSING_SUCCESS;
}

/***************************************************************************
 * This function clears HW semaphore bits.
 *
 * hw: Struct containing variables accessed by shared code
 *
 * returns: - None.
 *
 ***************************************************************************/
static void
bp_addressing_put_hw_eeprom_semaphore(struct bp_addressing_hw *hw)
{
    uint32_t swsm;

    DEBUGFUNC("bp_addressing_put_hw_eeprom_semaphore");

    if(!hw->eeprom_semaphore_present)
        return;

    swsm = BP_ADDRESSING_READ_REG(hw, SWSM);
    if (hw->mac_type == bp_addressing_80003es2lan) {
        /* Release both semaphores. */
        swsm &= ~(BP_ADDRESSING_SWSM_SMBI | BP_ADDRESSING_SWSM_SWESMBI);
    } else
        swsm &= ~(BP_ADDRESSING_SWSM_SWESMBI);
    BP_ADDRESSING_WRITE_REG(hw, SWSM, swsm);
}

/***************************************************************************
 *
 * Obtaining software semaphore bit (SMBI) before resetting PHY.
 *
 * hw: Struct containing variables accessed by shared code
 *
 * returns: - BP_ADDRESSING_ERR_RESET if fail to obtain semaphore.
 *            BP_ADDRESSING_SUCCESS at any other case.
 *
 ***************************************************************************/
static int32_t
bp_addressing_get_software_semaphore(struct bp_addressing_hw *hw)
{
    int32_t timeout = hw->eeprom.word_size + 1;
    uint32_t swsm;

    DEBUGFUNC("bp_addressing_get_software_semaphore");

    if (hw->mac_type != bp_addressing_80003es2lan)
        return BP_ADDRESSING_SUCCESS;

    while(timeout) {
        swsm = BP_ADDRESSING_READ_REG(hw, SWSM);
        /* If SMBI bit cleared, it is now set and we hold the semaphore */
        if(!(swsm & BP_ADDRESSING_SWSM_SMBI))
            break;
        msec_delay_irq(1);
        timeout--;
    }

    if(!timeout) {
        DEBUGOUT("Driver can't access device - SMBI bit is set.\n");
        return -BP_ADDRESSING_ERR_RESET;
    }

    return BP_ADDRESSING_SUCCESS;
}

//jason
/******************************************************************************
 * Checks if PHY reset is blocked due to SOL/IDER session, for example.
 * Returning BP_ADDRESSING_BLK_PHY_RESET isn't necessarily an error.  But it's up to
 * the caller to figure out how to deal with it.
 *
 * hw - Struct containing variables accessed by shared code
 *
 * returns: - BP_ADDRESSING_BLK_PHY_RESET
 *            BP_ADDRESSING_SUCCESS
 *
 *****************************************************************************/
int32_t
bp_addressing_check_phy_reset_block(struct bp_addressing_hw *hw)
{
    uint32_t manc = 0;
    uint32_t fwsm = 0;

    if (hw->mac_type == bp_addressing_ich8lan) {
        fwsm = BP_ADDRESSING_READ_REG(hw, FWSM);
        return (fwsm & BP_ADDRESSING_FWSM_RSPCIPHY) ? BP_ADDRESSING_SUCCESS
                                            : BP_ADDRESSING_BLK_PHY_RESET;
    }

    if (hw->mac_type > bp_addressing_82547_rev_2)
        manc = BP_ADDRESSING_READ_REG(hw, MANC);
    return (manc & BP_ADDRESSING_MANC_BLK_PHY_RST_ON_IDE) ?
	    BP_ADDRESSING_BLK_PHY_RESET : BP_ADDRESSING_SUCCESS;
}

static uint8_t
bp_addressing_arc_subsystem_valid(struct bp_addressing_hw *hw)
{
    uint32_t fwsm;

    /* On 8257x silicon, registers in the range of 0x8800 - 0x8FFC
     * may not be provided a DMA clock when no manageability features are
     * enabled.  We do not want to perform any reads/writes to these registers
     * if this is the case.  We read FWSM to determine the manageability mode.
     */
    switch (hw->mac_type) {
    case bp_addressing_82571:
    case bp_addressing_82572:
    case bp_addressing_82573:
    case bp_addressing_80003es2lan:
        fwsm = BP_ADDRESSING_READ_REG(hw, FWSM);
        if((fwsm & BP_ADDRESSING_FWSM_MODE_MASK) != 0)
            return TRUE;
        break;
    case bp_addressing_ich8lan:
        return TRUE;
    default:
        break;
    }
    return FALSE;
}

/***************************************************************************
 *
 * Get software semaphore FLAG bit (SWFLAG).
 * SWFLAG is used to synchronize the access to all shared resource between
 * SW, FW and HW.
 *
 * hw: Struct containing variables accessed by shared code
 *
 ***************************************************************************/
static int32_t
bp_addressing_get_software_flag(struct bp_addressing_hw *hw)
{
    int32_t timeout = PHY_CFG_TIMEOUT;
    uint32_t extcnf_ctrl;

    DEBUGFUNC("bp_addressing_get_software_flag");

    if (hw->mac_type == bp_addressing_ich8lan) {
        while (timeout) {
            extcnf_ctrl = BP_ADDRESSING_READ_REG(hw, EXTCNF_CTRL);
            extcnf_ctrl |= BP_ADDRESSING_EXTCNF_CTRL_SWFLAG;
            BP_ADDRESSING_WRITE_REG(hw, EXTCNF_CTRL, extcnf_ctrl);

            extcnf_ctrl = BP_ADDRESSING_READ_REG(hw, EXTCNF_CTRL);
            if (extcnf_ctrl & BP_ADDRESSING_EXTCNF_CTRL_SWFLAG)
                break;
            msec_delay_irq(1);
            timeout--;
        }

        if (!timeout) {
            DEBUGOUT("FW or HW locks the resource too long.\n");
            return -BP_ADDRESSING_ERR_CONFIG;
        }
    }

    return BP_ADDRESSING_SUCCESS;
}

/***************************************************************************
 *
 * Release software semaphore FLAG bit (SWFLAG).
 * SWFLAG is used to synchronize the access to all shared resource between
 * SW, FW and HW.
 *
 * hw: Struct containing variables accessed by shared code
 *
 ***************************************************************************/
static void
bp_addressing_release_software_flag(struct bp_addressing_hw *hw)
{
    uint32_t extcnf_ctrl;

    DEBUGFUNC("bp_addressing_release_software_flag");

    if (hw->mac_type == bp_addressing_ich8lan) {
        extcnf_ctrl= BP_ADDRESSING_READ_REG(hw, EXTCNF_CTRL);
        extcnf_ctrl &= ~BP_ADDRESSING_EXTCNF_CTRL_SWFLAG;
        BP_ADDRESSING_WRITE_REG(hw, EXTCNF_CTRL, extcnf_ctrl);
    }

    return;
}


//jason
/******************************************************************************
 * Reads a 16 bit word or words from the EEPROM using the ICH8's flash access
 * register.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset of word in the EEPROM to read
 * data - word read from the EEPROM
 * words - number of words to read
 *****************************************************************************/
static int32_t
bp_addressing_read_eeprom_ich8(struct bp_addressing_hw *hw, uint16_t offset, uint16_t words,
                       uint16_t *data)
{
    int32_t  error = BP_ADDRESSING_SUCCESS;
    uint32_t flash_bank = 0;
    uint32_t act_offset = 0;
    uint32_t bank_offset = 0;
    uint16_t word = 0;
    uint16_t i = 0;

    /* We need to know which is the valid flash bank.  In the event
     * that we didn't allocate eeprom_shadow_ram, we may not be
     * managing flash_bank.  So it cannot be trusted and needs
     * to be updated with each read.
     */
    /* Value of bit 22 corresponds to the flash bank we're on. */
    flash_bank = (BP_ADDRESSING_READ_REG(hw, EECD) & BP_ADDRESSING_EECD_SEC1VAL) ? 1 : 0;

    /* Adjust offset appropriately if we're on bank 1 - adjust for word size */
    bank_offset = flash_bank * (hw->flash_bank_size * 2);

    error = bp_addressing_get_software_flag(hw);
    if (error != BP_ADDRESSING_SUCCESS)
        return error;

    for (i = 0; i < words; i++) {
        if (hw->eeprom_shadow_ram != NULL &&
            hw->eeprom_shadow_ram[offset+i].modified == TRUE) {
            data[i] = hw->eeprom_shadow_ram[offset+i].eeprom_word;
        } else {
            /* The NVM part needs a byte offset, hence * 2 */
            act_offset = bank_offset + ((offset + i) * 2);
            error = bp_addressing_read_ich8_word(hw, act_offset, &word);
            if (error != BP_ADDRESSING_SUCCESS)
                break;
            data[i] = word;
        }
    }

    bp_addressing_release_software_flag(hw);

    return error;
}

/******************************************************************************
 * Writes a 16 bit word or words to the EEPROM using the ICH8's flash access
 * register.  Actually, writes are written to the shadow ram cache in the hw
 * structure hw->bp_addressing_shadow_ram.  bp_addressing_commit_shadow_ram flushes this to
 * the NVM, which occurs when the NVM checksum is updated.
 *
 * hw - Struct containing variables accessed by shared code
 * offset - offset of word in the EEPROM to write
 * words - number of words to write
 * data - words to write to the EEPROM
 *****************************************************************************/
static int32_t
bp_addressing_write_eeprom_ich8(struct bp_addressing_hw *hw, uint16_t offset, uint16_t words,
                        uint16_t *data)
{
    uint32_t i = 0;
    int32_t error = BP_ADDRESSING_SUCCESS;

    error = bp_addressing_get_software_flag(hw);
    if (error != BP_ADDRESSING_SUCCESS)
        return error;

    /* A driver can write to the NVM only if it has eeprom_shadow_ram
     * allocated.  Subsequent reads to the modified words are read from
     * this cached structure as well.  Writes will only go into this
     * cached structure unless it's followed by a call to
     * bp_addressing_update_eeprom_checksum() where it will commit the changes
     * and clear the "modified" field.
     */
    if (hw->eeprom_shadow_ram != NULL) {
        for (i = 0; i < words; i++) {
            if ((offset + i) < BP_ADDRESSING_SHADOW_RAM_WORDS) {
                hw->eeprom_shadow_ram[offset+i].modified = TRUE;
                hw->eeprom_shadow_ram[offset+i].eeprom_word = data[i];
            } else {
                error = -BP_ADDRESSING_ERR_EEPROM;
                break;
            }
        }
    } else {
        /* Drivers have the option to not allocate eeprom_shadow_ram as long
         * as they don't perform any NVM writes.  An attempt in doing so
         * will result in this error.
         */
        error = -BP_ADDRESSING_ERR_EEPROM;
    }

    bp_addressing_release_software_flag(hw);

    return error;
}

/******************************************************************************
 * This function does initial flash setup so that a new read/write/erase cycle
 * can be started.
 *
 * hw - The pointer to the hw structure
 ****************************************************************************/
static int32_t
bp_addressing_ich8_cycle_init(struct bp_addressing_hw *hw)
{
    union ich8_hws_flash_status hsfsts;
    int32_t error = BP_ADDRESSING_ERR_EEPROM;
    int32_t i     = 0;

    DEBUGFUNC("bp_addressing_ich8_cycle_init");

    hsfsts.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFSTS);

    /* May be check the Flash Des Valid bit in Hw status */
    if (hsfsts.hsf_status.fldesvalid == 0) {
        DEBUGOUT("Flash descriptor invalid.  SW Sequencing must be used.");
        return error;
    }

    /* Clear FCERR in Hw status by writing 1 */
    /* Clear DAEL in Hw status by writing a 1 */
    hsfsts.hsf_status.flcerr = 1;
    hsfsts.hsf_status.dael = 1;

    BP_ADDRESSING_WRITE_ICH8_REG16(hw, ICH8_FLASH_HSFSTS, hsfsts.regval);

    /* Either we should have a hardware SPI cycle in progress bit to check
     * against, in order to start a new cycle or FDONE bit should be changed
     * in the hardware so that it is 1 after harware reset, which can then be
     * used as an indication whether a cycle is in progress or has been
     * completed .. we should also have some software semaphore mechanism to
     * guard FDONE or the cycle in progress bit so that two threads access to
     * those bits can be sequentiallized or a way so that 2 threads dont
     * start the cycle at the same time */

    if (hsfsts.hsf_status.flcinprog == 0) {
        /* There is no cycle running at present, so we can start a cycle */
        /* Begin by setting Flash Cycle Done. */
        hsfsts.hsf_status.flcdone = 1;
        BP_ADDRESSING_WRITE_ICH8_REG16(hw, ICH8_FLASH_HSFSTS, hsfsts.regval);
        error = BP_ADDRESSING_SUCCESS;
    } else {
        /* otherwise poll for sometime so the current cycle has a chance
         * to end before giving up. */
        for (i = 0; i < ICH8_FLASH_COMMAND_TIMEOUT; i++) {
            hsfsts.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFSTS);
            if (hsfsts.hsf_status.flcinprog == 0) {
                error = BP_ADDRESSING_SUCCESS;
                break;
            }
            udelay(1);
        }
        if (error == BP_ADDRESSING_SUCCESS) {
            /* Successful in waiting for previous cycle to timeout,
             * now set the Flash Cycle Done. */
            hsfsts.hsf_status.flcdone = 1;
            BP_ADDRESSING_WRITE_ICH8_REG16(hw, ICH8_FLASH_HSFSTS, hsfsts.regval);
        } else {
            DEBUGOUT("Flash controller busy, cannot get access");
        }
    }
    return error;
}

/******************************************************************************
 * This function starts a flash cycle and waits for its completion
 *
 * hw - The pointer to the hw structure
 ****************************************************************************/
static int32_t
bp_addressing_ich8_flash_cycle(struct bp_addressing_hw *hw, uint32_t timeout)
{
    union ich8_hws_flash_ctrl hsflctl;
    union ich8_hws_flash_status hsfsts;
    int32_t error = BP_ADDRESSING_ERR_EEPROM;
    uint32_t i = 0;

    /* Start a cycle by writing 1 in Flash Cycle Go in Hw Flash Control */
    hsflctl.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFCTL);
    hsflctl.hsf_ctrl.flcgo = 1;
    BP_ADDRESSING_WRITE_ICH8_REG16(hw, ICH8_FLASH_HSFCTL, hsflctl.regval);

    /* wait till FDONE bit is set to 1 */
    do {
        hsfsts.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFSTS);
        if (hsfsts.hsf_status.flcdone == 1)
            break;
        udelay(1);
        i++;
    } while (i < timeout);
    if (hsfsts.hsf_status.flcdone == 1 && hsfsts.hsf_status.flcerr == 0) {
        error = BP_ADDRESSING_SUCCESS;
    }
    return error;
}

/******************************************************************************
 * Reads a byte or word from the NVM using the ICH8 flash access registers.
 *
 * hw - The pointer to the hw structure
 * index - The index of the byte or word to read.
 * size - Size of data to read, 1=byte 2=word
 * data - Pointer to the word to store the value read.
 *****************************************************************************/
static int32_t
bp_addressing_read_ich8_data(struct bp_addressing_hw *hw, uint32_t index,
                     uint32_t size, uint16_t* data)
{
    union ich8_hws_flash_status hsfsts;
    union ich8_hws_flash_ctrl hsflctl;
    uint32_t flash_linear_address;
    uint32_t flash_data = 0;
    int32_t error = -BP_ADDRESSING_ERR_EEPROM;
    int32_t count = 0;

    DEBUGFUNC("bp_addressing_read_ich8_data");

    if (size < 1  || size > 2 || data == 0x0 ||
        index > ICH8_FLASH_LINEAR_ADDR_MASK)
        return error;

    flash_linear_address = (ICH8_FLASH_LINEAR_ADDR_MASK & index) +
                           hw->flash_base_addr;

    do {
        udelay(1);
        /* Steps */
        error = bp_addressing_ich8_cycle_init(hw);
        if (error != BP_ADDRESSING_SUCCESS)
            break;

        hsflctl.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFCTL);
        /* 0b/1b corresponds to 1 or 2 byte size, respectively. */
        hsflctl.hsf_ctrl.fldbcount = size - 1;
        hsflctl.hsf_ctrl.flcycle = ICH8_CYCLE_READ;
        BP_ADDRESSING_WRITE_ICH8_REG16(hw, ICH8_FLASH_HSFCTL, hsflctl.regval);

        /* Write the last 24 bits of index into Flash Linear address field in
         * Flash Address */
        /* TODO: TBD maybe check the index against the size of flash */

        BP_ADDRESSING_WRITE_ICH8_REG(hw, ICH8_FLASH_FADDR, flash_linear_address);

        error = bp_addressing_ich8_flash_cycle(hw, ICH8_FLASH_COMMAND_TIMEOUT);

        /* Check if FCERR is set to 1, if set to 1, clear it and try the whole
         * sequence a few more times, else read in (shift in) the Flash Data0,
         * the order is least significant byte first msb to lsb */
        if (error == BP_ADDRESSING_SUCCESS) {
            flash_data = BP_ADDRESSING_READ_ICH8_REG(hw, ICH8_FLASH_FDATA0);
            if (size == 1) {
                *data = (uint8_t)(flash_data & 0x000000FF);
            } else if (size == 2) {
                *data = (uint16_t)(flash_data & 0x0000FFFF);
            }
            break;
        } else {
            /* If we've gotten here, then things are probably completely hosed,
             * but if the error condition is detected, it won't hurt to give
             * it another try...ICH8_FLASH_CYCLE_REPEAT_COUNT times.
             */
            hsfsts.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFSTS);
            if (hsfsts.hsf_status.flcerr == 1) {
                /* Repeat for some time before giving up. */
                continue;
            } else if (hsfsts.hsf_status.flcdone == 0) {
                DEBUGOUT("Timeout error - flash cycle did not complete.");
                break;
            }
        }
    } while (count++ < ICH8_FLASH_CYCLE_REPEAT_COUNT);

    return error;
}

/******************************************************************************
 * Writes One /two bytes to the NVM using the ICH8 flash access registers.
 *
 * hw - The pointer to the hw structure
 * index - The index of the byte/word to read.
 * size - Size of data to read, 1=byte 2=word
 * data - The byte(s) to write to the NVM.
 *****************************************************************************/
static int32_t
bp_addressing_write_ich8_data(struct bp_addressing_hw *hw, uint32_t index, uint32_t size,
                      uint16_t data)
{
    union ich8_hws_flash_status hsfsts;
    union ich8_hws_flash_ctrl hsflctl;
    uint32_t flash_linear_address;
    uint32_t flash_data = 0;
    int32_t error = -BP_ADDRESSING_ERR_EEPROM;
    int32_t count = 0;

    DEBUGFUNC("bp_addressing_write_ich8_data");

    if (size < 1  || size > 2 || data > size * 0xff ||
        index > ICH8_FLASH_LINEAR_ADDR_MASK)
        return error;

    flash_linear_address = (ICH8_FLASH_LINEAR_ADDR_MASK & index) +
                           hw->flash_base_addr;

    do {
        udelay(1);
        /* Steps */
        error = bp_addressing_ich8_cycle_init(hw);
        if (error != BP_ADDRESSING_SUCCESS)
            break;

        hsflctl.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFCTL);
        /* 0b/1b corresponds to 1 or 2 byte size, respectively. */
        hsflctl.hsf_ctrl.fldbcount = size -1;
        hsflctl.hsf_ctrl.flcycle = ICH8_CYCLE_WRITE;
        BP_ADDRESSING_WRITE_ICH8_REG16(hw, ICH8_FLASH_HSFCTL, hsflctl.regval);

        /* Write the last 24 bits of index into Flash Linear address field in
         * Flash Address */
        BP_ADDRESSING_WRITE_ICH8_REG(hw, ICH8_FLASH_FADDR, flash_linear_address);

        if (size == 1)
            flash_data = (uint32_t)data & 0x00FF;
        else
            flash_data = (uint32_t)data;

        BP_ADDRESSING_WRITE_ICH8_REG(hw, ICH8_FLASH_FDATA0, flash_data);

        /* check if FCERR is set to 1 , if set to 1, clear it and try the whole
         * sequence a few more times else done */
        error = bp_addressing_ich8_flash_cycle(hw, ICH8_FLASH_COMMAND_TIMEOUT);
        if (error == BP_ADDRESSING_SUCCESS) {
            break;
        } else {
            /* If we're here, then things are most likely completely hosed,
             * but if the error condition is detected, it won't hurt to give
             * it another try...ICH8_FLASH_CYCLE_REPEAT_COUNT times.
             */
            hsfsts.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFSTS);
            if (hsfsts.hsf_status.flcerr == 1) {
                /* Repeat for some time before giving up. */
                continue;
            } else if (hsfsts.hsf_status.flcdone == 0) {
                DEBUGOUT("Timeout error - flash cycle did not complete.");
                break;
            }
        }
    } while (count++ < ICH8_FLASH_CYCLE_REPEAT_COUNT);

    return error;
}

/******************************************************************************
 * Reads a single byte from the NVM using the ICH8 flash access registers.
 *
 * hw - pointer to bp_addressing_hw structure
 * index - The index of the byte to read.
 * data - Pointer to a byte to store the value read.
 *****************************************************************************/
static int32_t
bp_addressing_read_ich8_byte(struct bp_addressing_hw *hw, uint32_t index, uint8_t* data)
{
    int32_t status = BP_ADDRESSING_SUCCESS;
    uint16_t word = 0;

    status = bp_addressing_read_ich8_data(hw, index, 1, &word);
    if (status == BP_ADDRESSING_SUCCESS) {
        *data = (uint8_t)word;
    }

    return status;
}

/******************************************************************************
 * Writes a single byte to the NVM using the ICH8 flash access registers.
 * Performs verification by reading back the value and then going through
 * a retry algorithm before giving up.
 *
 * hw - pointer to bp_addressing_hw structure
 * index - The index of the byte to write.
 * byte - The byte to write to the NVM.
 *****************************************************************************/
static int32_t
bp_addressing_verify_write_ich8_byte(struct bp_addressing_hw *hw, uint32_t index, uint8_t byte)
{
    int32_t error = BP_ADDRESSING_SUCCESS;
    int32_t program_retries;
    uint8_t temp_byte;

    bp_addressing_write_ich8_byte(hw, index, byte);
    udelay(100);

    for (program_retries = 0; program_retries < 100; program_retries++) {
        bp_addressing_read_ich8_byte(hw, index, &temp_byte);
        if (temp_byte == byte)
            break;
        udelay(10);
        bp_addressing_write_ich8_byte(hw, index, byte);
        udelay(100);
    }
    if (program_retries == 100)
        error = BP_ADDRESSING_ERR_EEPROM;

    return error;
}

/******************************************************************************
 * Writes a single byte to the NVM using the ICH8 flash access registers.
 *
 * hw - pointer to bp_addressing_hw structure
 * index - The index of the byte to read.
 * data - The byte to write to the NVM.
 *****************************************************************************/
static int32_t
bp_addressing_write_ich8_byte(struct bp_addressing_hw *hw, uint32_t index, uint8_t data)
{
    int32_t status = BP_ADDRESSING_SUCCESS;
    uint16_t word = (uint16_t)data;

    status = bp_addressing_write_ich8_data(hw, index, 1, word);

    return status;
}

/******************************************************************************
 * Reads a word from the NVM using the ICH8 flash access registers.
 *
 * hw - pointer to bp_addressing_hw structure
 * index - The starting byte index of the word to read.
 * data - Pointer to a word to store the value read.
 *****************************************************************************/
static int32_t
bp_addressing_read_ich8_word(struct bp_addressing_hw *hw, uint32_t index, uint16_t *data)
{
    int32_t status = BP_ADDRESSING_SUCCESS;
    status = bp_addressing_read_ich8_data(hw, index, 2, data);
    return status;
}

/******************************************************************************
 * Erases the bank specified. Each bank is a 4k block. Segments are 0 based.
 * segment N is 4096 * N + flash_reg_addr.
 *
 * hw - pointer to bp_addressing_hw structure
 * segment - 0 for first segment, 1 for second segment, etc.
 *****************************************************************************/
static int32_t
bp_addressing_erase_ich8_4k_segment(struct bp_addressing_hw *hw, uint32_t segment)
{
    union ich8_hws_flash_status hsfsts;
    union ich8_hws_flash_ctrl hsflctl;
    uint32_t flash_linear_address;
    int32_t  count = 0;
    int32_t  error = BP_ADDRESSING_ERR_EEPROM;
    int32_t  iteration, seg_size;
    int32_t  sector_size;
    int32_t  j = 0;
    int32_t  error_flag = 0;

    hsfsts.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFSTS);

    /* Determine HW Sector size: Read BERASE bits of Hw flash Status register */
    /* 00: The Hw sector is 256 bytes, hence we need to erase 16
     *     consecutive sectors.  The start index for the nth Hw sector can be
     *     calculated as = segment * 4096 + n * 256
     * 01: The Hw sector is 4K bytes, hence we need to erase 1 sector.
     *     The start index for the nth Hw sector can be calculated
     *     as = segment * 4096
     * 10: Error condition
     * 11: The Hw sector size is much bigger than the size asked to
     *     erase...error condition */
    if (hsfsts.hsf_status.berasesz == 0x0) {
        /* Hw sector size 256 */
        sector_size = seg_size = ICH8_FLASH_SEG_SIZE_256;
        iteration = ICH8_FLASH_SECTOR_SIZE / ICH8_FLASH_SEG_SIZE_256;
    } else if (hsfsts.hsf_status.berasesz == 0x1) {
        sector_size = seg_size = ICH8_FLASH_SEG_SIZE_4K;
        iteration = 1;
    } else if (hsfsts.hsf_status.berasesz == 0x3) {
        sector_size = seg_size = ICH8_FLASH_SEG_SIZE_64K;
        iteration = 1;
    } else {
        return error;
    }

    for (j = 0; j < iteration ; j++) {
        do {
            count++;
            /* Steps */
            error = bp_addressing_ich8_cycle_init(hw);
            if (error != BP_ADDRESSING_SUCCESS) {
                error_flag = 1;
                break;
            }

            /* Write a value 11 (block Erase) in Flash Cycle field in Hw flash
             * Control */
            hsflctl.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFCTL);
            hsflctl.hsf_ctrl.flcycle = ICH8_CYCLE_ERASE;
            BP_ADDRESSING_WRITE_ICH8_REG16(hw, ICH8_FLASH_HSFCTL, hsflctl.regval);

            /* Write the last 24 bits of an index within the block into Flash
             * Linear address field in Flash Address.  This probably needs to
             * be calculated here based off the on-chip segment size and the
             * software segment size assumed (4K) */
            /* TBD */
            flash_linear_address = segment * sector_size + j * seg_size;
            flash_linear_address &= ICH8_FLASH_LINEAR_ADDR_MASK;
            flash_linear_address += hw->flash_base_addr;

            BP_ADDRESSING_WRITE_ICH8_REG(hw, ICH8_FLASH_FADDR, flash_linear_address);

            error = bp_addressing_ich8_flash_cycle(hw, 1000000);
            /* Check if FCERR is set to 1.  If 1, clear it and try the whole
             * sequence a few more times else Done */
            if (error == BP_ADDRESSING_SUCCESS) {
                break;
            } else {
                hsfsts.regval = BP_ADDRESSING_READ_ICH8_REG16(hw, ICH8_FLASH_HSFSTS);
                if (hsfsts.hsf_status.flcerr == 1) {
                    /* repeat for some time before giving up */
                    continue;
                } else if (hsfsts.hsf_status.flcdone == 0) {
                    error_flag = 1;
                    break;
                }
            }
        } while ((count < ICH8_FLASH_CYCLE_REPEAT_COUNT) && !error_flag);
        if (error_flag == 1)
            break;
    }
    if (error_flag != 1)
        error = BP_ADDRESSING_SUCCESS;
    return error;
}






