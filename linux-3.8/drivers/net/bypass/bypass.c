#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/mc146818rtc.h>
#include <linux/hardirq.h> 
#include <linux/pci.h>
#include <linux/sched.h>

#include <future/nfw.h>
#include <future/bypass.h>

#include "fw7551_bypass.h"

#define MAX_BYPASS_NUM		24
#define MAX_CAR_BYPASS_NUM	12
#define LANNER_MODEL_NUM	8

void caswell_bypass_init(int port);
void caswell_bypass_enable(int port);
void caswell_bypass_disable(int port);
void caswell_bypass_wd_reset(int port);
void caswell_bypass_wd_timeout(int port, int timeout);

void caswell_gen3_bypass_init(int port);
void caswell_gen3_bypass_enable(int port);
void caswell_gen3_bypass_disable(int port);
void caswell_gen3_bypass_wd_reset(int port);
void caswell_gen3_bypass_wd_timeout(int port, int timeout);

int caswell_get_bypass_device_count(void);

void lanner_bypass_init(int port);
void lanner_bypass_enable(int port);
void lanner_bypass_disable(int port);
void lanner_bypass_wd_reset(int port);
void lanner_bypass_wd_timeout(int port, int timeout);
int lanner_bypass_scan_bus(void);

static int num_of_bypass_device = -1;
static int is_caswell_bypass;
static int is_lanner_bypass;

int bypass_map[MAX_BYPASS_NUM] = {0, 1, 2, 3, 4, 5};
int zen_bypass_mode;
int bypass_wathdog_test;
int g_wd_timeout = 5;
/*
 * select_bypass_driver
 * value 0 : No bypass
 * value 1 : Caswell GEN2 bypass driver
 * value 2 : Caswell GEN3 bypass driver
 */
int select_bypass_driver[MAX_CAR_BYPASS_NUM] = {0};
static atomic_t run_refresh;
extern char bootserial[25];
extern int g_bypass_show;

struct _zen_bypass{
	int port;
	int mode;
	struct _f{
		void (*init)(int port);
		void (*enable)(int port);
		void (*disable)(int port);
		void (*wd_reset)(int port);
		void (*wd_timeout)(int port, int timeout);
	}f[MAX_BYPASS_NUM];
};

static struct _zen_bypass zen_bypass;

static const struct _zen_bypass_info zen_bypass_table[] = {
	{ "WZN301A", 	"MB-7583", 3, 1 },
	{ "WZN301S", 	"MB-7583", 3, 1 },
	{ "WZN401A",    "MB-887X", 3, 4 },
	{ "WZN501A", "MB-8771EXT", 3, 2 },
	{ "WZN701A",    "MB-887X", 0, 4 },
	{ "WZN801A",    "MB-887X", 0, 4 },
	{ "WZN102A",    "MB-8895", 0, 8 },
	{ "WZN202A",    "MB-8895", 0, 8 },
	{ "", "", 0, 0 }
};

struct _zen_bypass_info zen_bypass_info;
static char msg1[100];

void bypass_wd_refresh(void)
{
	int port;

	if(!zen_bypass_mode)
		return;

	while( atomic_inc_return(&run_refresh) > 1 ){
		atomic_dec(&run_refresh);
		if(!in_interrupt()){
			schedule();
			continue;
		}
		return;
	}

	if(is_lanner_bypass) {
		if(zen_bypass.f[0].wd_reset)
			zen_bypass.f[0].wd_reset(0);
	}
	else {
		for(port=0; port < zen_bypass.port; port++){
			if(zen_bypass.mode&(0x01<<port)){
				if(zen_bypass.f[port].wd_reset)
					zen_bypass.f[port].wd_reset(port);
			}
		}
	}
	atomic_dec(&run_refresh);
}



int zen_thread_bypass(void *dummy)
{
	int port;
	static int run_thread;

	if(run_thread)
		return 0;
	run_thread = 1;

	while(zen_bypass_mode){
		if(bypass_wathdog_test)
			break;

		if( atomic_inc_return(&run_refresh) == 1 ){
			if(is_lanner_bypass) {
				if(zen_bypass.f[0].wd_reset)
					zen_bypass.f[0].wd_reset(0);
			}
			else {
				for(port=0; port < zen_bypass.port; port++){
					if(zen_bypass.mode&(0x01<<port)){
						if(zen_bypass.f[port].wd_reset)
							zen_bypass.f[port].wd_reset(port);
					}
				}
			}
		}
		atomic_dec(&run_refresh);
		msleep(1000);
	}

	run_thread = 0;
	return 0;
}

