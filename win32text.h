// wonbe Win32 text emulation
/* First Created: Nov.3,2000 by Pie Dey Co.,Ltd. */

/* This source code is distributed under GNU General Public License (GPL) */
/* see http://www.gnu.org/ about GPL */

int createText();
void clearText();
void drawChar( int x, int y, unsigned short ch );
void updateText();
void deleteText();
unsigned short win32_wait();
unsigned short win32_scan();
unsigned int win32_get_tick_count();
void win32_sys_wait( unsigned int val );
void win32_play_mml( const char * mml );

// end of win32text.h
