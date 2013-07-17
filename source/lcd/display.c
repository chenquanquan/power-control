/*
 *  display.c - 2013年07月14日 18时31分51秒
 *
 *  Description:  display on LCD
 * 
 *  Copyright (c) 2013, chenchacha
 */
#include <string.h>
#include "periph/lcd_5110.h"
#include "lcd/frame-buffer.h"
#include "lcd/display.h"

char title[100];

/*  display_start - start display on LCD
*/
void display_start(frame_buffer_t *frame_buffer)
{
	unsigned char **fb = frame_buffer->fb;
	int column_max = frame_buffer->column_max;

	LCD_init();

	fb_write_char(0,5,"NOKIA 5110", fb, column_max);
/* 	sprintf(title, "Description: Display on LCD", fb, column_max); */
	fb_write_char(1,0,"Description:  display on LCD", fb, column_max);
	fb_write_char(3,0,"Volage:10000V, Current:10000A.", fb, column_max);
/* 	fb_write_point(2,1,fb,column_max); */
/*   	fb_write_line(8,5,3,10,fb, column_max);
 */
	LCD_draw_frame_buffer(0,0,fb, column_max);
}		/* -----  end of function display_start  ----- */

/*  display_roll - roll the framebuffer
 *  place: place point in framebuffer.
 *  roll: roll scope.
 *  dirctrion: roll dirction.
 */
int display_roll(frame_buffer_t *frame_buffer, int place, int roll, int dirction, int step)
{
	int i;
	unsigned char **fb = frame_buffer->fb;
	int column_max = frame_buffer->column_max;

	/* roll left */
	if (dirction) {
		for (i=place; i<place+roll;) {
			LCD_draw_frame_buffer(0,i,fb,column_max);
			i += step;
		}
	} else {  /* roll right */
		for (i=place; i>place-roll;) {
			LCD_draw_frame_buffer(0,i,fb,column_max);
			i -= step;
		}
	}

	return i;
}		/* -----  end of function display_roll  ----- */
