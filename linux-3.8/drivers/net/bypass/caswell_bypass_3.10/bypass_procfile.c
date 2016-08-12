/*
    pca9555_bypass.c - 16-bit I/O port with interrupt and reset
    with special microcon connected. For net port bypass controller
    use. The driver is modified from pca9539.c

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
*/
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/i2c-proc.h>
#include <linux/init.h>
#include <linux/delay.h>
#include "bypass_def.h"
#define __INCLUDE_PLATEFORM_DATA__
#include "bypass_platform.h"
#include "bypass_control.h"
#include "bypass_action.h"

extern  bp_interface bp_int[NET_INTERFACE_MAX];

#define MAX_SYSTEL_ID 10
static unsigned int sysctl_id[MAX_SYSTEL_ID];
static unsigned int sysctl_num=0;

/* -- SENSORS SYSCTL START -- */
#define PCA9555_SYSCTL_INPUT0   1001
#define PCA9555_SYSCTL_INPUT1   1002
#define PCA9555_SYSCTL_OUTPUT0  1003
#define PCA9555_SYSCTL_OUTPUT1  1004
#define PCA9555_SYSCTL_INVERT0  1005
#define PCA9555_SYSCTL_INVERT1  1006
#define PCA9555_SYSCTL_DIRECTION0       1007
#define PCA9555_SYSCTL_DIRECTION1       1008
#define PCA9555_SYSCTL_BYPASS0       1009
#define PCA9555_SYSCTL_BYPASS1       1010
#define PCA9555_SYSCTL_NEXTBOOT0       1011
#define PCA9555_SYSCTL_NEXTBOOT1       1012
#define PCA9555_SYSCTL_BPE0       1013
#define PCA9555_SYSCTL_BPE1       1014
#define PCA9555_SYSCTL_PERIOD0       1015
#define PCA9555_SYSCTL_PERIOD1       1016
#define PCA9555_SYSCTL_WDT_TIMEOUT0       1017
#define PCA9555_SYSCTL_WDT_TIMEOUT1       1018
#define PCA9555_SYSCTL_WDT0      1019
#define PCA9555_SYSCTL_WDT1       1020
#define PCA9555_SYSCTL_BP_TYPE       1021
#define PCA9555_SYSCTL_AUTO_TRIGGER0    1022
#define PCA9555_SYSCTL_AUTO_TRIGGER1    1023
#define PCA9555_SYSCTL_FIRMWARE0    1024
#define PCA9555_SYSCTL_FIRMWARE1    1025
/* -- SENSORS SYSCTL END -- */

/* following are the sysfs callback functions */
void pca9555_read(struct i2c_client *client, int operation,
			int ctl_name, int *nrels_mag, long *results)
{
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		if(ctl_name==PCA9555_SYSCTL_INPUT0){
			data->in0 = i2c_smbus_read_byte_data(client, PCA9555_INPUT_0);       
			results[0]=data->in0;
		}else if(ctl_name==PCA9555_SYSCTL_INPUT1){
			data->in1 = i2c_smbus_read_byte_data(client, PCA9555_INPUT_1);
			results[0]=data->in1;
		}
		*nrels_mag = 1;
	}  else if (operation == SENSORS_PROC_REAL_WRITE) {
#ifdef DEBUG
	printk("pca9555.o: Attempted write to read only sysctl!");
#endif
	}
}
void pca9555_write(struct i2c_client *client, int operation,
						int ctl_name, int *nrels_mag, long *results)
{
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		switch (ctl_name){
		case PCA9555_SYSCTL_OUTPUT0:
			results[0]=data->out0;
		break;
		case PCA9555_SYSCTL_OUTPUT1:
			results[0]=data->out1;
		break;
		case PCA9555_SYSCTL_DIRECTION0:
			results[0]=data->conf0;
		break;
		case PCA9555_SYSCTL_DIRECTION1:
			results[0]=data->conf1;
		break;
		case PCA9555_SYSCTL_INVERT0:
			results[0]=data->inv0;
		break;
		case PCA9555_SYSCTL_INVERT1:
			results[0]=data->inv1;
		break;
		}
		*nrels_mag = 1;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag == 1) {
		switch (ctl_name){
		case PCA9555_SYSCTL_OUTPUT0:
			data->out0=(results[0]) & 0xFF;
			i2c_smbus_write_byte_data(client, PCA9555_OUTPUT_0, data->out0);
		break;
		case PCA9555_SYSCTL_OUTPUT1:
			data->out1=(results[0]) & 0xFF;
			i2c_smbus_write_byte_data(client, PCA9555_OUTPUT_1, data->out1);
		break;
		case PCA9555_SYSCTL_DIRECTION0:
			data->conf0=(results[0]) & 0xFF;
			i2c_smbus_write_byte_data(client, PCA9555_DIRECTION_0, data->conf0);
		break;
		case PCA9555_SYSCTL_DIRECTION1:
			data->conf1=(results[0]) & 0xFF;
			i2c_smbus_write_byte_data(client, PCA9555_DIRECTION_1, data->conf1);
		break;
		case PCA9555_SYSCTL_INVERT0:
			data->inv0=(results[0]) & 0xFF;
			i2c_smbus_write_byte_data(client, PCA9555_INVERT_0, data->inv0);
		break;
		case PCA9555_SYSCTL_INVERT1:
			data->inv1=(results[0]) & 0xFF;
			i2c_smbus_write_byte_data(client, PCA9555_INVERT_1, data->inv1);
		break;
		}
	}
	}
}
                            


