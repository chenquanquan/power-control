/*
 *  frame-buffer.h - 2013年07月14日 18时07分03秒
 *
 *  Description:  
 * 
 *  Copyright (c) 2013, chenchacha
 */
#ifndef __FRAME_BUFFER_H__
#define __FRAME_BUFFER_H__

extern void fb_write_char(int row, int column, char *ptrc, unsigned char **frame_buffer, int column_max);

extern void fb_write_line(int start_r, int start_c, int length, int width, unsigned char **fb, int column_max);

#include "lcd/display.h"
extern void fb_write_char_debug(int row, int column, char *ptrc, unsigned char fb[][FRAME_BUFFER_COLUMN_MAX], int column_max);
void fb_write_char_nostdlib(int row, int column, char *ptrc); 
#endif /* __FRAME_BUFFER_H__ */
