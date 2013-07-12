#ifndef __BUTTON_H__
#define __BUTTON_H__

#define BUTTON_PERIPH		SYSCTL_PERIPH_GPIOB
#define BUTTON_PORT		GPIO_PORTB_BASE
#define BUTTON_INT		INT_GPIOB

#define BUTTON_1		GPIO_PIN_3
#define BUTTON_2		GPIO_PIN_4
#define BUTTON_3		GPIO_PIN_6
#define BUTTON_4		GPIO_PIN_5
#define BUTTON_MASK		(BUTTON_1 | BUTTON_2 | BUTTON_3 | BUTTON_4)

void button_init_gpio();

#endif /* __BUTTON_H__ */
