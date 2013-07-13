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
/* #define MODULE_LCD_5110 */
/* #define MODULE_DAC_5618 */
/* #define MODULE_BUTTON */


#ifdef MODULE_LCD_5110
#include "lcd/lcd_5110.h"
#endif

#ifdef MODULE_DAC_5618
#include "periph/dac_5618.h"
#endif

#ifdef MODULE_BUTTON
#include "periph/button.h"
#endif

#ifdef MODULE_TIMER
#include "src/timer.h"
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


void Timer0IntHandler(void)
{
	static unsigned char count = 0, i = 0;

#if 1
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
#endif
	/* Clear the timer interrupt */
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}

int main(void)
{
 	unsigned int n;

	jtag_wait();


#if 1
	SysCtlLDOSet(SYSCTL_LDO_2_75V);//  配置PLL前须将LDO设为2.75V

	SysCtlClockSet(SYSCTL_USE_PLL |//  系统时钟设置，采用PLL
			SYSCTL_OSC_MAIN |//  主振荡器
			SYSCTL_XTAL_6MHZ |//  外接6MHz晶振
			SYSCTL_SYSDIV_2);//  分频结果为20MHz
#endif

#define TIME_A_DIVISION		10

#ifdef MODULE_LCD_5110
	LCD_init();
	count = SysCtlClockGet() / TIME_A_DIVISION;

	LCD_write_char(0, 0, count / 1000 % 10);
	LCD_write_char(1, 0, count / 100 % 10);
	LCD_write_char(2, 0, count / 10 % 10);
	LCD_write_char(3, 0, count % 10);
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
	/* Configure the 32-bit periodic timers */
	TimerConfigure(TIMER0_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/1000);
	/*      TimerLoadSet(TIMER0_BASE, TIMER_A, 0xffffff00); */
	/* Registe timer handler */
	TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);
	/* Setup the interrupt for the timer timeouts */
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	/* Enable the timer */
	TimerEnable(TIMER0_BASE, TIMER_A);
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
#ifdef MODULE_LCD_5110
		/* 
		capture_value++;
		LCD_write_char(0, 3, capture_value / 100000 % 10);
		LCD_write_char(1, 3, capture_value / 10000 % 10);
		LCD_write_char(2, 3, capture_value / 1000 % 10);
		LCD_write_char(3, 3, capture_value / 100 % 10);
		LCD_write_char(4, 3, capture_value / 10 % 10);
		LCD_write_char(5, 3, capture_value % 10);
		*/
#endif
	}

	return 0;
}
