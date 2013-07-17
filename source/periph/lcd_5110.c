/* driver for nokia-5110 lcd
 * 2012.7.23
 */
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "src/debug.h"
#include "src/sysctl.h"
#include "src/gpio.h"
#include "periph/lcd_5110.h"

#define delay_ms(a)	SysCtlDelay((a) * (SysCtlClockGet() / 3000));
#define delay_us(a)	SysCtlDelay((a) * (SysCtlClockGet() / 3));


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


#if 0
extern unsigned char lcd_font[96][5];
unsigned char lcd_fb[6][17*5];

void LCD_write_char(int row, int column, char *ptrc)
{
	int i, j, n;
	unsigned char word;

	i=row;
	j=column;

	while (*ptrc!='\0') {
		word = *ptrc & 0x7f;
		if (word < ' ')
			word = 0;
		else
			word -= ' ';

		/* write char in frame buffer */
		for (n=0;n<5;n++) {
			lcd_fb[i][j+n] = lcd_font[word][n];
		}
		j += 5;
		ptrc++;
	}

}

void LCD_draw_fb(int start_row, int start_column)
{
	unsigned int i, column, row;

	LCD_set_pos(0, 0);
	/* enable LCD */
	LCD_CE_L;
	/* dat is data */
	LCD_DC_H;


	for (row=start_row; row<start_row+6; row++) {
		for (column=start_column; column<start_column+84; column++) {
			for (i = 0; i < 8; i++) {
				LCD_CLK_L; 

				if (lcd_fb[row][column] & 0x80 >> i)
					LCD_DAT_H;
				else
					LCD_DAT_L;

				LCD_CLK_H;
			}
		}
	}

	/* disable LCD */
	LCD_CE_H;
}

void LCD_start()
{
	LCD_init();

	LCD_write_char(1,1,"NOKIA");
/* 	fb_write_line(8,5,3,10,fb); */
	LCD_draw_fb(0,0);
}
#endif

/* LCD_draw() - draw frame buffer on LCD
 */
void LCD_draw_frame_buffer(int start_row, int start_column,
							unsigned char **frame_buffer, int column_max)
{
	unsigned int i, column, row;

	LCD_set_pos(0, 0);
	/* enable LCD */
	LCD_CE_L;
	/* dat is data */
	LCD_DC_H;


	for (row=start_row; row<start_row+6; row++) {
		for (column=start_column; column<start_column+84; column++) {
			for (i = 0; i < 8; i++) {
				LCD_CLK_L; 

/* 				if ((*(unsigned char *)frame_buffer+row*column_max+column) & 0x80 >> i) */
				if ((*((unsigned char *)frame_buffer+row*column_max+column)) & 0x80 >> i)
					LCD_DAT_H;
				else
					LCD_DAT_L;

				LCD_CLK_H;
			}
		}
	}

	/* disable LCD */
	LCD_CE_H;
}