/* Define the device attributes */
static ctl_table pca9555_defattr_group[] = {
	{PCA9555_SYSCTL_INPUT0, "input0", NULL, 0, 0444, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &pca9555_read},
	{PCA9555_SYSCTL_INPUT1, "input1", NULL, 0, 0444, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &pca9555_read},
	{PCA9555_SYSCTL_OUTPUT0, "output0", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &pca9555_write},
	{PCA9555_SYSCTL_OUTPUT1, "output1", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &pca9555_write},
	{PCA9555_SYSCTL_DIRECTION0, "direction0", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &pca9555_write},
	{PCA9555_SYSCTL_DIRECTION1, "direction1", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &pca9555_write},
	{PCA9555_SYSCTL_INVERT0, "invert0", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &pca9555_write},
	{PCA9555_SYSCTL_INVERT1, "invert1",NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &pca9555_write},
	{0}
};

void bypass_control(struct i2c_client *client, int operation,
						int ctl_name, int *nrels_mag, long *results)
{
	int ret=0, bpe=0;
	bp_seg *bp_seg;
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		switch (ctl_name){
		case PCA9555_SYSCTL_BYPASS0:
			results[0]=bypass_show(client,BP_SEGA);
		break;
		case PCA9555_SYSCTL_BYPASS1:
			results[0]=bypass_show(client,BP_SEGB);                        
		break;
		}
	*nrels_mag = 1;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag == 1) {
		switch (ctl_name){
		case PCA9555_SYSCTL_BYPASS0:
			bypass_store(client,BP_SEGA,(results[0]) & 0xFF);
		break;
		case PCA9555_SYSCTL_BYPASS1:
			bypass_store(client,BP_SEGB,(results[0]) & 0xFF);                            
		break;
		}
	}
	}
}
int  bypass_show(struct i2c_client *client, int index)
{
	return bypass_get_status(client, index);
}

int bypass_store(struct i2c_client *client, int index,int buf)
{
	return bypass_set_status(client, index, buf);
}


int nextboot_show(struct i2c_client *client, int index)
{
	return  bypass_get_nextboot(client, index);
}

int nextboot_store(struct i2c_client *client, int index,int buf)
{
	return bypass_set_nextboot(client, index, buf);
}

void nextboot_control(struct i2c_client *client, int operation,
                    int ctl_name, int *nrels_mag, long *results)
{
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		switch (ctl_name){
		case PCA9555_SYSCTL_NEXTBOOT0:
			results[0]=nextboot_show(client,BP_SEGA);
		break;
		case PCA9555_SYSCTL_NEXTBOOT1:
			results[0]=nextboot_show(client,BP_SEGB);                        
		break;
		}
		*nrels_mag = 1;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag == 1) {
			switch (ctl_name){
			case PCA9555_SYSCTL_NEXTBOOT0:
				nextboot_store(client,BP_SEGA,(results[0]) & 0xFF);
			break;
			case PCA9555_SYSCTL_NEXTBOOT1:
				nextboot_store(client,BP_SEGB,(results[0]) & 0xFF);                            
			break;
		}
	}
	}
}

int bpe_show(struct i2c_client *client, int index)
{
	return bypass_get_bpe(client, index);
}

int bpe_store(struct i2c_client *client, int index,int buf)
{

	bypass_set_bpe(client, index, buf);

	return 0;
}

void bpe_control(struct i2c_client *client, int operation,
                    int ctl_name, int *nrels_mag, long *results)
{
	int ret=0, bpe=0;
	bp_seg *bp_seg;
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		switch (ctl_name){
		case PCA9555_SYSCTL_BPE0:
			results[0]=bpe_show(client,BP_SEGA);
		break;
		case PCA9555_SYSCTL_BPE1:
			results[0]=bpe_show(client,BP_SEGB);                        
		break;
		}
		*nrels_mag = 1;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag == 1) {
			switch (ctl_name){
			case PCA9555_SYSCTL_BPE0:
				bpe_store(client,BP_SEGA,(results[0]) & 0xFF);
			break;
			case PCA9555_SYSCTL_BPE1:
				bpe_store(client,BP_SEGB,(results[0]) & 0xFF);                            
			break;
		}
	}
	}
}

