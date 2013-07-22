/*
 *  menu.h - 2013年07月18日 14时50分44秒
 *
 *  Description:  write menu on LCD
 * 
 *  Copyright (c) 2013, chenchacha
 */
#ifndef __MENU_H__
#define __MENU_H__

extern void menu_start(void);
extern int menu_roll(int screen);
extern void menu_refresh(void);
extern void menu_end(void);
#endif /* __MENU_H__ */
