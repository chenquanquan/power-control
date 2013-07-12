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

#define MODULE_LCD_5110
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

unsigned long sys_clock_value;
unsigned int sin_data[1024];
extern unsigned int capture_value;

int main(void)
{
	unsigned int count;

	jtag_wait();


	sys_clock_value = 10;
#if 1
	SysCtlLDOSet(SYSCTL_LDO_2_75V);//  配置PLL前须将LDO设为2.75V

	SysCtlClockSet(SYSCTL_USE_PLL |//  系统时钟设置，采用PLL
			SYSCTL_OSC_MAIN |//  主振荡器
			SYSCTL_XTAL_6MHZ |//  外接6MHz晶振
			SYSCTL_SYSDIV_2);//  分频结果为20MHz

	sys_clock_value = SysCtlClockGet();
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

	/* enable systerm interrupt */
/* 	IntMasterEnable(); */

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
