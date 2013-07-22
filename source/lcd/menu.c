/*
 *  menu.c - 2013年07月18日 14时51分38秒
 *
 *  Description:  write menu on LCD
 * 
 *  Copyright (c) 2013, chenchacha
 */
#include <stdlib.h>
#include "lcd/display.h"
#include "periph/button.h"

unsigned char frame_buffer[FRAME_BUFFER_ROW_MAX][FRAME_BUFFER_COLUMN_MAX];
frame_buffer_t fb;
unsigned int fb_place=0;

/*  menu_start - initial all module for menu
 *  initial LCD.
 *  initial button.
 */
void menu_start(void)
{

	/*
	unsigned char *frame_buffer;
 	frame_buffer = (unsigned char *)malloc((sizeof (unsigned char)) * FRAME_BUFFER_ROW_MAX * FRAME_BUFFER_COLUMN_MAX); */

	/* initial LCD */
	fb.fb = (unsigned char *)frame_buffer;
	fb.column_max = FRAME_BUFFER_COLUMN_MAX;
	display_start(&fb);

	/* initial button */
	button_init_gpio();
}		/* -----  end of function menu_start  ----- */

void menu_end(void)
{
/* 	free(fb.fb); */
}

/*  menu_roll - roll the display to screen
 *  return now screen.
*/
int menu_roll(int screen)
{
	static int now_screen=0;
	static int fb_place = 0;

	if (screen > now_screen) {
		/* left roll the screen */
		fb_place = display_roll(&fb,fb_place,1,1, 1);
		fb_place = display_roll(&fb,fb_place,7,1, 7);

		fb_place = display_roll(&fb,fb_place,60,1, 20);

		fb_place = display_roll(&fb,fb_place,15,1, 5);
		fb_place = display_roll(&fb,fb_place,1,1, 1);
		now_screen++;
	} else if (screen < now_screen) {
		/* right roll the screen */
		fb_place = display_roll(&fb,fb_place,1,0, 1);
		fb_place = display_roll(&fb,fb_place,7,0, 7);

		fb_place = display_roll(&fb,fb_place,60,0, 20);

		fb_place = display_roll(&fb,fb_place,15,0, 5);
		fb_place = display_roll(&fb,fb_place,1,0, 1);
		now_screen--;
	}
	return now_screen;

}		/* -----  end of function menu_roll  ----- */

/*  menu_refresh - refresh the display with button
*/
extern unsigned int gpio_b_int_status;
void menu_refresh(void)
{
	static int screen = 0;
	static int i = 0;

#if 0
	switch (gpio_b_int_status & BUTTON_4) {
		/* next page */
		case BUTTON_1:
			if (screen < (FRAME_BUFFER_SCREEN - 1))
				screen++;
			break;
		/* previous page */
		case BUTTON_2:
			if (screen > 0)
				screen--;
			break;
		/* look around */
		case BUTTON_4:
			screen++;
			screen %= (FRAME_BUFFER_SCREEN - 1);
			break;
		default:
			break;
	}
#endif
	if (gpio_b_int_status && !button_test(BUTTON_4)) {
		if  (i == 0) {
			if (screen < (FRAME_BUFFER_SCREEN - 1) ) screen++;
			else {
				i=1;
				screen--;
			}
		} else {
			if (screen > 0) screen--;
			else {
				i=0;
				screen++;
			}
		}
		gpio_b_int_status = 0;
	}

	menu_roll(screen);
}		/* -----  end of function menu_refresh  ----- */
