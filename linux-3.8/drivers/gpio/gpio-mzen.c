//#define	DEBUG
//#define	ENABLE_GPIO_INIT
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>

#include "gpio-mzen.h"

#define	GPIOBASE	(0x500)

#define GPIO_USESEL_OFFSET(x)   (x)
#define GPIO_IOSEL_OFFSET(x)    (x + 4)
#define GPIO_LVL_OFFSET(x)      (x + 8)

const static char *cmdstr_table[] = {
	"MGPIO_GLED", 
	"MGPIO_RLED",
	"MGPIO_TAMPER",
	"MGPIO_TEMPERATURE",
	"MGPIO_CLED0",
	"MGPIO_CLED1",
	"MGPIO_BUZZER",

	"CGPIO_RLED",
	"CGPIO_GLED",
	"CGPIO_BAT",
	"CGPIO_BUZZER",

	"CGPIO_BLUE", /*test purpose on minnowboard turbot*/
	NULL
};

#define FNAME	"mzen_gpio_test"	
#define	BUFSZ	(128)
static char cmdstr[32];
static int cmdval;

#define	DIR_IN	(0)
#define	DIR_OUT	(1)
struct mzen_gpio_table {
	int gpio_offset0;
	int gpio_offset1;
	int direction;
	int default_outval;		//0:output, 1:input
	spinlock_t slock;
} gpiotable[] = {
	{0x80, 8, 	DIR_IN, 	0, },	//MGPIO_GLED
	{0x80, 9, 	DIR_IN, 	0, },	//MGPIO_RLED
	{0x80, 23, 	DIR_IN,		-1, },	//MGPIO_TAMPER
	{0x80, 24, 	DIR_IN,		-1, },	//MGPIO_TEMPERATURE
	{0x80, 26, 	DIR_OUT,	0, },	//MGPIO_CLED0
	{0x80, 27, 	DIR_OUT,	0, },	//MGPIO_CLED1
	{0x80, 29, 	DIR_OUT,	1, },	//MGPIO_BUZZER

	{0x80, 27, 	DIR_OUT,	0, },	//CGPIO_RLED
	{0x80, 28, 	DIR_OUT,	0, },	//CGPIO_GLED
	{0x80, 29, 	DIR_IN,		0, },	//CGPIO_BAT
	{0x80, 30, 	DIR_OUT,	0, },	//CGPIO_BUZZER

	{0x80, 22, 	DIR_OUT,	0, },	//CGPIO_BLUE
	{-1, -1, 	DIR_OUT,	1, },
};


static int find_command_index(char *str)
{
	int i = 0;
	
	while (cmdstr_table[i] != NULL) {
		if (strcmp(cmdstr_table[i], str) == 0)
			return i;
		i++;
	}

	return -1;
}

#ifdef ENABLE_GPIO_INIT
static int _ich6_gpio_set_function(uint16_t base, unsigned offset, int func)
{
        u32 val;

        if (func) {
                val = inl(base);
                val |= (1UL << offset);
                outl(val, base);
        } else {
                val = inl(base);
                val &= ~(1UL << offset);
                outl(val, base);
        }

        return 0;
}

static int _ich6_gpio_set_direction(uint16_t base, unsigned offset, int dir)
{
        u32 val;

        if (!dir) {
                val = inl(base);
                val |= (1UL << offset);
                outl(val, base);
        } else {
                val = inl(base);
                val &= ~(1UL << offset);
                outl(val, base);
        }

        return 0;
}
#endif

static int _ich6_gpio_set_value(uint16_t base, unsigned offset, int value)
{
        u32 val;

        val = inl(base);
        if (value)
                val |= (1UL << offset);
        else
                val &= ~(1UL << offset);
        outl(val, base);

        return 0;
}

static int _ich6_gpio_get_value(uint16_t base, unsigned offset)
{
	int r;
	u32 tmplong;

	tmplong = inl(base);
	r = (tmplong & (1UL << offset)) ? 1 : 0;
	return r;
}

