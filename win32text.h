// wonbe Win32 text emulation
// First Created: Nov.4,2000 by Nashiko
// Copyright 2000 (c) by Pie Dey Co.,Ltd.

int createText();
void clearText();
void drawChar( int x, int y, unsigned short ch );
void updateText();
void deleteText();
unsigned short win32_wait();
unsigned short win32_scan();
unsigned int win32_get_tick_count();
void win32_sys_wait( unsigned int val );

// end of win32text.h
