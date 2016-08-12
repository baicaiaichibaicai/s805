#pragma once

void mv6176_phy_read(struct phy_device *phydev, ushort port, ushort reg, int *data);
void mv6176_phy_write(struct phy_device *phydev, ushort port, ushort reg, ushort data);

typedef union mv_smix_cmd {
	unsigned short u16;
	struct mv_smix_cmd_s {
#ifdef __BIG_ENDIAN_BITFIELD
		unsigned short busy             : 1;
		unsigned short reserved_13_14   : 2;
		unsigned short mode             : 1;    /* 0 : Clause22, 1:45 */
		unsigned short opcode           : 2;
		unsigned short devaddr          : 5;
		unsigned short regaddr          : 5;
#else
		unsigned short regaddr          : 5;
		unsigned short devaddr          : 5;
		unsigned short opcode           : 2;
		unsigned short mode             : 1;    /* 0 : Clause22, 1:45 */
		unsigned short reserved_13_14   : 2;
		unsigned short busy             : 1;
#endif
	} s;
};
typedef union mv_smix_cmd mv_smix_cmd_t;

#define MARVELL_PHY_ID_88E6176 	0xff001761
/* SWITCH GLOBAL REGISTER */
#define SWC_GLOBAL_ADDR_1	0x1B
#define SWC_GLOBAL_ADDR_2	0x1C
#define SWC_GLOBAL_ADDR_3	0x1D

#define GLOBAL_STATUS_REG	0x00
#define SMI_PHY_CMD_REG		0x18
#define SMI_PHY_DATA_REG	0x19

#define MV6176_SMI_BUSY_BIT		0x8000
#define MV6176_MAX_SMI_ADDR		0x1f
#define MV6176_MGMT_PORT		0x15
