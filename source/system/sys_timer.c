/* system module to produce PWM and SPWM
 */
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "hw_pwm.h"
#include "src/debug.h"
#include "src/interrupt.h"
#include "src/sysctl.h"
#include "src/timer.h"
#include "src/gpio.h"
#include "src/pin_map.h"
#include "src/pwm.h"

#include "lcd/lcd_5110.h"

#include "system/sys_timer.h"
#include "periph/dac_5618.h"
unsigned int capture_value;
void INT_TIMER0_A_routine(void)
{
	/* clean interrupt flag */
	TimerIntClear(T0_A_INT_CAP_EVENT);
	/* get capture value */
	capture_value =  TimerValueGet(T0_A);
}

extern const unsigned int sin_table[];
extern unsigned int sin_data[];
extern unsigned int dac_fre_value;
extern unsigned int dac_ref_value;
void INT_TIMER1_A_routine(void)
{
	static int i = 0;
	/* TimerDisable(T1_A); */
	TimerIntClear(T1_A_INT_TIMEOUT);
	/* dac5618 */
	DAC_write_data(sin_table[i] >> dac_ref_value, 1);

	TimerLoadSet(T1_A, 0xff);
	/* TimerMatchSet(T1_A , sin_data[i]);  */
	i += dac_fre_value;
	if (i > 1024)
		i = 0;

	/*
	capture_value = i;
	*/

	/* GPIOPinWrite(GPIO_PORTF_BASE, 0xff, sin_table[i]); */

}

void INT_TIMER2_A_routine(void)
{
	INT_TIMER1_A_routine();
	LCD_write_char(3, 0, 4);
}

/* TIMER_init_clk) - initialize timer and configure to counter
 * enable timer1A in counter 
 */
void TIMER_init_clk(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
	TimerConfigure(TIMER2_BASE, TIMER_CFG_32_BIT_PER);
	TimerLoadSet(TIMER2_BASE, TIMER_A, SysCtlClockGet());
	IntEnable(INT_TIMER2A);
	TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
	TimerEnable(TIMER2_BASE, TIMER_A);
}

/* TIMER_init_pwm() - initialize timer and configure to pwm
 * enable CCP2 and timer1A in PWM.
 */
void TIMER_init_pwm(void)
{
	/* enable CCP2 */
	/* SysCtlPeripheralEnable(CCP2_PERIPH); */
	LCD_write_char(3, 0, 1);
	/* configure CCP2 in PWM */
	/* GPIOPinTypeTimer(CCP2_PORT,CCP2_PIN); */
	LCD_write_char(3, 0, 2);
	/* enable CCP3 */
	/* SysCtlPeripheralEnable(CCP3_PERIPH); */
	/* configure CCP3 in PWM */
	/* GPIOPinTypeTimer(CCP3_PORT,CCP3_PIN); */

	/* enable timer 1 */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	LCD_write_char(3, 0, 3);
	/* configure timer 1 in 16 bit */
	TimerConfigure(T1_A_CFG_16_PER);
	/* TimerConfigure(T1_A_CFG_16_PWM); */
	LCD_write_char(3, 0, 4);
	/* set PWM output level */
	/* TimerControlLevel(T1_BOTH , true); */
	LCD_write_char(3, 0, 5);
	/* 100 division */
	/* TimerPrescaleSet(T1_BOTH , 99); */
	/* TimerPrescaleSet(T1_A, 2); */
	/* load initial value */
	/* TimerLoadSet(T1_BOTH , 0xff); */
	TimerLoadSet(T1_A, 0xffff);
	LCD_write_char(3, 0, 6);
	/* TimerMatchSet(T1_A , 128); */
	LCD_write_char(3, 0, 7);
	/* enable timer B interrupt */
	/* TimerIntEnable(T1_BOTH_INT_TIMEOUT); */
	TimerIntEnable(T1_A_INT_TIMEOUT);
	LCD_write_char(3, 0, 8);
	/* IntEnable(INT_TIMER1B); */
	IntEnable(INT_TIMER1A); 
	LCD_write_char(3, 0, 9);
	/* enable system interrupt */
	/* IntMasterEnable(); */
	/* enable timer B */
	/* TimerEnable(T1_BOTH); */
	TimerEnable(T1_A);
	LCD_write_char(3, 0, 0);
}

/* TIMER_init_capture() - initialize timer to capture
 * configure CCP0 and timer0A in capture.
 */
void TIMER_init_capture(void)
{
	/* enable CCP2 */
	SysCtlPeripheralEnable(CCP0_PERIPH);
	/* configure CCP2 in input */
	GPIOPinTypeTimer(CCP0_PORT, CCP0_PIN);
	/* enable timer0 */
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	/* configure timer0 in 16 bit */
	TimerConfigure(T0_A_CFG_16_CAP_TIME);
	/* allow timer stop */
	/* TimerControlLevel(T0_A, true); */
	/* CCP capture band edge */
	TimerControlEvent(T0_A_EVENT_POS_EDGE);
	/* set initial value */
	TimerLoadSet(T0_A, 0xffff);
	/* enable interrupt event */
	TimerIntEnable(T0_A_INT_CAP_EVENT);
	/* enable timer interrupt */
	IntEnable(INT_TIMER0A);
	/* enable system interrupt */
	/* IntMasterEnable(); */
	/* timer0A enable */
	TimerEnable(T0_A);
}