int wdt_timeout_show(struct i2c_client *client, int index)
{
	return bypass_get_timeout_bit (client, index);
}

int wdt_timeout_read(struct i2c_client *client, int operation,
                    int ctl_name, int *nrels_mag, long *results)
{
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		if(ctl_name==PCA9555_SYSCTL_WDT_TIMEOUT0){
			results[0]=wdt_timeout_show(client,BP_SEGA);
		}else if(ctl_name==PCA9555_SYSCTL_WDT_TIMEOUT1){
			results[0]=wdt_timeout_show(client,BP_SEGB);
		}
		*nrels_mag = 1;
	}  else if (operation == SENSORS_PROC_REAL_WRITE) {
#ifdef DEBUG
		printk("pca9555.o: Attempted write to read only sysctl!");
#endif
	}
}

int wdt_show(struct i2c_client *client, int index)
{
	return  bypass_get_wdt(client, index);
}

int wdt_store(struct i2c_client *client, int index,int buf)
{
	return bypass_set_wdt (client, index, buf);
}

void wdt_control(struct i2c_client *client, int operation,
                    int ctl_name, int *nrels_mag, long *results)
{
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		switch (ctl_name){
			case PCA9555_SYSCTL_WDT0:
				results[0]=wdt_show(client,BP_SEGA);
			break;
			case PCA9555_SYSCTL_WDT1:
				results[0]=wdt_show(client,BP_SEGB);                        
			break;
		}
		*nrels_mag = 1;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag == 1) {
			switch (ctl_name){
			case PCA9555_SYSCTL_WDT0:
					wdt_store(client,BP_SEGA,(results[0]) & 0xFF);
			break;
			case PCA9555_SYSCTL_WDT1:
				wdt_store(client,BP_SEGB,(results[0]) & 0xFF);                            
			break;
			}
		}
	}
}

int period_show(struct i2c_client *client, int index)
{
	return bypass_get_period(client, index);
}

int period_store(struct i2c_client *client, int index, int buf)
{
	return bypass_set_period(client, index, buf);
}

void period_control(struct i2c_client *client, int operation,
                    int ctl_name, int *nrels_mag, long *results)
{
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		switch (ctl_name){
		case PCA9555_SYSCTL_PERIOD0:
			results[0]=period_show(client,BP_SEGA);
		break;
		case PCA9555_SYSCTL_PERIOD1:
			results[0]=period_show(client,BP_SEGB);                        
		break;
		}
		*nrels_mag = 1;
	} else if (operation == SENSORS_PROC_REAL_WRITE) {
		if (*nrels_mag == 1) {
			switch (ctl_name){
			case PCA9555_SYSCTL_PERIOD0:
				period_store(client,BP_SEGA,(results[0]) & 0xFF);
			break;
			case PCA9555_SYSCTL_PERIOD1:
				period_store(client,BP_SEGB,(results[0]) & 0xFF);                            
			break;
			}
		}
	}
}

int bp_type_show(struct i2c_client *client, int index)
{
	bp_interface *bp_interface;
	if (find_client_bp_interface(&bp_interface, client, index))
		return -ENXIO;
	printk("%s\n", bp_interface->modelname);
	return 0;
}

int bp_type_read(struct i2c_client *client, int operation,
				int ctl_name, int *nrels_mag, long *results)
{
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		if(ctl_name==PCA9555_SYSCTL_BP_TYPE){
			bp_type_show(client,BP_SEGA);
			results[0]=0;
		}
		*nrels_mag = 1;
	}  else if (operation == SENSORS_PROC_REAL_WRITE) {
#ifdef DEBUG
		printk("pca9555.o: Attempted write to read only sysctl!");
#endif
	}
}

int auto_trigger_show (struct i2c_client *client, int index)
{
	return bypass_get_auto_trigger(client, index);
}

int auto_trigger_store (struct i2c_client *client, int index, int buf)
{
	return bypass_set_auto_trigger(client, index, buf);
}

void auto_trigger_control (struct i2c_client *client, int operation,
                    int ctl_name, int *nrels_mag, long *results)
{
	int index;
	if (operation == SENSORS_PROC_REAL_INFO){
                *nrels_mag = 0;
		return;
	}

	switch (ctl_name){
                case PCA9555_SYSCTL_AUTO_TRIGGER0:
                        index = BP_SEGA;
                break;
                case PCA9555_SYSCTL_AUTO_TRIGGER1:
                        index = BP_SEGB;
                break;
        }

        if (operation == SENSORS_PROC_REAL_READ) {
                *nrels_mag = 1;
		results[0] = auto_trigger_show(client, index);
	}else if (operation == SENSORS_PROC_REAL_WRITE){
		auto_trigger_store(client, index, results[0]);
	}
}