void bypass_enable(void)
{
	int port, mode;

	zen_bypass_mode=0;
	mode = zen_bypass.mode;

	if(is_lanner_bypass) {
		if(zen_bypass.f[0].enable)
			zen_bypass.f[0].enable(mode);
	}
	else {
		for(port=0; port<zen_bypass.port; port++){
			if(zen_bypass.mode&(0x01<<port)){
				if(zen_bypass.f[port].enable)
					zen_bypass.f[port].enable(port);
			}
		}
	}
}

static int bypass_proc_read(char *page, char **start,off_t off, int count, int *eof,void *data)
{
	return sprintf(page, "mode : %d, port : %d, show : %s\n%s", zen_bypass.mode, zen_bypass.port, g_bypass_show ? "on":"off",msg1);
}

static int bypass_proc_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
	char tmp[20]={0};
	int mode = 0, port;
	int ff=0,i;
	char msg2[50]={'\0'};

	/* Get the number of bypass pair */
	if (is_caswell_bypass && num_of_bypass_device < 0) {
		int i;
		num_of_bypass_device = caswell_get_bypass_device_count();
		zen_bypass.port = num_of_bypass_device;
		for(i=0;i<zen_bypass.port;i++) {
			if(!select_bypass_driver[i])
				continue;

			if(select_bypass_driver[i] == 1) {
				/* Use caswell GEN3 bypass driver */
				zen_bypass.f[i].init 		= caswell_bypass_init;
				zen_bypass.f[i].enable 		= caswell_bypass_enable;
				zen_bypass.f[i].disable 	= caswell_bypass_disable;
				zen_bypass.f[i].wd_reset 	= caswell_bypass_wd_reset;
				zen_bypass.f[i].wd_timeout 	= caswell_bypass_wd_timeout;
			}
			else if(select_bypass_driver[i] == 2) {
				/* Use caswell GEN2 bypass driver */
				zen_bypass.f[i].init 		= caswell_gen3_bypass_init;
				zen_bypass.f[i].enable 		= caswell_gen3_bypass_enable;
				zen_bypass.f[i].disable 	= caswell_gen3_bypass_disable;
				zen_bypass.f[i].wd_reset 	= caswell_gen3_bypass_wd_reset;
				zen_bypass.f[i].wd_timeout 	= caswell_gen3_bypass_wd_timeout;
			}
		}
	}
	else if (is_lanner_bypass && num_of_bypass_device < 0) {
		num_of_bypass_device = lanner_bypass_scan_bus();
		zen_bypass.port = num_of_bypass_device;
	}

	if (is_caswell_bypass && !num_of_bypass_device) {
		zen_bypass.port = 0;
		return count;
	}
	else if (is_lanner_bypass && !num_of_bypass_device) {
		zen_bypass.port = 0;
		return count;
	}

	copy_from_user(tmp, buffer, count);
	if(count==0 || count>3){
		char name[20];
		int  flag;
		sscanf(tmp, "%s %d\n", name, &flag);
		if(!strncmp(name, "wd", 2)){
			if((flag>0) && (flag<30)){
				g_wd_timeout = flag;
				if(is_lanner_bypass) {
					if(zen_bypass.f[0].wd_timeout)
						zen_bypass.f[0].wd_timeout(mode, g_wd_timeout);
				}
				else {
					for(port=0; port<zen_bypass.port; port++){
						if(zen_bypass.f[port].wd_timeout)
							zen_bypass.f[port].wd_timeout(port, g_wd_timeout);
					}
				}
			}
			else
				printk("watchdog timeout : 1 ~ 30 second\n");
			return count;
		}
		else if(!strncmp(name, "test", 4)){
			if(flag == 1)
				bypass_enable();
			else if(flag == 2){
				char *tmp=NULL;
				tmp[0x1234] = 0x56;
				while(1);
			}
			else if(flag == 3){
				local_irq_disable();
				while(1);
			}
			else if(flag == 4)
				bypass_wathdog_test = bypass_wathdog_test? 0 : 1;
			else if(flag == 5)
				bypass_wd_refresh();
			return count;
		}
	}

	mode = simple_strtoul(tmp, NULL, 10);
	zen_bypass_mode =  mode? 1 : 0;

	if((mode<0) || (mode>=(0x01<<zen_bypass.port)))
		return count;

	zen_bypass.mode = mode;


	if(is_lanner_bypass) {
		if(zen_bypass.f[0].init)
			zen_bypass.f[0].init(mode);
	}
	else {
		for(port=0; port<zen_bypass.port; port++){
			if(mode&(0x01<<port)){
				if(zen_bypass.f[port].init)
					zen_bypass.f[port].init(port);
			}
			else{
				if(zen_bypass.f[port].disable)
					zen_bypass.f[port].disable(port);
			}
		}
	}
	msg1[0]='\0';
	for (i=0; i< zen_bypass.port; i++)
	{
		sprintf(msg2,"bp%d: eth%d eth%d\n",i+1,zen_bypass_info.allbp[ff],zen_bypass_info.allbp[ff+1]);
		strcat(msg1,msg2);
		ff+=2;
	}
	return count;
}

