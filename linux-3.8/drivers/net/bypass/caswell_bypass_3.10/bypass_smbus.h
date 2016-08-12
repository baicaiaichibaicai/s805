//#include "bypass_slot.h"

#define MAX_CLIENT_ADDR_PER_SLOT 3

const static uint8_t client_addrs[][MAX_CLIENT_ADDR_PER_SLOT] = {{0x20, 0x24, 0x74}, {0x21, 0x25, 0x75}, {0x22, 0x26, 0x76}, {0x23, 0x27, 0x77}};

const static uint8_t client_addrs_NAR5520[MAX_CLIENT_ADDR_PER_SLOT] = {0x21, 0x26};

struct i2c_client *bypass_get_i2c_client_by_idx_NAR5520(u8 slot, u8 index);
struct i2c_client *bypass_get_i2c_client_by_idx(u8 slot, u8 index);
struct i2c_client *bypass_get_i2c_client_by_addr(u8 slot, u8 addr);
int bypass_smbus_client_create(u8 *slot);
int bypass_smbus_init(void);
