/*
 * wave.c - 2013年07月22日 15时21分44秒
 *
 * Copyright (c) 2013, chenchacha
 * 
 */
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "src/sysctl.h"
#include "src/debug.h"
#include "src/interrupt.h"
#include "src/sysctl.h"
#include "src/timer.h"
#include "src/gpio.h"
#include "src/pwm.h"
#include "src/pin_map.h"

#include "wave.h"
#include "system/sys_timer.h"
#include "system/sys_pwm.h"
#include "data/spwm.h"
unsigned char spwm[256];

void timer0_spwm_handler(void)
{
	static unsigned char count = 0, i = 0;

	if (i == 0) {
		TimerLoadSet(TIMER0_BASE, TIMER_A, (spwm[count])+0xf);
		GPIOPinWrite(SPWM_GPIO_BASE, SPWM_PIN_2, 1);

		i++;

		if ((count==128) || (count==0))
			GPIOPinWrite(SPWM_GPIO_BASE, SPWM_PIN_1,
					~GPIOPinRead(SPWM_GPIO_BASE, SPWM_PIN_1));
	} else {
		TimerLoadSet(TIMER0_BASE, TIMER_A, (0xff - spwm[count])+0xf);
		GPIOPinWrite(SPWM_GPIO_BASE, SPWM_PIN_2, 0);

		i=0;
		count++;
	}
	/* Clear the timer interrupt */
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}


/* wave_spwm - (failed)
*/
void wave_spwm(void)
{
	int n;

	for (n=0; n < 255; n++) {
		spwm[n] = 0xff - spwm_data[n];
	}
	/* Enable the peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	/* Set GPIO B0 as an output */
    GPIOPinTypeGPIOOutput(SPWM_GPIO_BASE, SPWM_MASK);
	GPIOPinWrite(SPWM_GPIO_BASE, SPWM_MASK, 0);

	TIMER_t timer;
	timer.base = TIMER0_BASE;
	timer.ntimer = TIMER_A;
	timer.config = TIMER_CFG_32_BIT_PER;
	timer.value = SysCtlClockGet() / 1000;
	timer.interrupt = INT_TIMER0A;
	timer.intermod = TIMER_TIMA_TIMEOUT;
	timer.handler = timer0_spwm_handler;
	TIMER_init(&timer);
}

/* wave_pwm - output two pwm
*/
void wave_pwm(unsigned long period1, unsigned long period2)
{
	PWM_t pwm1, pwm2;
	int count = SysCtlClockGet();

	/* configure pwm1 */
	pwm1.gpio_periph = SYSCTL_PERIPH_GPIOD;
	pwm1.gpio_base = GPIO_PORTD_BASE;
	pwm1.gpio = GPIO_PIN_0;
	pwm1.gen = PWM_GEN_0;
	pwm1.config = PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC;
	pwm1.period = count / period1;
	pwm1.width = pwm1.period / 2;
	pwm1.out = PWM_OUT_0;
	pwm1.outbit = PWM_OUT_0_BIT;
	pwm1.handler = 0;
	PWM_init(&pwm1);

	pwm1.gpio_periph = SYSCTL_PERIPH_GPIOD;
	pwm1.gpio_base = GPIO_PORTD_BASE;
	pwm1.gpio = GPIO_PIN_1;
	pwm1.gen = PWM_GEN_1;
	pwm1.config = PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC;
	pwm1.period = count / period2;
	pwm1.width = pwm1.period / 2;
	pwm1.out = PWM_OUT_1;
	pwm1.outbit = PWM_OUT_1_BIT;
	pwm1.handler = 0;
	PWM_init(&pwm1);

	/* configure pwm2 */
	pwm2.gpio_periph = SYSCTL_PERIPH_GPIOB;
	pwm2.gpio_base = GPIO_PORTB_BASE;
	pwm2.gpio = GPIO_PIN_0;
	pwm2.gen = PWM_GEN_2;
	pwm2.config = PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC;
	pwm2.period = count / period2;
	pwm2.width = pwm2.period / 2;
	pwm2.out = PWM_OUT_2;
	pwm2.outbit = PWM_OUT_2_BIT;
	pwm2.handler = 0;
 	PWM_init(&pwm2);
}		/* -----  end of function wave_pwm  ----- */

/* wave_capture - wave capture by timer
 * @capture_handler: the handler function for capture.
 * enable timer0 channel A and CCP0(PA0).
 * capture all of edges. both positive edges and negative edges.
 */
void wave_capture(void (*capture_handler)(void))
{
	TIMER_t timer;

	/* Enable the peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(CCP0_PERIPH);
	/* Set GPIO B0 as an output */
	GPIOPinTypeTimer(CCP0_PORT, CCP0_PIN);

	/* configure timer structure */
	timer.base = TIMER0_BASE;
	timer.ntimer = TIMER_A;
	/* Timer A event timer and Two 16-bit timers */
	timer.config = TIMER_CFG_A_CAP_TIME | TIMER_CFG_16_BIT_PAIR;
	timer.event_config = TIMER_EVENT_BOTH_EDGES;
	timer.value = 0xffff;
	timer.interrupt = INT_TIMER0A;
	timer.prescale = 0;
	/* CaptureA event interrupt */
	timer.intermod = TIMER_CAPA_EVENT;
	timer.handler = capture_handler;
	TIMER_init(&timer);

	TimerEnable(timer.base, timer.ntimer);
}		/* -----  end of function wave_capture  ----- */

