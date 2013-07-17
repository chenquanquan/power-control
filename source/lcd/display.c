/*
 *  display.c - 2013年07月14日 18时31分51秒
 *
 *  Description:  display on LCD
 * 
 *  Copyright (c) 2013, chenchacha
 */
#include "periph/lcd_5110.h"
#include "lcd/frame-buffer.h"
#include "lcd/display.h"


/*  display_start - start display on LCD
*/
void display_start(unsigned char **fb, int column_max)
{
	LCD_init();

	fb_write_char(1,1,"NOKIA", fb, column_max);
/* 	fb_write_line(8,5,3,10,fb); */
	LCD_draw_frame_buffer(0,0,fb, column_max);
}		/* -----  end of function display_start  ----- */

/*  display_start_debug - start display on LCD with arry
*/
void display_start_debug(unsigned char fb[][FRAME_BUFFER_COLUMN_MAX], int column_max)
{
	LCD_init();

	fb_write_char_debug(1,1,"NOKIA", fb, column_max);
/* 	fb_write_line(8,5,3,10,fb); */
	LCD_draw_fb_debug(0,0,fb, column_max);
}

/*  display_start_nostdlib - start display on LCD with global variable lcd_frame_buffer
*/
#if 1
void display_start_nostdlib(void)
{
	LCD_init();

	fb_write_char_nostdlib(1,1,"NOKIA");
/* 	fb_write_line(8,5,3,10,fb); */
	LCD_draw_fb_nostdlib(0,0);
}
#endif
