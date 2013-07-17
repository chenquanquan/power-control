/* effecture keyboard
 */
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "src/debug.h"
#include "src/sysctl.h"
#include "src/interrupt.h"
#include "src/timer.h"
#include "src/gpio.h"

#include "periph/button.h"
#include "periph/lcd_5110.h"

#include "src/pin_map.h"
#include "hw_pwm.h"
#include "src/pwm.h"

#include "system/sys_pwm.h"

/* button_init_gpio() - initilize GPIO for button
 */
void button_init_gpio(void)
{
	/* enable GPIO */
	SysCtlPeripheralEnable(BUTTON_PERIPH);
	/* configure GPIO in input */
	GPIOPinTypeGPIOInput(BUTTON_PORT, BUTTON_MASK);
	/* configure interrupt type */
	GPIOIntTypeSet(BUTTON_PORT, BUTTON_MASK, GPIO_FALLING_EDGE);
	/* GPIOIntTypeSet(BUTTON_PORT, BUTTON_MASK, GPIO_LOW_LEVEL); */
	/* enable interrupt to the processor */
	/* IntMasterEnable(); */
	/* set the interrupt prioprity */
	/* IntPrioritySet(BUTTON_INT, 0x00); */
	/* enable the pin interrupt */
	GPIOPinIntEnable(BUTTON_PORT, BUTTON_MASK);
	/* enable the port interrupt */
	IntEnable(BUTTON_INT);
}

/* GPIO_B_routine() - GPIO B interrupt routine
 */
unsigned int dac_ref_value = 0;
unsigned int dac_fre_value = 1;
extern unsigned int sin_table[];
extern unsigned int sin_data[];
void GPIO_B_routine(void)
{
	unsigned int status;
	unsigned int i;

	status = GPIOPinIntStatus(BUTTON_PORT, true);
	GPIOPinIntClear(BUTTON_PORT, status);

	/*tmp = ~tmp;
	GPIOPinWrite(GPIO_PORTB_BASE, 0xff, tmp);
	*/
	/* LCD_write_char(3, 2, status % 10); */
#if 1
	switch (status & BUTTON_MASK) {
		/* frequency decreate */
		case BUTTON_1:
			LCD_write_char(3, 3, 1);
			dac_fre_value += 1;
			break;

		/* frequency increate */
		case BUTTON_2:
			LCD_write_char(3, 3, 2);
			if (dac_fre_value > 1)
				dac_fre_value -= 1;
			break;

		/* output decreate */
		case BUTTON_3:
			LCD_write_char(3, 3, 3);
			dac_ref_value = 0;
			for (i = 0; i < 1024; i++)
				sin_data[i] = sin_table[i];
			break;

		/* output increate */
		case BUTTON_4:
			LCD_write_char(3, 3, 4);
			dac_ref_value++;
			for (i = 0; i < 1024; i++)
				sin_data[i] = sin_table[i] >> dac_ref_value;

			break;
			
		default:
			break;
	}
#endif

}




/* GPIO_E_routine() - GPIO E interrupt routine
 */
int ep_status = 0;
void GPIO_E_routine(void)
{
	unsigned int status;

	status = GPIOPinIntStatus(BUTTON_PORT, true);
	GPIOPinIntClear(BUTTON_PORT, status);

	/*tmp = ~tmp;
	GPIOPinWrite(GPIO_PORTB_BASE, 0xff, tmp);
	*/
}