int fw_ver_show(struct i2c_client *client, int index)
{
	return bypass_get_fw_ver (client, index);
}

int fw_ver_read(struct i2c_client *client, int operation,
                    int ctl_name, int *nrels_mag, long *results)
{
	struct ca_bypass_data *data = client->data;
	if (operation == SENSORS_PROC_REAL_INFO)
		*nrels_mag = 0;
	else if (operation == SENSORS_PROC_REAL_READ) {
		if(ctl_name==PCA9555_SYSCTL_FIRMWARE0){
			results[0]=fw_ver_show(client,BP_SEGA);
		}else if(ctl_name==PCA9555_SYSCTL_FIRMWARE1){
			results[0]=fw_ver_show(client,BP_SEGB);
		}
		*nrels_mag = 1;
	}  else if (operation == SENSORS_PROC_REAL_WRITE) {
#ifdef DEBUG
		printk("pca9555.o: Attempted write to read only sysctl!");
#endif
	}
}

/* Define the device attributes */
static ctl_table pca9555_bypass0_group[] = {
	{PCA9555_SYSCTL_BYPASS0, "bypass0", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &bypass_control},
	{PCA9555_SYSCTL_BP_TYPE, "bp_type", NULL, 0, 0444, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &bp_type_read},   
	{PCA9555_SYSCTL_NEXTBOOT0, "nextboot0", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &nextboot_control},  
	{PCA9555_SYSCTL_BPE0, "bpe0", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &bpe_control},     
	{PCA9555_SYSCTL_PERIOD0, "period0", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &period_control},    
	{PCA9555_SYSCTL_WDT_TIMEOUT0, "timeout0", NULL, 0, 0444, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &wdt_timeout_read},  
	{PCA9555_SYSCTL_WDT0, "wdt0", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &wdt_control},             
	{PCA9555_SYSCTL_AUTO_TRIGGER0, "auto_trigger0", NULL, 0, 0644, NULL, &i2c_proc_real,
        &i2c_sysctl_real, NULL, &auto_trigger_control},
	{PCA9555_SYSCTL_FIRMWARE0, "firmware0", NULL, 0, 0444, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &fw_ver_read},  
	{0}
};

static ctl_table pca9555_bypass1_group[] = {
	{PCA9555_SYSCTL_BYPASS1, "bypass1", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &bypass_control},
	{PCA9555_SYSCTL_BP_TYPE, "bp_type", NULL, 0, 0444, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &bp_type_read},
	{PCA9555_SYSCTL_NEXTBOOT1, "nextboot1", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &nextboot_control},  
	{PCA9555_SYSCTL_BPE1, "bpe1", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &bpe_control},   
	{PCA9555_SYSCTL_PERIOD1, "period1", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &period_control},
	{PCA9555_SYSCTL_WDT_TIMEOUT1, "timeout1", NULL, 0, 0444, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &wdt_timeout_read},
	{PCA9555_SYSCTL_WDT1, "wdt1", NULL, 0, 0644, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &wdt_control},
	{PCA9555_SYSCTL_AUTO_TRIGGER1, "auto_trigger1", NULL, 0, 0644, NULL, &i2c_proc_real,
        &i2c_sysctl_real, NULL, &auto_trigger_control},
	{PCA9555_SYSCTL_FIRMWARE1, "firmware1", NULL, 0, 0444, NULL, &i2c_proc_real,
	&i2c_sysctl_real, NULL, &fw_ver_read},  
	{0}
};

int bypass_unsetup_fs(struct i2c_client *client) {
	int icnt;
	for(icnt=0;icnt<sysctl_num;icnt++)
		i2c_deregister_entry(sysctl_id[icnt]);
	return 0;
}
int bypass_setup_fs(struct i2c_client *client, int type) {
	int ret = 0;
	/* Register a new directory entry with module sensors */
	if (type==DEFATTR){
		if ((ret = i2c_register_entry(client, "pca9555",pca9555_defattr_group, THIS_MODULE)) < 0)    
			return 1; 
	}else if  (type==BYPASS0ATTR){
		if ((ret = i2c_register_entry(client, "pca9555",pca9555_bypass0_group, THIS_MODULE)) < 0)    
			return 1;
	}else if  (type==BYPASS1ATTR){
		if ((ret = i2c_register_entry(client, "pca9555",pca9555_bypass1_group, THIS_MODULE)) < 0)    
			return 1;
	}
	sysctl_id[sysctl_num]=ret;
	sysctl_num++;    
	return 0;
}


