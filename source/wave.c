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


/* wave_spwm -
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