int add_bypass_device(int dev)
{
	if(zen_bypass.port > MAX_BYPASS_NUM)
		return -1;
	if(dev == 1){
		zen_bypass.f[zen_bypass.port].init			= caswell_bypass_init;
		zen_bypass.f[zen_bypass.port].enable		= caswell_bypass_enable;
		zen_bypass.f[zen_bypass.port].disable		= caswell_bypass_disable;
		zen_bypass.f[zen_bypass.port].wd_reset		= caswell_bypass_wd_reset;
		zen_bypass.f[zen_bypass.port].wd_timeout	= caswell_bypass_wd_timeout;
		zen_bypass.port += 1;
		return (zen_bypass.port - 1);
	}
	else if (dev == 2) {
		zen_bypass.f[0].init       = lanner_bypass_init;
		zen_bypass.f[0].enable     = lanner_bypass_enable;
		zen_bypass.f[0].disable    = lanner_bypass_disable;
		zen_bypass.f[0].wd_reset   = lanner_bypass_wd_reset;
		zen_bypass.f[0].wd_timeout = lanner_bypass_wd_timeout;
		zen_bypass.port += 1;
	}
	else if (dev == 3) {
		zen_bypass.f[zen_bypass.port].init		= zen_7551_init;
		zen_bypass.f[zen_bypass.port].enable	= zen_7551_enable;
		zen_bypass.f[zen_bypass.port].disable	= zen_7551_disable;

		zen_bypass.port += 1;
	}

	return -1;
}

extern struct proc_dir_entry *proc_ferret_system;

static int bypass_proc_init(void)
{
	struct proc_dir_entry *p = NULL;
	int i;

	p = create_proc_entry("bypass", 0, proc_ferret_network);
	if (p == NULL) {
		printk("Creating proc entry failed: /proc/ferret/network/bypass\n");
		return -ENOMEM;
	}

	p->read_proc = bypass_proc_read;
	p->write_proc = bypass_proc_write;

	if(!strncmp(bootserial, "WZN201A", 7) ||
			!strncmp(bootserial, "WZN301R", 7) ||	// Caswell ZEN3000R
			!strncmp(bootserial, "WZN601A", 7)) {
		is_caswell_bypass = 1;
		zen_bypass.port = MAX_CAR_BYPASS_NUM;
	}
	else if(!strncmp(bootserial, "WZN101A", 7)) {
		if(!w83627chip(W83627_CTL_REG)) {
			init_7551_gpio();
			add_bypass_device(3);
		}
	}
	else {
		for(i=0;i<LANNER_MODEL_NUM+1;i++) {
			if(!strncmp(zen_bypass_table[i].serial, bootserial, 7)) {
				zen_bypass_info = zen_bypass_table[i];
				is_lanner_bypass = 1;
				zen_bypass.port = MAX_BYPASS_NUM;
				add_bypass_device(2);
				break;
			}
		}
	}
	return 0;
}

module_init(bypass_proc_init);
