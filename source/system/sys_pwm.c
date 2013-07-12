/* sys_pwm.c
 * system module to produce PWM and SPWM
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
#include "system/sys_pwm.h"


/*  PWM_init - initial PWM module
*/
void PWM_init(PWM_t *pwm)
{
	/* Enable the peripherals used by this example */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypePWM(GPIO_PORTD_BASE, pwm->gpio);
    PWMGenConfigure(PWM_BASE, pwm->gen, pwm->config);
	/* Set PWM period */
	PWMGenPeriodSet(PWM_BASE, pwm->gen, pwm->period);
	/* Set PWM Width */
    PWMPulseWidthSet(PWM_BASE, pwm->out, pwm->width);
    PWMOutputState(PWM_BASE, pwm->out, true);
	/* if interrupt handler set */
	if (pwm->handler) {
		PWMGenIntTrigEnable(PWM_BASE, pwm->intergen, pwm->trig);
		PWMGenIntRegister(PWM_BASE, pwm->intergen, pwm->handler);
		PWMIntEnable(PWM_BASE, pwm->intergen);
	}
    PWMGenEnable(PWM_BASE, pwm->gen);
}		/* -----  end of function PWM_init  ----- */