static void init_gpio_cfg(void)
{
	int i;
	struct mzen_gpio_table *gt;

	for (i=0; i<=GPIO_NUM_MAX; i++) {
		gt = &gpiotable[i];

#ifdef ENABLE_GPIO_INIT	//Use setting of u-boot
		_ich6_gpio_set_function(GPIO_USESEL_OFFSET(GPIOBASE) +
				gt->gpio_offset0,
				gt->gpio_offset1, 1);

		_ich6_gpio_set_direction(GPIO_IOSEL_OFFSET(GPIOBASE) + 
				gt->gpio_offset0,
				gt->gpio_offset1, 
				gt->direction);

		if (gt->default_outval != -1) {
			_ich6_gpio_set_value(GPIO_LVL_OFFSET(GPIOBASE) + 
					gt->gpio_offset0,
					gt->gpio_offset1, 
					gt->default_outval);
		}
#endif
		spin_lock_init(&gt->slock);
	}
}

void mzen_gpio_set_value(int index, int val)
{
	uint16_t base;
	unsigned long flags;
	struct mzen_gpio_table *gt = &gpiotable[index];

	base = GPIO_LVL_OFFSET(GPIOBASE) + gt->gpio_offset0;

	pr_debug("[%s] %x %x %x %x\n", __func__, base, gt->gpio_offset1, 
			index, val);
	spin_lock_irqsave(&gt->slock, flags);
	_ich6_gpio_set_value(base, gt->gpio_offset1, val);
	spin_unlock_irqrestore(&gt->slock, flags);
}
EXPORT_SYMBOL(mzen_gpio_set_value);

int mzen_gpio_get_value(int index)
{
	int r;
	uint16_t base;
	unsigned long flags;
	struct mzen_gpio_table *gt = &gpiotable[index];

	base = GPIO_LVL_OFFSET(GPIOBASE) + gt->gpio_offset0;

	pr_debug("[%s] %x %x %x\n", __func__, base, gt->gpio_offset1, index);
	spin_lock_irqsave(&gt->slock, flags);
	r = _ich6_gpio_get_value(base, gt->gpio_offset1);
	spin_unlock_irqrestore(&gt->slock, flags);
	return r;
}
EXPORT_SYMBOL(mzen_gpio_get_value);


static ssize_t mzen_gpio_proc_read(struct file *file, char __user *buf, size_t count, loff_t *f_ops)
{
	int idx;
	char tmpbuf[BUFSZ];

	idx = find_command_index(cmdstr);
	if (idx != -1) {
		cmdval = mzen_gpio_get_value(idx);
	}
	sprintf(tmpbuf, "%s %d\n", cmdstr, cmdval);
	return simple_read_from_buffer(buf, count, f_ops, tmpbuf, strlen(tmpbuf));
}

static ssize_t mzen_gpio_proc_write(struct file *file, const char __user *buf, size_t count, loff_t *f_pos)
{
	int cmdidx;
	size_t ret;
	char tmpbuf[BUFSZ];

	if (*f_pos > BUFSZ)
		return -EINVAL;

	ret = simple_write_to_buffer(tmpbuf, sizeof(tmpbuf), f_pos, buf, count);
	if (ret < 0)
		return ret;

	if (sscanf(tmpbuf, "%s %d", cmdstr, &cmdval) != 2)
		return -EINVAL;
	
	cmdidx = find_command_index(cmdstr);
	if (cmdidx < 0) 
		return -EINVAL;

	mzen_gpio_set_value(cmdidx, !!cmdval);
	return ret;
}

static const struct file_operations mzen_gpio_fops = {
	.owner = THIS_MODULE,
	.read = mzen_gpio_proc_read,
	.write = mzen_gpio_proc_write,
};

static int __init mzen_gpio_init(void)
{
	printk("[%s] start...\n", __func__);

	init_gpio_cfg();

	proc_create(FNAME, 0666, NULL, &mzen_gpio_fops);
	return 0;
}

static void __exit mzen_gpio_exit(void)
{
	remove_proc_entry(FNAME, NULL);

	printk("[%s] ...exit\n", __func__);
}

module_init(mzen_gpio_init);
module_exit(mzen_gpio_exit);

MODULE_LICENSE("GPL");
