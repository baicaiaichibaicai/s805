#ifndef _BYPASS_ACTION_H_
#define _BYPASS_ACTION_H_

int bypass_get_status(struct i2c_client* client, int index);
int bypass_set_status(struct i2c_client* client, int index, int cmd_val);
int bypass_get_nextboot (struct i2c_client* client, int index);
int bypass_set_nextboot (struct i2c_client* client, int index, int cmd_val);
int bypass_get_bpe (struct i2c_client* client, int index);
int bypass_set_bpe (struct i2c_client* client, int index, int cmd_val);
int bypass_get_timeout_bit (struct i2c_client* client, int index);
int bypass_get_wdt (struct i2c_client* client, int index);
int bypass_set_wdt (struct i2c_client* client, int index, int cmd_val);
int bypass_get_period (struct i2c_client* client, int index);
int bypass_set_period (struct i2c_client* client, int index, int cmd_val);
int bypass_get_fw_ver (struct i2c_client* client, int index);

int bypass_get_auto_trigger(struct i2c_client* client, int index);
int bypass_set_auto_trigger(struct i2c_client* client, int index, int cmd_val);

int bypass_get_txdisable(struct i2c_client* client, int index);
int bypass_set_txdisable(struct i2c_client* client, int index, int cmd_val);

#endif
