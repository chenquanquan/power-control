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

#include "system/sys_timer.h"

/*  TIMER_init - initial timer module
*/
void TIMER_init(TIMER_t *timer)
{
	/* Configure timers mode */
	TimerConfigure(timer->base, timer->config);
	TimerLoadSet(timer->base, timer->ntimer, timer->value);
	if (timer->handler) {
		/* Registe timer handler */
		TimerIntRegister(timer->base, timer->ntimer, timer->handler);
		/* Setup the interrupt for the timer timeouts */
		IntEnable(timer->interrupt);
		TimerIntEnable(timer->base, timer->intermod);
		/* Enable the timer */
		TimerEnable(timer->base, timer->ntimer);
	}

}		/* -----  end of function TIMER_init  ----- */

void timer0_handler_example(void)
{
	TimerLoadSet(TIMER0_BASE, TIMER_A, 0xf);
	/* Clear the timer interrupt */
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
}
