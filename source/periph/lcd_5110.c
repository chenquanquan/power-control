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
#include "lcd/display.h"

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

/* LCD_write_char(row,page,dat) - write charactor in LCD
 * @row: local position in LCD.
 * @line:
 * @dat: charactor data.
 */
#if 0
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
#endif

/* LCD_write_char(row,page,dat) - write 6*8 charactor in LCD
 * @row: local position in LCD.
 * @line:
 * @dat: charactor data.
 */
#if 0
void LCD_write_small_char(unsigned int row, unsigned int line, unsigned char dat)
{
	unsigned int i;

	LCD_set_pos(row * 8, line);

	dat &= 0x7f;
	if (dat < ' ')
		dat = 0;
	else
		dat -= ' ';

	for (i = 0; i < 5; i++)
		LCD_write_byte(lcd_font[dat][i], 1);
}
#endif

#if 0
extern unsigned char lcd_font[96][5];
void LCD_draw_all_char(void)
{
	unsigned int i,count,word;

	LCD_set_pos(0, 0);
	/* enable LCD */
	LCD_CE_L;
	/* dat is data */
	LCD_DC_H;

	for (word=0; word<90; word++) {
		for (count=0; count<5; count++) {
			for (i = 0; i < 8; i++) {
				LCD_CLK_L; 

				if (lcd_font[word][count] & 0x80 >> i)
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
#endif

#if 1
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

#if 1
/* LCD_draw() - draw frame buffer on LCD
 */
void LCD_draw_fb_debug(int start_row, int start_column,
							unsigned char frame_buffer[][FRAME_BUFFER_COLUMN_MAX],
							int column_max)
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
				if (frame_buffer[row][column] & 0x80 >> i)
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
#endif

#if 0

/* write char on framebuffer an framebuffer is a global variable */
void fb_write_char_nostdlib(int row, int column, char *ptrc)
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
			lcd_frame_buffer[i][j+n] = lcd_font[word][n];
		}
		j += 5;
		ptrc++;
	}

}
#endif

#if 1
extern unsigned char lcd_frame_buffer[FRAME_BUFFER_ROW_MAX][FRAME_BUFFER_COLUMN_MAX];
void LCD_draw_fb_nostdlib(int start_row, int start_column)
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
				if (lcd_frame_buffer[row][column] & 0x80 >> i)
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

#endif

#if 0
void display_start_nostdlib(void)
{
	LCD_init();

	fb_write_char_nostdlib(1,1,"NOKIA");
/* 	fb_write_line(8,5,3,10,fb); */
	LCD_draw_fb_nostdlib(0,0);
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
