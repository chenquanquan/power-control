/*
 *  display.h - 2013年07月14日 18时32分07秒
 *
 *  Description:  display on LCD
 * 
 *  Copyright (c) 2013, chenchacha
 */
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#define FRAME_BUFFER_ROW_MAX	(6)
#define FRAME_BUFFER_COLUMN_MAX	(17*5*2)

extern void display_start(unsigned char **fb, int column_max);
extern void display_start_debug(unsigned char fb[][FRAME_BUFFER_COLUMN_MAX], int column_max);
extern void display_start_nostdlib(void);
#endif /* __DISPLAY_H__ */
