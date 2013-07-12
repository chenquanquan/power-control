/* driver for nokia-5110 lcd
 * 2012.7.23
 */
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "src/debug.h"
#include "src/sysctl.h"
#include "src/gpio.h"
#include "lcd/lcd_5110.h"
#include "lcd/char_data.h"

extern unsigned long sys_clock_value;
#define delay_ms(a)	SysCtlDelay((a) * (sys_clock_value / 3000));
#define delay_us(a)	SysCtlDelay((a) * (sys_clock_value / 3));

/* LCD_init_gpio() - initialize LCD gpio
 */
static void LCD_init_gpio(void)
{
	/* enable GPIO */
	SysCtlPeripheralEnable(LCD_PERIPH);
	/* configure GPIO in output */
	GPIOPinTypeGPIOOutput(LCD_PORT, LCD_PORT_MASK);
}

/* LCD_write_byte(dat,cmd) - write 8 bit to LCD by SPI
 * @dat: write data by SPI.
 * @cmd: flag of command or data, 0 is command, 1 is data.
 */
static void LCD_write_byte(unsigned char dat, unsigned int cmd)
{
	unsigned int i;
	
	/* enable LCD */
	LCD_CE_L;
	/* check command or data */
	if (cmd)
		LCD_DC_H;	/* dat is data */
	else
		LCD_DC_L;	/* dat is command */

	/* write data by GPIO */
	for (i = 0; i < 8; i++) {
		LCD_CLK_L;

		if (dat & 0x80)
			LCD_DAT_H;
		else
			LCD_DAT_L;

		dat <<= 1;
		LCD_CLK_H;
	}

	/* disable LCD */
	LCD_CE_H;
}

/* LCD_clean() - clean LCD display
 */
void LCD_clean(void)
{
	unsigned int i;

	LCD_write_byte(0x40, 0); /* start low */
	LCD_write_byte(0x80, 0); /* start line */

	for (i = 0; i < 504; i++)
		LCD_write_byte(0, 1);
}

/* LCD_init() - initialize LCD
 */
void LCD_init(void)
{
	LCD_init_gpio();

	/* reset the LCD */
	LCD_RST_L;
	delay_us(1);
	LCD_RST_H;

	/* enable the LCD */
	LCD_CE_L;
	delay_us(1);
	LCD_CE_H;
	delay_us(1);

	/* write initialize command */
	LCD_write_byte(0x21, 0);
	LCD_write_byte(0xc8, 0);
	LCD_write_byte(0x13, 0);
	LCD_write_byte(0x06, 0);
	LCD_write_byte(0x20, 0);
	LCD_write_byte(0x0c, 0);

	LCD_clean();
}

/* LCD_set_pos(x,y) - set pos in LCD
 * @x:
 * @y:
 */
void LCD_set_pos(unsigned int x, unsigned int y)
{
	LCD_write_byte(0x40 | y, 0);
	LCD_write_byte(0x80 | x, 0);
}

/* LCD_write_char(row,page,dat) - write charactor in LCD
 * @row: local position in LCD.
 * @line:
 * @dat: charactor data.
 */
void LCD_write_char(unsigned int row, unsigned int line, unsigned char dat)
{
	unsigned int i;

	LCD_set_pos(row * 8, line);
	for (i = 0; i < 8; i++)
		LCD_write_byte(table_char[dat * 16 + i], 1);
	LCD_set_pos(row * 8, line + 1);
	for (i = 8; i < 16; i++)
		LCD_write_byte(table_char[dat * 16 + i], 1);
}

