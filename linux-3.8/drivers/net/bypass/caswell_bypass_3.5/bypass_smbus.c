
#include <linux/slab.h>
#include <linux/i2c.h>

#include <linux/version.h>
#include <linux/pci.h>
#include "bypass_kversion.h"
#include "bypass_slot.h"
#include "bypass_pci.h"
#include "bypass_i2cmux.h"
#include "bypass_smbus.h"

static struct i2c_adapter *adapter;

static struct i2c_client *clients[PCI_ADDCARD_MAX_SLOTS];

struct i2c_client *bypass_get_i2c_client_by_idx_NAR5520(u8 slot, u8 index){
	if (index == 2)
		return NULL;
	clients[slot]->addr = client_addrs_NAR5520[index];

	return clients[slot];
}

struct i2c_client *bypass_get_i2c_client_by_idx(u8 slot, u8 index){
	clients[slot]->addr = client_addrs[slot % 4][index];	
	return clients[slot];
}

struct i2c_client *bypass_get_i2c_client_by_addr(u8 slot, u8 addr){
	clients[slot]->addr = addr;	
	return clients[slot];
}

int bypass_smbus_client_create(u8 *slot){

	clients[*slot] = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);
	clients[*slot]->adapter = adapter;
	clients[*slot]->dev.platform_data = slot;
	clients[*slot]->flags = 0;
	
	return 0;
}

int bypass_smbus_init(){
	int i;
	struct i2c_adapter *tmp;

	for (i = 0; i <= 0xFF; i++){
		tmp = i2c_get_adapter(i);

		if (tmp != NULL){
			if (!strncmp("SMBus I801", tmp->name, 10))
				break;
		}
	}

	if (i == 0x100)
		return -ENODEV;

	adapter = i2c_get_adapter(i);

	return 0;
}
