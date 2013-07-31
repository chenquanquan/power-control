
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "src/debug.h"
#include "src/interrupt.h"
#include "src/sysctl.h"
#include "src/timer.h"
#include "src/gpio.h"

/* module */
/* #define MODULE_SPWM */
#define MODULE_LCD
#define MODULE_PWM
#define MODULE_PLL
#define MODULE_ADS
#define MODULE_CAP
/* #define MODULE_DAC_5618 */
/* #define MODULE_BUTTON */


#ifdef MODULE_ADS
#include "periph/ads1115.h"
#endif

#ifdef MODULE_LCD
/* #include "periph/lcd_5110.h" */
/* #include "lcd/display.h" */
#include "lcd/menu.h"
#endif

#ifdef MODULE_DAC_5618
#include "periph/dac_5618.h"
#endif

#ifdef MODULE_BUTTON
#include "periph/button.h"
#endif

#ifdef MODULE_PWM
#include "wave.h"
#endif

#ifdef MODULE_CAP
#define TIMER_VALUE_DEEPIN (16)
volatile unsigned long timer_cap[TIMER_VALUE_DEEPIN];
void timer_capture_handler(void)
{
	static unsigned char i = 0;

	timer_cap[i++] = TimerValueGet(TIMER0_BASE, TIMER_A);
	TimerLoadSet(TIMER0_BASE, TIMER_A, 0xffff);
	TimerIntClear(TIMER0_BASE, TIMER_CAPA_EVENT);
	
	/* get the wave form */
	i = i%TIMER_VALUE_DEEPIN;

	/* follower wave pin */
	WAVE_INT_OU;
	/* Enable the follower wave */
	wave_interrupt_start();
}

void timer_interrupt_handler(void)
{
	wave_interrupt_clean();
	/* follower wave pin */
	WAVE_INT_OD;
	wave_interrupt_stop();
}
#endif

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

/* jtag_wait() - wait for JTAG pin. protect the JTAG
 */
void jtag_wait(void)
{
	/* enable key GPIO */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	/* set key GPIO is input */
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_5);
	/* if press button while rest */
	if (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_5) == 0)
		for (;;);
	/* disable key GPIO */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
}


int main(void)
{
	unsigned int count=0;
 	int m, n;
	unsigned long tmp1, tmp2;

	jtag_wait();


#ifdef MODULE_PLL
	/* set LDO in 2.75v */
	SysCtlLDOSet(SYSCTL_LDO_2_75V);

	/* configure system clock
	 * PLL.
	 * main osc.
	 * xtal 6MHz.
	 * system clock div 20MHz.
	 */
	SysCtlClockSet(SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_6MHZ|SYSCTL_SYSDIV_2);
#endif

#ifdef MODULE_LCD
	menu_start();
#endif

#ifdef MODULE_BUTTON
#endif

#ifdef MODULE_SPWM
	wave_spwm();
#endif

#ifdef MODULE_CAP
	wave_capture(timer_capture_handler);
	wave_interrupt_init(0xffff, timer_interrupt_handler);
#endif

	/* enable systerm interrupt */
 	IntMasterEnable();

#ifdef MODULE_DAC_5618
	DAC_write_data(0x1ff, 1);
#endif


#ifdef MODULE_PWM
	wave_pwm(10000, 80000);
#endif

	while(1) {
#ifdef MODULE_LCD

#ifdef MODULE_ADS
		int i, j;
		char string[30];
		unsigned int ads_value;

		for (i=0; i<3; i++) {
			for (j = 10; j; j--)
				ads_value = ads_read(i);

			sprintf(string, "ADC%d: %6d", i,  ads_value);
			menu_add_string(i, string);
		}


#endif

#ifdef MODULE_CAP
		for (tmp1 = timer_cap[0], count=2; count<TIMER_VALUE_DEEPIN; count+=2)
			tmp1 = (tmp1 + timer_cap[count]) >> 1;

 		sprintf(string, "CAP: %6ld", tmp1);
		menu_add_string(i++, string);

		for (tmp2 = timer_cap[1], count=3; count<TIMER_VALUE_DEEPIN; count+=2)
			tmp2 = (tmp2 + timer_cap[count]) >> 1;

 		sprintf(string, "CAP: %6ld", tmp2);
		menu_add_string(i, string);

		/* which is zero point */
		if (tmp1 > tmp2)
			tmp1 = tmp2;

		/* load value to follower wave */
		wave_interrupt_load(tmp1);
#endif


		m = menu_refresh();

#ifdef MODULE_PWM
		if (n != m) {
			n = m;
			wave_pwm((n+1)*10000, 80000);
		}
#endif
#endif
	}

#ifdef MODULE_LCD
	menu_end();
#endif

	return 0;
}
