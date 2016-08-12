#ifndef __GPIP_MZEN_H__
#define __GPIP_MZEN_H__

/*
 * If you want to add some fileds in mzen_gpio_index, 
 * You have to add that in cmdstr_table variable of gpio-mzen.c too.
 *
 * MGPIO_XXX: Main part GPIO
 * CGPIO_XXX: Crypto part GPIO
 */
enum mzen_gpio_index {
	MGPIO_GLED=0,
	MGPIO_RLED,
	MGPIO_TAMPER,
	MGPIO_TEMPERATURE,
	MGPIO_CLED0,
	MGPIO_CLED1,
	MGPIO_BUZZER,

	CGPIO_RLED,
	CGPIO_GLED,
	CGPIO_BAT,
	CGPIO_BUZZER,

	CGPIO_BLUE, /*test purpose on minnowboard turbot*/
	GPIO_NUM_MAX,
};

#endif