/* wave_capture_32 - wave capture by timer
 * @capture_handler: the handler function for capture.
 * enable timer0 channel A and CCP0(PA0).
 * capture all of edges. both positive edges and negative edges.
 */
void wave_cap32(void (*capture_handler)(void))
{
	TIMER_t timer;

	/* Enable the peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(WAVE_32_PERIPH);
	/* configure GPIO in input */
	GPIOPinTypeGPIOInput(WAVE_32_PORT, WAVE_32_PIN);
	/* configure interrupt type */
	GPIOIntTypeSet(WAVE_32_PORT, WAVE_32_PIN, GPIO_BOTH_EDGES);

	/* register the interrupt handler */
	GPIOPortIntRegister(WAVE_32_PORT, capture_handler);
	/* enable the pin interrupt */
	GPIOPinIntEnable(WAVE_32_PORT, WAVE_32_PIN);

	/* configure timer structure */
	timer.base = TIMER0_BASE;
	timer.ntimer = TIMER_A;
	timer.config = TIMER_CFG_32_BIT_PER;
	timer.value = 0xffffffff;
	timer.event_config = 0xffffffff;
	timer.prescale = 0;
	TIMER_init(&timer);

	/* enable the port interrupt */
	TimerEnable(timer.base, timer.ntimer);
	IntEnable(WAVE_32_INT);
}		/* -----  end of function wave_capture  ----- */

/* wave_cap32_load - load the timer interrupt value
 */
void wave_cap32_load(unsigned long value)
{
	TimerLoadSet(TIMER0_BASE, TIMER_A, value);
}		/* -----  end of function wave_interrupt_load  ----- */

/* wave_cap32_start - start counter
 */
void wave_cap32_start(void)
{
	TimerEnable(TIMER0_BASE, TIMER_A);
}		/* -----  end of function wave_interrupt_start  ----- */

/* wave_cap32_stop -
 */
void wave_cap32_stop(void)
{
	TimerDisable(TIMER0_BASE, TIMER_A);
}		/* -----  end of function wave_interrupt_stop  ----- */

/* wave_cap32_getvalue -
*/
unsigned long wave_cap32_getvalue(void)
{
	return (0xffffffff - TimerValueGet(TIMER0_BASE, TIMER_A));
}		/* -----  end of function wave_cap32_getvalue  ----- */

/* wave_cap32_clean -
*/
void wave_cap32_clean(void)
{
	unsigned int status;

	status = GPIOPinIntStatus(WAVE_32_PORT, true);
	GPIOPinIntClear(WAVE_32_PORT, status);
}		/* -----  end of function wave_interrupt_clean  ----- */

/* wave_interrupt_init - enable a timer interrupt
 * @value: the count value.
 * @wave_handler: the handler function for capture.
 */
void wave_interrupt_init(unsigned long value, void (*wave_handler)(void))
{
	/* Enable the peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

	TIMER_t timer;
	timer.base = TIMER1_BASE;
	timer.ntimer = TIMER_A;
	timer.config = TIMER_CFG_32_BIT_PER;
	timer.value = value;
	timer.interrupt = INT_TIMER1A;
	timer.prescale = 0;
	timer.intermod = TIMER_TIMA_TIMEOUT;
	timer.handler = wave_handler;
	TIMER_init(&timer);

	/* Eable output pin */
	SysCtlPeripheralEnable(WAVE_INT_PPER);
	GPIOPinTypeGPIOOutput(WAVE_INT_PBASE, WAVE_INT_PIN);
	GPIOPinTypeGPIOInput(WAVE_INT_PBASE, WAVE_SCAN_PIN);

	GPIOPinWrite(GPIO_PORTD_BASE, WAVE_INT_PIN, 0);
}

/* wave_interrupt_load - load the timer interrupt value
 */
void wave_interrupt_load(unsigned long value)
{
	TimerLoadSet(TIMER1_BASE, TIMER_A, value);
}		/* -----  end of function wave_interrupt_load  ----- */

/* wave_interrupt_start - start counter
 */
void wave_interrupt_start(void)
{
	TimerEnable(TIMER1_BASE, TIMER_A);
}		/* -----  end of function wave_interrupt_start  ----- */

/* wave_interrupt_stop -
 */
void wave_interrupt_stop(void)
{
	TimerDisable(TIMER1_BASE, TIMER_A);
}		/* -----  end of function wave_interrupt_stop  ----- */

/* wave_interrupt_clean -
*/
void wave_interrupt_clean(void)
{
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
}		/* -----  end of function wave_interrupt_clean  ----- */
