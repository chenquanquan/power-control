/* #include "stdlib.h" */
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "src/debug.h"
#include "src/interrupt.h"
#include "src/sysctl.h"
#include "src/timer.h"
#include "src/gpio.h"

/* module */
#define MODULE_TIMER
#define MODULE_LCD
/* #define MODULE_DAC_5618 */
/* #define MODULE_BUTTON */
/* #define MODULE_STDLIB */

#ifdef MODULE_STDLIB
#endif

#ifdef MODULE_LCD
/* #include "periph/lcd_5110.h" */
#include "lcd/display.h"
unsigned char frame_buffer[FRAME_BUFFER_ROW_MAX][FRAME_BUFFER_COLUMN_MAX];
#endif

#ifdef MODULE_DAC_5618
#include "periph/dac_5618.h"
#endif

#ifdef MODULE_BUTTON
#include "periph/button.h"
#endif

#ifdef MODULE_TIMER
#include "system/sys_timer.h"
#include "data/spwm.h"
unsigned char spwm[256];
#endif

/* #include "data/sin.h" */

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

#ifdef MODULE_TIMER
void Timer0IntHandler(void)
{
	static unsigned char count = 0, i = 0;

	if (i == 0) {
		TimerLoadSet(TIMER0_BASE, TIMER_A, (spwm[count])+0xf);
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
		i++;

		if ((count==128) || (count==0))
			GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2,
					~GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2));
	} else {
		TimerLoadSet(TIMER0_BASE, TIMER_A, (0xff - spwm[count])+0xf);
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0);
		i=0;
		count++;
	}
	/* Clear the timer interrupt */
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}
#endif


int main(void)
{
 	unsigned int n;
	frame_buffer_t fb;
	unsigned int fb_place=0;

	jtag_wait();


#if 1
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
	fb.fb = (unsigned char **)frame_buffer;
	fb.column_max = FRAME_BUFFER_COLUMN_MAX;
	display_start(&fb);
#endif

#ifdef MODULE_BUTTON
	button_init_gpio();
#endif

#ifdef MODULE_TIMER
	for (n=0; n < 255; n++) {
		spwm[n] = 0xff - spwm_data[n];
	}
	/* Enable the peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	/* Set GPIO B0 as an output */
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_2);
	GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_2, 0);

	TIMER_t timer;
	timer.base = TIMER0_BASE;
	timer.ntimer = TIMER_A;
	timer.config = TIMER_CFG_32_BIT_PER;
	timer.value = SysCtlClockGet() / 1000;
	timer.interrupt = INT_TIMER0A;
	timer.intermod = TIMER_TIMA_TIMEOUT;
	timer.handler = Timer0IntHandler;
	TIMER_init(&timer);
#endif

	/* enable systerm interrupt */
 	IntMasterEnable();

	/* TIMER_init_capture(); */
	/* DAC_init_gpio(); */
	/* TIMER_init_pwm(); */
	/* TIMER_init_clk(); */


#ifdef MODULE_DAC_5618
	DAC_write_data(0x1ff, 1);
#endif

	while(1) {
#ifdef MODULE_LCD
		/* left roll the screen */
		fb_place = display_roll(&fb,fb_place,80,1, 8);
		/* right roll the screen */
		fb_place = display_roll(&fb,fb_place,80,0, 1);
#endif
	}

	return 0;
}
