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
/* #define FRAME_BUFFER_COLUMN_MAX	(17*5*2) */
#define FRAME_BUFFER_COLUMN_MAX	(18*6*2)

typedef struct {
	unsigned char **fb;
	int column_max;
} frame_buffer_t;

extern void display_start(frame_buffer_t *frame_buffer);
extern int display_roll(frame_buffer_t *frame_buffer, int place, int roll, int dirction, int step);
#endif /* __DISPLAY_H__ */
