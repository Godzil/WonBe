// wonbe Win32 text emulation
// First Created: Nov.4,2000 by Nashiko
// Copyright 2000 (c) by Pie Dey Co.,Ltd.

#include <windows.h>
#include "win32text.h"

#define SCREEN_SIZE_X (224/8)
#define SCREEN_SIZE_Y (144/8)
static WORD screen[SCREEN_SIZE_X][SCREEN_SIZE_Y];

//static WORD lastChar;
static UINT lastVKey;

static HWND hwnd;

static void wmPaint( HWND hWnd, HDC hdc )
{
	int x,y;
	int xArea, yArea;
	RECT rect, rectDraw;
	TEXTMETRIC tm;

	GetClientRect( hWnd, &rect );

	SelectObject( hdc, GetStockObject( SYSTEM_FIXED_FONT ) );
	GetTextMetrics( hdc, &tm );

	xArea = SCREEN_SIZE_X * tm.tmMaxCharWidth;
	yArea = SCREEN_SIZE_Y * tm.tmHeight;

	rectDraw.left = (rect.right-rect.left)/2-xArea/2;
	rectDraw.top = (rect.bottom-rect.top)/2-yArea/2;
	rectDraw.right = (rect.right-rect.left)/2+xArea/2;
	rectDraw.bottom = (rect.bottom-rect.top)/2+yArea/2;

	FillRect( hdc, &rect, GetStockObject(GRAY_BRUSH) );
	FillRect( hdc, &rectDraw, GetStockObject(WHITE_BRUSH) );

	for( y=0; y<SCREEN_SIZE_Y; y++ ) {
		for( x=0; x<SCREEN_SIZE_X; x++ ) {
			WORD val;
			BYTE str[2];
			val = screen[x][y];
			if( val >= 0x100 ) {
				str[0] = (val >> 8);
				str[1] = (val & 0xff);
				TextOut( hdc,
					rectDraw.left+x*tm.tmMaxCharWidth,
					rectDraw.top+y*tm.tmHeight,
					str, 2 );
			} else {
				static char table[] = "Å@ÅIÅhÅîÅêÅìÅïÅfÅiÅjÅñÅ{ÅCÅ|ÅDÅ^ÇOÇPÇQÇRÇSÇTÇUÇVÇWÇXÅFÅGÅÉÅÅÅÑÅHÅóÇ`ÇaÇbÇcÇdÇeÇfÇgÇhÇiÇjÇkÇlÇmÇnÇoÇpÇqÇrÇsÇtÇuÇvÇwÇxÇyÅmÅèÅnÅOÅQÅMÇÅÇÇÇÉÇÑÇÖÇÜÇáÇàÇâÇäÇãÇåÇçÇéÇèÇêÇëÇíÇìÇîÇïÇñÇóÇòÇôÇöÅoÅbÅpÅP";
				if( val >= 0x20 && val <= 0x7e ) {
					TextOut( hdc,
						rectDraw.left+x*tm.tmMaxCharWidth,
						rectDraw.top+y*tm.tmHeight,
						&table[(val-0x20)*2], 2 );
				} else {
					str[0] = (BYTE)val;
					TextOut( hdc,
						rectDraw.left+x*tm.tmMaxCharWidth,
						rectDraw.top+y*tm.tmHeight,
						str, 1 );
				}
			}
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		wmPaint(hWnd,hdc);
		EndPaint(hWnd, &ps);
		break;

	case WM_KEYDOWN:
		lastVKey = (WORD)wParam;
		break;

	//case WM_CHAR:
	//	lastChar = (WORD)wParam;
	//	break;

	case WM_CLOSE:
		return 0;	// ï¬Ç∂ÇøÇ·ë ñ⁄

	case WM_DESTROY:
		hwnd = NULL;
		PostQuitMessage(0);
		break;

	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}

static int createTextInThread()
{
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.hIcon         = NULL;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "wonbe_win32text";
	RegisterClass(&wc);

	hwnd = CreateWindow(wc.lpszClassName, "WONBE TEXT", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, SCREEN_SIZE_X*16+64, SCREEN_SIZE_Y*20+64,
		NULL, NULL, wc.hInstance, NULL);

	if( !hwnd ) return FALSE;

	clearText();
	ShowWindow( hwnd, SW_SHOW );
	UpdateWindow( hwnd );

	return TRUE;
}

void clearText()
{
	int x,y;
	if( hwnd == NULL ) return;
	for( y=0; y<SCREEN_SIZE_Y; y++ ) {
		for( x=0; x<SCREEN_SIZE_X; x++ ) {
			screen[x][y] = ' ';
		}
	}
	InvalidateRect( hwnd, NULL, FALSE );
}

void drawChar( int x, int y, unsigned short ch )
{
	if( x < 0 ) return;
	if( y < 0 ) return;
	if( x >= SCREEN_SIZE_X ) return;
	if( y >= SCREEN_SIZE_Y ) return;
	screen[x][y] = ch;
	InvalidateRect( hwnd, NULL, FALSE );
}

void updateText()
{
	UpdateWindow( hwnd );
}

void deleteText()
{
	if( hwnd == NULL ) return;
	DestroyWindow( hwnd );
	hwnd = NULL;
}

DWORD WINAPI ThreadFunc( LPVOID lpParam ) 
{
	MSG msg;
	createTextInThread();
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
    return 0;
}

int createText()
{ 
    DWORD dwThreadId, dwThrdParam = 1; 
    HANDLE hThread; 

	//lastChar = 0;
	lastVKey = 0;
    hThread = CreateThread( 
        NULL,                        // no security attributes 
        0,                           // use default stack size  
        ThreadFunc,                  // thread function 
        &dwThrdParam,                // argument to thread function 
        0,                           // use default creation flags 
        &dwThreadId);                // returns the thread identifier 
	return TRUE;
}

unsigned short win32_wait()
{
	UINT t;
	while( TRUE ) {
		// à»â∫ÇÃ2çsÇÕñ{ìñÇÕÉ}ÉãÉ`ÉXÉåÉbÉhìIÇ…NGÇ»ÇÒÇæÇØÇ«
		// ÉeÉXÉgópä¬ã´ÇæÇ©ÇÁëÂñ⁄Ç…å©ÇÈ
		t = lastVKey;
		lastVKey = 0;
		switch( t ) {
		case 'P':
			return 0x02;	// START BUTTON
		case VK_SPACE:
			return 0x04;	//KEYWORD_SCAN_A;
		case VK_ESCAPE:
			return 0x08;	//KEYWORD_SCAN_B;
		case VK_UP:
		case 'E':
			return 0x10;	//KEYWORD_SCAN_X1;
		case VK_RIGHT:
		case 'D':
			return 0x20;	//KEYWORD_SCAN_X2;
		case VK_DOWN:
		case 'X':
			return 0x40;	//KEYWORD_SCAN_X3;
		case VK_LEFT:
		case 'S':
			return 0x80;	//KEYWORD_SCAN_X4;
		case 'T':
			return 0x100;	//KEYWORD_SCAN_Y1;
		case 'G':
			return 0x200;	//KEYWORD_SCAN_Y2;
		case 'V':
			return 0x400;	//KEYWORD_SCAN_Y3;
		case 'F':
			return 0x800;	//KEYWORD_SCAN_Y4;
		}
		Sleep( 100 );
	}
}

unsigned short win32_scan()
{
	WORD t;
	t = 0;
	if( GetAsyncKeyState('P') & 0x8000 ) {
		t |= 0x02;	// START
	}
	if( GetAsyncKeyState(VK_SPACE) & 0x8000 ) {
		t |= 0x04;	// A
	}
	if( GetAsyncKeyState(VK_ESCAPE) & 0x8000 ) {
		t |= 0x08;	// B
	}
	if( GetAsyncKeyState('E') & 0x8000 ) {
		t |= 0x10;	// X1
	}
	if( GetAsyncKeyState('D') & 0x8000 ) {
		t |= 0x20;	// X2
	}
	if( GetAsyncKeyState('X') & 0x8000 ) {
		t |= 0x40;	// X3
	}
	if( GetAsyncKeyState('S') & 0x8000 ) {
		t |= 0x80;	// X4
	}
	if( GetAsyncKeyState('T') & 0x8000 ) {
		t |= 0x100;	// Y1
	}
	if( GetAsyncKeyState('G') & 0x8000 ) {
		t |= 0x200;	// Y2
	}
	if( GetAsyncKeyState('V') & 0x8000 ) {
		t |= 0x400;	// Y3
	}
	if( GetAsyncKeyState('F') & 0x8000 ) {
		t |= 0x800;	// Y4
	}
	return t;
}

unsigned int win32_get_tick_count()
{
	return GetTickCount()/10;
}

void win32_sys_wait( unsigned int val )
{
	Sleep( val*10 );
}

// end of win32text.c
