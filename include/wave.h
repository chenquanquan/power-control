/*
 * wave.h - 2013年07月22日 15时25分22秒
 *
 * Copyright (c) 2013, chenchacha
 *
 */
#ifndef __WAVE_H__
#define __WAVE_H__

#define SPWM_TIMER_PERIPH	SYSCTL_PERIPH_TIMER0
#define SPWM_GPIO_PERIPH	SYSCTL_PERIPH_GPIOB
#define SPWM_TIMER_BASE	TIMER0_BASE
#define SPWM_GPIO_BASE	GPIO_PORTB_BASE
#define SPWM_TIEMR		TIMER_A
#define SPWM_INT_TIEMR	INT_TIMER0A
#define SPWM_PIN_1	GPIO_PIN_2
#define SPWM_PIN_2	GPIO_PIN_0
#define SPWM_MASK	(SPWM_PIN_1 | SPWM_PIN_2)

extern void wave_spwm(void);
extern void wave_pwm(unsigned long period1, unsigned long period2);
extern void wave_capture(void (*capture_handler)(void));
#endif /* __WAVE_H__ */
