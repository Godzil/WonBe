/* wonbe: WonderWitch Basic Environment */
/* First Created: Nov.3,2000 by Nashiko */
/* Copyright 2000 (c) by Pie Dey Co.,Ltd. */

/* This source code is for both VC++6.0 and Turbo C 2.0 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef WW
#include <sys/comm.h>
#include <sys/process.h>
#include <sys/fcntl.h>
#include <sys/bios.h>
#endif
#include "keywords.h"
#ifdef WIN32
#include "win32text.h"
#endif

char myVersion[] = "0.02";

#ifdef WW
#define MEMMOVE mymemmove
#define FAR far
#else
#define MEMMOVE mymemmove
#define FAR
#endif

#ifdef WIN32
typedef unsigned char BYTE;
#endif
typedef unsigned short WORD;
typedef signed short SHORT;

#define EOL (0x0d)

#ifdef WIN32
typedef int BOOL;
#endif
#define FALSE 0
#define TRUE 1

typedef void (*NEARPROC)();

#define WORKAREA_SIZE ((WORD)49152)
#define LINE_SIZE (256)

/* ���s�݂̂̃��[�h�œ����Ă���Ƃ� (�V���A���o�R�Œ[�������Ȃ��Ƃ�) */
BOOL runtimeOnly;

#ifdef WW
void fatalExit( const char * msg )
{
	text_screen_init();
	text_put_string( 0, 0, (char*)msg );
	text_put_string( 0, 3, "PUSH ANY BUTTON" );
	key_wait();
}
#endif

BOOL commonPrint( FILE FAR * fp, const char * msg,... )
{
#ifdef WW
	static
#endif
	char buf[256];
	vsprintf(buf, msg, (va_list)(&msg+1) ); 
	if( fp != NULL ) {
		int r;
		r = fprintf( fp, buf );
		return r >= 0;
	} else {
#ifdef WW
		if( runtimeOnly ) {
			fatalExit( buf );
		} else {
			comm_send_string( buf );
		}
#else
		printf( buf );
#endif
	}
	return TRUE;
}

void consoleInput( BYTE * buf, WORD len )
{
#ifdef WW
	WORD cursor;
	if( runtimeOnly ) {
		fatalExit("consoleInput not allowed in runtime mode");
	} else {
		cursor = 0;
		while( TRUE ) {
			int ch;
			ch = comm_receive_char();
			if( ch == 0x0d ) {	/* CR */
				buf[cursor] = '\0';
				return;
			}
			if( ch == 0x09 ) {	/* TAB */
				goto inputAsChar;
			}
			if( ch == 0x08 ) {	/* BS */
				if( cursor > 0 ) {
					cursor--;
					comm_send_char( ch );
					continue;
				}
			}
			if( ch >= 0 && ch < ' ' ) {
				/* ignore it */
				continue;
			}
			if( ch >= ' ' && ch <= 0xfc ) {
inputAsChar:
				if( cursor < len-1 ) {
					buf[cursor] = ch;
					cursor++;
					comm_send_char( ch );
				}
			}
		}
	}
#else
	gets(buf);
#endif
}

typedef struct {
	WORD id;
	const BYTE * name;
} KEYWORDITEM;

KEYWORDITEM keywords[] = {
	{ KEYWORD_IF,"if" },
	{ KEYWORD_PRINT,"print" },
	{ KEYWORD_LOCATE,"locate" },
	{ KEYWORD_CLS,"cls" },
	{ KEYWORD_GOTO,"goto" },
	{ KEYWORD_GOSUB,"gosub" },
	{ KEYWORD_RETURN,"return" },
	{ KEYWORD_FOR,"for" },
	{ KEYWORD_NEXT,"next" },
	{ KEYWORD_END,"end" },
	{ KEYWORD_BREAK,"break" },
	{ KEYWORD_REM,"rem" },
	{ KEYWORD_NEW,"new" },
	{ KEYWORD_LIST,"list" },
	{ KEYWORD_RUN,"run" },
	{ KEYWORD_CONT,"cont" },
	{ KEYWORD_SAVE,"save" },
	{ KEYWORD_LOAD,"load" },
	{ KEYWORD_MERGE,"merge" },
	{ KEYWORD_RANDOMIZE,"randomize" },
	{ KEYWORD_EXIT,"exit" },
	{ KEYWORD_DEBUG,"debug" },
	{ KEYWORD_WAITVB,"waitvb" },
	{ KEYWORD_FILES,"files" },
	{ KEYWORD_AND,"and" },
	{ KEYWORD_OR,"or" },
	{ KEYWORD_XOR,"xor" },
	{ KEYWORD_NOT,"not" },
	{ KEYWORD_WAIT,"wait" },
	{ KEYWORD_RND,"rnd" },
	{ KEYWORD_ABS,"abs" },
	{ KEYWORD_TICK,"tick" },
	{ KEYWORD_SCAN_A,"scan_a" },
	{ KEYWORD_SCAN_B,"scan_b" },
	{ KEYWORD_SCAN_X1,"scan_x1" },
	{ KEYWORD_SCAN_X2,"scan_x2" },
	{ KEYWORD_SCAN_X3,"scan_x3" },
	{ KEYWORD_SCAN_X4,"scan_x4" },
	{ KEYWORD_SCAN_Y1,"scan_y1" },
	{ KEYWORD_SCAN_Y2,"scan_y2" },
	{ KEYWORD_SCAN_Y3,"scan_y3" },
	{ KEYWORD_SCAN_Y4,"scan_y4" },
	{ KEYWORD_SCAN,"scan" },	/* scan��scna_X����ɂȂ���΂Ȃ�Ȃ� */
	{ KEYWORD_THEN,"then" },
	{ KEYWORD_CHR,"chr" },
	{ KEYWORD_TO,"to" },
	{ KEYWORD_STEP,"step" },
	{ 0,NULL }
};

/* �C���^���N�e�B�u���[�h����1�s�o�b�t�@ */
BYTE waRawLine[LINE_SIZE];
BYTE waCoockedLine[LINE_SIZE];

/* �R�[�h�ƃf�[�^�����߂�̈� */
WORD codeTop;
WORD dataTop;
/*WORD savedPointer;		/* break�����Ƃ��̎��s���ʒu */
BYTE wa[WORKAREA_SIZE];

/* �O���[�o���ϐ��̈� */
#define NUMBER_OF_SIMPLE_VARIABLES (26)
SHORT globalVariables[NUMBER_OF_SIMPLE_VARIABLES];
SHORT topLevelVariables[NUMBER_OF_SIMPLE_VARIABLES];
SHORT * localVariables;	/* ����X�R�[�v�ŗL���ȃ��[�J���ϐ��̈������ */

/* ���ݎ��s���̈ʒu�Ɋւ����� */
/* wa��waCookedLine�̂ǂ��炩�̒���������NULL */
BYTE * executionPointer;
/* cont�R�}���h�Ŏ��s���ĊJ����|�C���g */
BYTE * resumePointer;

/* ���ݏ������̍s�ԍ� (0�Ȃ�_�C���N�g���[�h) */
WORD currentLineNumber;

/* gosub��for�̂��߂̃X�^�b�N */
#define STACK_TYPE_GOSUB 1
#define STACK_TYPE_FOR 2
typedef struct {
	WORD type;	/* 1==GOSUB, 2==FOR */
	/* for GOSUB and FOR */
	BYTE * returnPointer;
	/* for GOSUB */
	SHORT * lastLocalVariables;
	SHORT simpleVariables[NUMBER_OF_SIMPLE_VARIABLES];
	/* for FOR */
	SHORT * pvar;	/* counter variable */
	SHORT limit;	/* limit value */
	SHORT step;		/* step value */
} STACK;
#define STACK_MAX 8
STACK stacks[STACK_MAX];
int stackPointer;

/* �C���^���N�e�B�u���[�h���C���^�v���^���[�h�� */
BOOL bInteractive;

/* �C���^�v���^�E�o��̐U�镑���̃��N�G�X�g */
BOOL bForceToReturnSuper;

int requestNextAction;	/* �ȉ��̂����ꂩ */
/* ������������̂܂� */
#define REQUEST_NO_ACTION 0
/* �Θb���[�h�֍s�� */
#define REQUEST_INTERACTIVE 1
/* runLineNumber������s���J�n���� */
#define REQUEST_RUN_LINENUMBER 2
/* waRawLine�ɓ����Ă��镶������t�@�C�����Ƃ��Ă����ǂݍ���Ŏ��s���� */
#define REQUEST_RUN_FILE 3
/* new���� */
#define REQUEST_NEW 4
/* waRawLine�ɓ����Ă��镶������t�@�C�����Ƃ��Ă�������[�h */
#define REQUEST_LOAD_FILE 5
/* WONBE���I������OS�ɖ߂� */
#define REQUEST_EXIT_SYSTEM 6
/* waRawLine�ɓ����Ă��镶������t�@�C�����Ƃ��Ă�����}�[�W */
#define REQUEST_MERGE_FILE 7
/* REQUEST_RUN_LINENUMBER�Ŏ��s�J�n�����N�G�X�g����s�ԍ� */
WORD runLineNumber;

/* �G���[���� */
void syntaxError()
{
	commonPrint(NULL, "Syntax Error in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void divideByZero()
{
	commonPrint(NULL, "Divide by 0 in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void outOfArraySubscription()
{
	commonPrint(NULL, "Out of Array Subscription in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void lineNumberNotFound( WORD lineNumber )
{
	commonPrint(NULL, "Line Number %d not Found in %d\x07\n", lineNumber, currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void stackOverflow()
{
	commonPrint(NULL, "Stack Overflow in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void stackUnderflow()
{
	commonPrint(NULL, "Stack Underflow in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void nextWithoutFor()
{
	commonPrint(NULL, "Next without For in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void outOfMemory()
{
	commonPrint(NULL, "Out of memory in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void loadError()
{
	commonPrint(NULL, "File Not Found or Load Error in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void cantContinue()
{
	commonPrint(NULL, "Can't Continue in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void saveError()
{
	commonPrint(NULL, "Save Erroor in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

void paramError()
{
	commonPrint(NULL, "Parameter Erroor in %d\x07\n", currentLineNumber );
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

/* ���[�e�B���e�B�֐��B */
void mymemmove( BYTE * dst, const BYTE * src, WORD len )
{
	WORD i;
	if( dst < src ) {
		for( i=0; i<len; i++ ) {
			*dst++ = *src++;
		}
	} else {
		dst += len;
		src += len;
		for( i=0; i<len; i++ ) {
			*--dst = *--src;
		}
	}
}

BYTE * skipToEOL( BYTE * p )
{
	while( TRUE ) {
		if( *p == EOL ) return p;
		switch( *p++ ) {
		case 0x01:	/*		���ɂ���̂�10�i2�o�C�g���� */
		case 0x02:	/*		���ɂ���̂�16�i2�o�C�g���� */
			p += 2;
			break;
		case 0x03:	/*		���ɂ���͉̂ϒ������� 0x00�Ń^�[�~�l�[�g */
			{
				while( TRUE ) {
					if( *p++ == '\0' ) break;
				}
			}
			break;
		}
	}
}

BYTE mytolower( BYTE ch )
{
	if( ch >= 'A' && ch <= 'Z' ) {
		return ch - 'A' + 'a';
	}
	return ch;
}

/* �V�t�gJIS��1�o�C�g�ڂ��ǂ������f�B�}�N���Ȃ̂ŕ���p�ɒ��� */
#define IsDBCSLeadByte(ch) ((ch >= 0x81 && ch <= 0x9f) || (ch >= 0xe0 && ch <= 0xfc))

/* ���s�����̃N���A (cont�ł��Ȃ��Ȃ�) */
void clearRuntimeInfo()
{
	memset( globalVariables, 0, sizeof(globalVariables) );
	memset( topLevelVariables, 0, sizeof(topLevelVariables) );
	memset( stacks, 0, sizeof(globalVariables) );
	executionPointer = NULL;
	resumePointer = NULL;
	currentLineNumber = 0;
	localVariables = topLevelVariables;
	stackPointer = 0;
	srand(0);
}

/* �s���̍s�ԍ��́A���s���̒�^���� */
void processLineHeader()
{
	if( !bInteractive ) {
		currentLineNumber = *((WORD *)executionPointer);
		executionPointer += 2;
	} else {
		currentLineNumber = 0;
	}
}

/* �s�ԍ����� */
BYTE * getLineReferenceFromLineNumber( WORD lineNumber )
{
	BYTE * p;
	WORD t;
	p = wa;
	while( TRUE ) {
		t = *((WORD *)p);
		if( t == 0 ) {
			return NULL;
		}
		if( lineNumber == t ) {
			return p;
		}
		p += 2;
		p = skipToEOL( p );
		p++;
	}
}

BYTE * getInsertionPointFromLineNumber( WORD lineNumber )
{
	BYTE * p;
	WORD t;
	p = wa;
	while( TRUE ) {
		t = *((WORD *)p);
		if( t == 0 ) {
			return p;
		}
		if( lineNumber <= t ) {
			return p;
		}
		p += 2;
		p = skipToEOL( p );
		p++;
	}
}

/* �z��Ǘ� */
int availableArrayItems()
{
	return (WORKAREA_SIZE-dataTop) >> 1;
}

SHORT * derefArrayItem( int index )
{
	if( index < 0 || index >= availableArrayItems() ) {
		outOfArraySubscription();
		return NULL;	/* ���̃C���f�b�N�X�͎g���܂��� */
	}
	return (SHORT*)(&wa[dataTop+index*2]);
}

/* �����e�L�X�g�̊Ǘ� */
#define SCREEN_SIZE_X (224/8)
#define SCREEN_SIZE_Y (144/8)

WORD screen[SCREEN_SIZE_X][SCREEN_SIZE_Y];

SHORT xCursor, yCursor;

#define TABSTOP 8

void ptextScrollUp()
{
	int x, y;
	for( y=0; y<SCREEN_SIZE_Y-1; y++ ) {
		for( x=0; x<SCREEN_SIZE_X; x++ ) {
			screen[x][y] = screen[x][y+1];
		}
	}
	for( x=0; x<SCREEN_SIZE_X; x++ ) {
		screen[x][SCREEN_SIZE_Y-1] = ' ';
	}
	for( y=0; y<SCREEN_SIZE_Y; y++ ) {
		for( x=0; x<SCREEN_SIZE_X; x++ ) {
#ifdef WIN32
			drawChar( x, y, screen[x][y] );
#endif
#ifdef WW
			text_put_char( x, y, screen[x][y] );
#endif
		}
	}
}

void ptextNewline()
{
	xCursor = 0;
	yCursor++;
	if( yCursor >= SCREEN_SIZE_Y ) {
		ptextScrollUp();
		yCursor = SCREEN_SIZE_Y-1;
	}
}

void ptextTab()
{
	if( xCursor+TABSTOP >= SCREEN_SIZE_X ) {
		ptextNewline();
		return;
	}
	xCursor = (xCursor+TABSTOP) / TABSTOP * TABSTOP;
}

void ptextString( const BYTE * msg )
{
	while( TRUE ) {
		WORD ch;
		if( *msg == '\0' ) break;
		if( IsDBCSLeadByte(*msg) ) {
			ch = (msg[0] << 8) + msg[1];
			msg+=2;
		} else {
			ch = msg[0];
			msg++;
		}
		screen[xCursor][yCursor] = ch;
#ifdef WIN32
		drawChar( xCursor, yCursor, ch );
#endif
#ifdef WW
		text_put_char( xCursor, yCursor, ch );
#endif
		xCursor++;
		if( xCursor >= SCREEN_SIZE_X ) {
			xCursor = 0;
			yCursor++;
			if( yCursor >= SCREEN_SIZE_Y ) {
				ptextScrollUp();
				yCursor = SCREEN_SIZE_Y-1;
			}
		}
	}
}

void ptextLocate( int x, int y )
{
	if( x < 0 ) x = 0;
	if( x >= SCREEN_SIZE_X ) x = SCREEN_SIZE_X-1;
	if( y < 0 ) y = 0;
	if( y >= SCREEN_SIZE_Y ) y = SCREEN_SIZE_Y-1;
	xCursor = x;
	yCursor = y;
}

void ptextCLS()
{
	int x,y;
	xCursor = 0;
	yCursor = 0;
#ifdef WIN32
	clearText();
#endif
#ifdef WW
	for( y=0; y<SCREEN_SIZE_Y; y++ ) {
		text_fill_char(0, y, SCREEN_SIZE_X, ' ' ); 
	}
#endif
	for( y=0; y<SCREEN_SIZE_Y; y++ ) {
		for( x=0; x<SCREEN_SIZE_X; x++ ) {
			screen[x][y] = ' ';
		}
	}
}

/* ���ԃR�[�h����\�[�X�e�L�X�g�ɖ߂��Ȃ���o��(list��save�Ŏg��) */
BOOL sourceDump( FILE FAR * fp, WORD from, WORD to )
{
	BYTE * p;
	BOOL b;
	p = getInsertionPointFromLineNumber( from );
	while( TRUE ) {
		WORD lineNumber;
		lineNumber = *((WORD*)p);
		if( lineNumber == 0 ) break;
		if( lineNumber > to ) break;
		p += 2;
		b = commonPrint(fp,"%d",lineNumber);
		if( b == FALSE ) return FALSE;
		while( TRUE ) {
			if( *p == EOL ) {
				p++;
				break;
			}
			if( *p == 0x01 ) {
				WORD n;
				p++;
				n = *((WORD*)p);
				p += 2;
				b = commonPrint(fp,"%d",n);
				if( b == FALSE ) return FALSE;
			} else if( *p == 0x02 ) {
				WORD n;
				p++;
				n = *((WORD*)p);
				p += 2;
				b = commonPrint(fp,"0x%x",n);
				if( b == FALSE ) return FALSE;
			} else if( *p == 0x03 ) {
				p++;
				b = commonPrint(fp,"\"%s\"",p);
				if( b == FALSE ) return FALSE;
				p += strlen(p)+1;
			} else if( *p == '\'' ) {	/* �R�����g�̂��Ƃ͂��̂܂܍s���܂œ]�� */
				while( TRUE ) {
					if( *p == EOL ) break;
					b = commonPrint(fp,"%c",*p);
					if( b == FALSE ) return FALSE;
					p++;
				}
			} else if( *p >= 0x80 ) {
				KEYWORDITEM * k = keywords;
				while( TRUE ) {
					if( k->id == 0 ) {
						b = commonPrint(fp,"[?%x?]", *p );
						if( b == FALSE ) return FALSE;
						p++;
						break;
					}
					if( k->id == *p ) {
						b = commonPrint(fp, k->name );
						if( b == FALSE ) return FALSE;
						p++;
						if( k->id == KEYWORD_REM ) {	/* �R�����g�̂��Ƃ͂��̂܂܍s���܂œ]�� */
							while( TRUE ) {
								if( *p == EOL ) break;
								b = commonPrint(fp,"%c",*p);
								if( b == FALSE ) return FALSE;
								p++;
							}
						}
						break;
					}
					k++;
				}
			} else {
				b = commonPrint(fp,"%c",*p);
				if( b == FALSE ) return FALSE;
				p++;
			}
		}
		b = commonPrint(fp,"\n");
		if( b == FALSE ) return FALSE;
	}
	return TRUE;
}

/* ���v�Z�@�\ */
SHORT expr();	/* forward decl */

SHORT * getArrayReference()
{
	BYTE ch;
	int index;
	SHORT * pvar;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch != '(' && ch != '[' ) {
		syntaxError();
		return NULL;
	}
	index = expr();
	if( bForceToReturnSuper ) return NULL;
	pvar = derefArrayItem( index );
	if( pvar == NULL ) return NULL;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch != ')' && ch != ']' ) {
		syntaxError();
		return NULL;
	}
	return pvar;
}

SHORT calcValue()
{
	BYTE ch;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch >= 'A' && ch <= 'Z' ) {
		return globalVariables[ch-'A'];
	}
	if( ch >= 'a' && ch <= 'z' ) {
		return localVariables[ch-'a'];
	}
	if( ch == '@' ) {
		SHORT * pvar;
		pvar = getArrayReference();
		if( pvar == NULL ) return -1;
		return *pvar;
	}
	switch( ch ) {
	case 0x01:	/*		���ɂ���̂�10�i2�o�C�g���� */
	case 0x02:	/*		���ɂ���̂�16�i2�o�C�g���� */
		{
			SHORT t;
			t = *((SHORT *)executionPointer);
			executionPointer += 2;
			return t;
		}
	case '(':
		{
			SHORT t;
			t = expr();
			while( TRUE ) {
				ch = *executionPointer++;
				if( ch != ' ' && ch != '\t' ) break;
			}
			if( ch != ')' ) {
				syntaxError();
				return -1;
			}
			return t;
		}
	case '-':
		return -calcValue();
	case KEYWORD_NOT:
		return ~calcValue();
	case KEYWORD_SCAN:
#ifdef WW
		return key_press_check();
#else
		return win32_scan();
#endif
	case KEYWORD_WAIT:
#ifdef WW
		return key_wait();
#else
		return win32_wait();
#endif
	case KEYWORD_RND:
		{
			SHORT t;
			t = calcValue();
			return rand() % t;
		}
	case KEYWORD_ABS:
		{
			SHORT t;
			t = calcValue();
			if( t < 0 ) return -t;
			return t;
		}
	case KEYWORD_TICK:
#ifdef WW
		return sys_get_tick_count();
#else
		return win32_get_tick_count();
#endif
	case KEYWORD_SCAN_A:
		return 4;
	case KEYWORD_SCAN_B:
		return 8;
	case KEYWORD_SCAN_X1:
		return 16;
	case KEYWORD_SCAN_X2:
		return 32;
	case KEYWORD_SCAN_X3:
		return 64;
	case KEYWORD_SCAN_X4:
		return 128;
	case KEYWORD_SCAN_Y1:
		return 256;
	case KEYWORD_SCAN_Y2:
		return 512;
	case KEYWORD_SCAN_Y3:
		return 1024;
	case KEYWORD_SCAN_Y4:
		return 2048;
	}
	syntaxError();
	return -1;
}

SHORT expr()
{
	SHORT acc;
	acc = calcValue();
	if( bForceToReturnSuper ) return -1;

	while( TRUE ) {
		BYTE ch;
		while( TRUE ) {
			ch = *executionPointer++;
			if( ch != ' ' && ch != '\t' ) break;
		}
		switch( ch ) {
		case '+':
			acc = acc + calcValue();
			break;
		case '-':
			acc = acc - calcValue();
			break;
		case '*':
			acc = acc * calcValue();
			break;
		case '/':
			{
				SHORT t;
				t = calcValue();
				if( t == 0 ) {
					divideByZero();
				} else {
					acc = acc / t;
				}
			}
			break;
		case KEYWORD_AND:
			acc = acc & calcValue();
			break;
		case KEYWORD_OR:
			acc = acc | calcValue();
			break;
		case KEYWORD_XOR:
			acc = acc ^ calcValue();
			break;
		case '>':
			{
				BYTE ch2;
				while( TRUE ) {
					ch2 = *executionPointer++;
					if( ch2 != ' ' && ch2 != '\t' ) break;
				}
				if( ch2 == '=' ) {
					acc = (acc >= calcValue());
				} else {
					executionPointer--;
					acc = (acc > calcValue());
				}
			}
			break;
		case '<':
			{
				BYTE ch2;
				while( TRUE ) {
					ch2 = *executionPointer++;
					if( ch2 != ' ' && ch2 != '\t' ) break;
				}
				if( ch2 == '=' ) {
					acc = (acc <= calcValue());
				} else if( ch2 == '>' ) {
					acc = (acc != calcValue());
				} else {
					executionPointer--;
					acc = (acc < calcValue());
				}
			}
			break;
		case '=':
			acc = (acc == calcValue());
			break;
		default:
			executionPointer--;		/* unget it */
			return acc;
		}
		if( bForceToReturnSuper ) return -1;
	}
}

/* �e�X�e�[�g�����g���s�������C�� */

void st_assignment( SHORT * pvar )	/* ����X�e�[�g�����g������O�I�ɏ������� */
{
	BYTE ch;
	SHORT val;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch != '=' ) {
		syntaxError();
		return;
	}
	val = expr();
	if( bForceToReturnSuper ) return;
	*pvar = val;
}

void st_if()
{
	SHORT val;
	BYTE ch;
	val = expr();
	if( bForceToReturnSuper ) return;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch != KEYWORD_THEN ) {
		syntaxError();
		return;
	}
	if( val != 0 ) {
		return;	/* then�̎�����p�����s���� */
	}
	/* �����s�����ɂ��A�s���܂œǂݔ�΂� */
	executionPointer = skipToEOL( executionPointer );
}

void printOrDebug( BOOL bPrint )
{
	WORD lastChar;
	BYTE ch;
	lastChar = '\0';
	while( TRUE ) {
		while( TRUE ) {
			ch = *executionPointer++;
			if( ch != ' ' && ch != '\t' ) break;
		}
		if( ch == EOL || ch == ':' || ch == '\'' ) {
			executionPointer--;	/* unget it */
			break;
		}
		lastChar = ch;
		switch( ch ) {
		case 0x03:
			if( bPrint ) {
				ptextString( executionPointer );
			} else {
				commonPrint( NULL, executionPointer );
			}
			while( TRUE ) {
				ch = *executionPointer++;
				if( ch == '\0' ) break;
			}
			break;
		case ';':
			break;
		case ',':
			if( bPrint ) {
				ptextTab();
			} else {
				commonPrint( NULL, "\t" );
			}
			break;
		case KEYWORD_CHR:
			{
				WORD val;
#ifdef WW
				static
#endif
				BYTE str[3];
				val = expr();
				if( bForceToReturnSuper ) return;
				if( val >= 0x100 ) {
					str[0] = (val >> 8);
					str[1] = (val & 0xff);
					str[2] = '\0';
				} else {
					str[0] = (BYTE)val;
					str[1] = '\0';
				}
				if( bPrint ) {
					ptextString( str );
				} else {
					commonPrint( NULL, str );
				}
			}
			break;
		default:
			{
				WORD val;
#ifdef WW
				static
#endif
				BYTE str[10];
				executionPointer--;	/* unget it */
				val = expr();
				if( bForceToReturnSuper ) return;
				sprintf( str, "%d", val );
				if( bPrint ) {
					ptextString( str );
				} else {
					commonPrint( NULL, str );
				}
			}
			break;
		}
	}
	if( lastChar != ';' && lastChar != ',' ) {
		if( bPrint ) {
			ptextNewline();
		} else {
			commonPrint( NULL, "\r\n" );
		}
	}
}

void st_print()
{
	printOrDebug( TRUE );
}

void st_debug()
{
	printOrDebug( FALSE );
}

void st_locate()
{
	SHORT x, y;
	BYTE ch;
	x = expr();
	if( bForceToReturnSuper ) return;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch != ',' ) {
		syntaxError();
		return;
	}
	y = expr();
	if( bForceToReturnSuper ) return;
	ptextLocate( x, y );
}

void st_cls()
{
	ptextCLS();
}

void st_goto()
{
	SHORT val;
	BYTE * t;
	val = expr();
	if( bForceToReturnSuper ) return;
	t = getLineReferenceFromLineNumber( val );
	if( t == NULL ) {
		lineNumberNotFound( val );
		return;
	}
	executionPointer = t;
	bInteractive = FALSE;
	processLineHeader();
}

void st_gosub()
{
	SHORT val;
	BYTE * t;
	val = expr();
	if( bForceToReturnSuper ) return;
	t = getLineReferenceFromLineNumber( val );
	if( t == NULL ) {
		lineNumberNotFound( val );
		return;
	}
	if( stackPointer+1 >= STACK_MAX ) {
		stackOverflow();
		return;
	}
	stacks[stackPointer].type = STACK_TYPE_GOSUB;
	stacks[stackPointer].returnPointer = executionPointer;
	stacks[stackPointer].lastLocalVariables = localVariables;
	localVariables = stacks[stackPointer].simpleVariables;
	stackPointer++;
	executionPointer = t;
	bInteractive = FALSE;
	processLineHeader();
}

void st_return()
{
	while( TRUE ) {
		if( stackPointer == 0 ) {
			stackUnderflow();
			return;
		}
		stackPointer--;
		if( stacks[stackPointer].type == STACK_TYPE_GOSUB ) break;
	}
	executionPointer = stacks[stackPointer].returnPointer;
	if( executionPointer >= waCoockedLine && executionPointer < waCoockedLine+LINE_SIZE ) {
		bInteractive = TRUE;
	}
	localVariables = stacks[stackPointer].lastLocalVariables;
}

void st_for()
{
	BYTE ch;
	SHORT from, to, step;
	SHORT * pvar;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch == '@' ) {	/* is it l-value? */
		pvar = getArrayReference();
		if( pvar == NULL ) return;
	} else if( ch >= 'A' && ch <= 'Z' ) {
		pvar = &globalVariables[ch-'A'];
	} else if( ch >= 'a' && ch <= 'z' ) {
		pvar = &localVariables[ch-'a'];
	} else {
		syntaxError();
		return;
	}
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch != '=' ) {
		syntaxError();
		return;
	}
	from = expr();
	if( bForceToReturnSuper ) return;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch != KEYWORD_TO ) {
		syntaxError();
		return;
	}
	to = expr();
	if( bForceToReturnSuper ) return;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch == KEYWORD_STEP ) {
		step = expr();
		if( bForceToReturnSuper ) return;
	} else {
		step = 1;
		executionPointer--;	/* unget it */
	}

	if( stackPointer+1 >= STACK_MAX ) {
		stackOverflow();
		return;
	}
	stacks[stackPointer].type = STACK_TYPE_FOR;
	stacks[stackPointer].returnPointer = executionPointer;
	*pvar = from;
	stacks[stackPointer].pvar = pvar;
	stacks[stackPointer].limit = to;
	stacks[stackPointer].step = step;
	stackPointer++;
}

void st_next()
{
	if( stackPointer == 0 ) {
		nextWithoutFor();
		return;
	}
	if( stacks[stackPointer-1].type != STACK_TYPE_FOR ) {
		nextWithoutFor();
		return;
	}
	if( stacks[stackPointer-1].limit == *(stacks[stackPointer-1].pvar) ) {
		/* loop done */
		stackPointer--;
		return;
	}
	/* count step and loop again */
	*(stacks[stackPointer-1].pvar) += stacks[stackPointer-1].step;
	executionPointer = stacks[stackPointer-1].returnPointer;
}

/* end�X�e�[�g�����g:�@����ȏI�� */
void st_end()
{
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
}

/* break�X�e�[�g�����g:�@�f�o�b�O�p�̒��f */
void st_break()
{
	commonPrint(NULL, "Break in %d\x07\n", currentLineNumber);
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_INTERACTIVE;
	resumePointer = executionPointer;	/* cont����ꏊ��break�̎��Bbreak�݂̗̂�O���� */
}

void st_rem()
{
	executionPointer = skipToEOL( executionPointer );
}

void st_new()
{
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_NEW;
}

void st_list()
{
	WORD from, to;
	BYTE ch;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch == ':' || ch == EOL || ch == '\'' ) {
		executionPointer--;	/* unget */
		from = 1;
		to = 32767;
	} else {
		if( ch == 0x01 ) {
			from = *((WORD*)executionPointer);
			executionPointer += 2;
			while( TRUE ) {
				ch = *executionPointer++;
				if( ch != ' ' && ch != '\t' ) break;
			}
		} else {
			from = 1;
		}
		if( ch != '-' ) {
			executionPointer--;	/* unget */
			to = from;
		} else {
			while( TRUE ) {
				ch = *executionPointer++;
				if( ch != ' ' && ch != '\t' ) break;
			}
			if( ch == 0x01 ) {
				to = *((WORD*)executionPointer);
				executionPointer += 2;
			} else {
				executionPointer--;	/* unget */
				to = 32767;
			}
		}
	}
	sourceDump( NULL, from, to );	/* ���X�g�o�̖͂{�̂��Ă� */
}

void st_run()
{
	BYTE ch;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch == ':' || ch == EOL || ch == '\'' ) {
		bForceToReturnSuper = TRUE;
		requestNextAction = REQUEST_RUN_LINENUMBER;
		runLineNumber = 0;
	} else if( ch == 0x03 ) {
		strcpy( waRawLine, executionPointer );
		bForceToReturnSuper = TRUE;
		requestNextAction = REQUEST_RUN_FILE;
	} else {
		SHORT val;
		executionPointer--;	/* unget it */
		val = expr();
		if( bForceToReturnSuper ) return;
		bForceToReturnSuper = TRUE;
		requestNextAction = REQUEST_RUN_LINENUMBER;
		runLineNumber = val;
	}
}

void st_cont()
{
	if( resumePointer == NULL ) {
		cantContinue();
		return;
	}
	executionPointer = resumePointer;
	bInteractive = FALSE;
}

void st_save()
{
	BYTE ch;
	BOOL b;
	FILE FAR * fp;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch != 0x03 ) {
		syntaxError();
		return;
	}
	fp = fopen( executionPointer, "w" );
	if( fp == NULL ) {
		saveError();
		return;
	}
	b = sourceDump( fp, 1, 32767 );
	fclose( fp );
	if( b == FALSE ) {
		saveError();
		remove( executionPointer );
		return;
	}

	executionPointer += strlen(executionPointer)+1;
}

void st_load()
{
	BYTE ch;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch == 0x03 ) {
		strcpy( waRawLine, executionPointer );
		bForceToReturnSuper = TRUE;
		requestNextAction = REQUEST_LOAD_FILE;
	} else {
		syntaxError();
		return;
	}
}

void st_merge()
{
	BYTE ch;
	while( TRUE ) {
		ch = *executionPointer++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch == 0x03 ) {
		strcpy( waRawLine, executionPointer );
		bForceToReturnSuper = TRUE;
		requestNextAction = REQUEST_MERGE_FILE;
	} else {
		syntaxError();
		return;
	}
}

void st_randomize()
{
	SHORT val;
	val = expr();
	srand( val );
}

void st_exit()
{
	bForceToReturnSuper = TRUE;
	requestNextAction = REQUEST_EXIT_SYSTEM;
}

void st_waitvb()
{
	SHORT val;
	val = expr();
	if( bForceToReturnSuper ) return;
	if( val < 0 || val > 750 ) {
		paramError();
		return;
	}
#ifdef WW
	sys_wait( val ); 
#endif
#ifdef WIN32
	win32_sys_wait( val );
#endif
}

void st_files()
{
#ifdef WIN32
	commonPrint( NULL, "files statement not implemented in Win32\x07\n" );
#endif
#ifdef WW
	int i;
	struct stat statbuf;
	int nent;

	/*chdir("/rom0");*/
	nent = nument(NULL);

	for (i = 0; i < nent; i++) {
		int result = getent(NULL, i, &statbuf);
		if (result == E_FS_SUCCESS) {
			if (statbuf.count != -1) {
				/* Freya OS�̃o�O?�@����R�[�h */
				int p, q;
				p = 0;
				q = 0;
				waRawLine[p++] = '"';
				while( TRUE ) {
					if( statbuf.name[q] == '\0' ) break;
					waRawLine[p++] = statbuf.name[q++];
				}
				waRawLine[p++] = '"';
				waRawLine[p++] = ',';
				q = 0;
				while( TRUE ) {
					if( statbuf.info[q] == '\0' ) break;
					waRawLine[p++] = statbuf.info[q++];
				}
				waRawLine[p++] = '\0';
				commonPrint( NULL, "%s,%ld,%d\n", waRawLine, statbuf.len, statbuf.count );
			}
		} else {
			commonPrint( NULL, "*File Access Error\x07\n" );
		}
	}
#endif
}

/* �s�G�f�B�^ */
void editLine()
{
	WORD wishLineNumber;
	BYTE ch, * p, * target;
	/* �g���b�N�ɒ���! */
	/* �s���̐������A�s�ԍ��ɕϊ�����ɂ͌��ʓI�ɐ擪��0x01����苎�邾���ŗǂ� */
	MEMMOVE( waCoockedLine, waCoockedLine+1, LINE_SIZE-1 );
	/* �g���b�N�I���B���̎��_��wsCoockedLine�́A�v���O���������\���̌`���Ɉ�v���Ă��� */
	wishLineNumber = *((WORD*)waCoockedLine);
	p = waCoockedLine+2;
	target = getLineReferenceFromLineNumber( wishLineNumber );
	while( TRUE ) {
		ch = *p++;
		if( ch != ' ' && ch != '\t' ) break;
	}
	if( ch == EOL ) {
		WORD delta;
		BYTE * from;
		/* removing the line */
		if( target == NULL ) {
			lineNumberNotFound( wishLineNumber );
			return;
		}
		from = skipToEOL(target)+1;
		delta = from-target;
		MEMMOVE( target, from, (WORD)(dataTop-(from-wa)) );
		dataTop -= delta;
		clearRuntimeInfo();
	} else {
		WORD len;
		len = skipToEOL(waCoockedLine+2)-waCoockedLine+1;
		if( target == NULL ) {
			/* insert new line */
			/*const BYTE * to;*/
			if( dataTop + len >= WORKAREA_SIZE ) {
				outOfMemory();
				return;
			}
			target = getInsertionPointFromLineNumber( wishLineNumber );
			/* to = skipToEOL(target)+1; */
			MEMMOVE( target+len, target, (WORD)(dataTop-(target-wa)) );
			MEMMOVE( target, waCoockedLine, len );
			dataTop += len;
			clearRuntimeInfo();
		} else {
			/* replace line */
			WORD lost;
			BYTE * nextline;
			int delta;
			nextline = skipToEOL(target)+1;
			lost = nextline-target;
			delta = len-lost;
			if( dataTop + delta >= WORKAREA_SIZE ) {
				outOfMemory();
				return;
			}
			MEMMOVE( nextline+delta, nextline, (WORD)(dataTop-(nextline-wa)) );
			MEMMOVE( target, waCoockedLine, len );
			dataTop += delta;
			clearRuntimeInfo();
		}
	}
}

/* ���Ԍ���ɖ|�󂷂� */
BOOL convertInternalCode( BYTE * waCoockedLine, const BYTE * waRawLine )
{
	const BYTE * src = waRawLine;
	BYTE * dst = waCoockedLine;
	while( TRUE ) {
		if( *src == '\0' ) break;
		if( *src == ' ' || *src == '\t' ) {
			*dst++ = *src++;
		} else if( *src < 0x20 ) {
			syntaxError();
			return FALSE;
		} else if( *src >= '0' && *src <= '9' ) {
			if( *src == '0' && *(src+1) == 'x' ) {
				WORD acc;
				/* 16�i�̂Ƃ� */
				*dst++ = 0x02;
				acc = 0;
				src += 2;
				while( TRUE ) {
					if( *src >= '0' && *src <= '9' ) {
						acc *= 16;
						acc += *src - '0';
					} else if( *src >= 'a' && *src <= 'f' ) {
						acc *= 16;
						acc += *src - 'a' + 10;
					} else if( *src >= 'A' && *src <= 'F' ) {
						acc *= 16;
						acc += *src - 'A' + 10;
					} else {
						break;
					}
					src++;
				}
				*((WORD*)dst) = acc;
				dst += 2;
			} else {
				SHORT acc;
				/* 10�i�̂Ƃ� */
				*dst++ = 0x01;
				acc = *src-'0';
				src++;
				while( TRUE ) {
					if( *src < '0' || *src > '9' ) break;
					acc *= 10;
					acc += *src - '0';
					src++;
				}
				if( acc < 0 ) {	/* overflow case */
					syntaxError();
					return FALSE;
				}
				*((SHORT*)dst) = acc;
				dst += 2;
			}
		} else if( (*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') ) {
			BYTE next = *(src+1);
			if( (next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z') ) {
				WORD id;
				KEYWORDITEM * p = keywords;
				id = 0;
				while( TRUE ) {
					const BYTE * ps, * pd;
					if( p->id == 0 ) {
						syntaxError();
						return FALSE;
					}
					ps = src;
					pd = p->name;
					while( TRUE ) {
						if( *pd == '\0' ) {
							id = p->id;
							src = ps;
							break;
						}
						if( mytolower(*ps) != mytolower(*pd) ) break;
						ps++;
						pd++;
					}
					if( id != 0 ) break;
					p++;
				}
				*dst++ = (BYTE)id;
				if( id == KEYWORD_REM ) {
					/* �R�����g���̏ꍇ�͍s���܂ŉ����������̂܂܃R�s�[���� */
					while( TRUE ) {
						if( *src == '\0' ) break;
						*dst++ = *src++;
					}
				}
			} else {
				*dst++ = *src++;
			}
		} else if( *src == '\'' ) {
			*dst++ = *src++;
			/* �R�����g���̏ꍇ�͍s���܂ŉ����������̂܂܃R�s�[���� */
			while( TRUE ) {
				if( *src == '\0' ) break;
				*dst++ = *src++;
			}
		} else if( *src == '"' ) {
			src++;
			*dst++ = 0x03;
			while( TRUE ) {
				if( *src == '\0' ) {
					syntaxError();
					return FALSE;
				}
				if( *src == '"' ) break;
				*dst++ = *src++;
			}
			*dst++ = '\0';
			src++;
		} else {
			*dst++ = *src++;
		}
	}
	*dst = EOL;
	return TRUE;
}

/* �X�e�[�g�����g���s�����֐��̃��X�g */

NEARPROC statements[KEYWORDS_STATEMENT_TO-KEYWORDS_STATEMENT_FROM+1] = {
	st_if,
	st_print,
	st_locate,
	st_cls,
	st_goto,
	st_gosub,
	st_return,
	st_for,
	st_next,
	st_end,
	st_break,
	st_rem,
	st_new,
	st_list,
	st_run,
	st_cont,
	st_save,
	st_load,
	st_merge,
	st_randomize,
	st_exit,
	st_debug,
	st_waitvb,
	st_files
};

void interpreterMain()
{
	WORD keyscan;
	while( TRUE ) {
		/* �s�̊J�n */
		processLineHeader();
		/* �Ō�ɒB���Ă��܂���? */
		if( !bInteractive && currentLineNumber == 0 ) {
			bForceToReturnSuper = TRUE;
			requestNextAction = REQUEST_INTERACTIVE;
			return;
		}

		while( TRUE ) {
			BYTE ch;
			if( !bInteractive ) {
				resumePointer = executionPointer;	/* cont����ꏊ���L������ */
			}

#ifdef WW
			keyscan = key_press_check();
#else
			keyscan = win32_scan();
#endif
			if( keyscan & 0x02 ) {
				st_break();
				return;
			}

			ch = *executionPointer++;
			if( ch == EOL ) break;
			if( ch == ' ' || ch == '\t' || ch == ':' ) {
				/* nop */
			} else if( ch == '\'' ) {	/* comment */
				st_rem();
			} else if( ch == '@' ) {	/* is it l-value? */
				SHORT * pvar;
				pvar = getArrayReference();
				if( pvar == NULL ) return;
				st_assignment( pvar );
			} else if( ch >= 'A' && ch <= 'Z' ) {
				st_assignment( &globalVariables[ch-'A'] );
			} else if( ch >= 'a' && ch <= 'z' ) {
				st_assignment( &localVariables[ch-'a'] );
			} else if( ch >= KEYWORDS_STATEMENT_FROM && ch <= KEYWORDS_STATEMENT_TO ) {
				(*(statements[ch-KEYWORDS_STATEMENT_FROM]))();
			} else {
				syntaxError();
			}
			if( bForceToReturnSuper ) return;
		}
		if( bInteractive ) return;
	}
}

void interactiveMain( FILE FAR * fp )
{
	if( fp == NULL ) {
		commonPrint(NULL,"*Ready\n");
	}
	while( TRUE ) {
		BOOL b;
		if( fp == NULL ) {
			consoleInput( waRawLine, LINE_SIZE );
		} else {
			char FAR * r;
			char * p;
			r = fgets( waRawLine, LINE_SIZE, fp );
			if( r == NULL ) break;
			p = waRawLine;
			while( TRUE ) {
				if( *p == '\0' ) break;
				if( *p == '\n' || *p == '\r') {
					*p = '\0';
					break;
				}
				p++;
			}
		}
#ifdef WW
		if( fp == NULL ) {
			commonPrint( NULL, "\r\n" );
		}
#endif
		/* ���Ԍ���ɖ|�󂷂� */
		b = convertInternalCode( waCoockedLine, waRawLine );
		if( b == FALSE ) {
			bForceToReturnSuper = FALSE;
			continue;
		}
		/* ���l�ŊJ�n����Ă��邩? */
		if( waCoockedLine[0] == 0x01 ) {
			/* �s�G�f�B�^���Ăяo�� */
			editLine();
		} else {
			/* ���̍s�����s���� */
			executionPointer = waCoockedLine;
			interpreterMain();
		}
		if( bForceToReturnSuper ) return;
	}
}

/* �v���O�����̎��s�J�n */
void do_run( WORD runLineNumber )
{
	BYTE * target;
	if( runLineNumber == 0 ) {
		target = &wa[codeTop];
	} else {
		target = getLineReferenceFromLineNumber( runLineNumber );
		if( target == NULL ) {
			lineNumberNotFound( runLineNumber );
			return;
		}
	}
	clearRuntimeInfo();
	bInteractive = FALSE;
	executionPointer = target;
}

void do_new()
{
	clearRuntimeInfo();
	bInteractive = TRUE;
	codeTop = 0;
	wa[0] = 0;
	wa[1] = 0;
	wa[2] = EOL;
	dataTop = 3;
}

BOOL do_merge( const BYTE * filename )
{
	FILE FAR * fp;
	fp = fopen( filename, "r" );
	if( fp == NULL ) {
		loadError();
		return FALSE;
	}
	interactiveMain( fp );
	fclose( fp );
	if( bForceToReturnSuper ) return FALSE;
	return TRUE;
}

void superMain()
{
	while( TRUE ) {
		BOOL b;
		bForceToReturnSuper = FALSE;
		switch( requestNextAction ) {
		case REQUEST_INTERACTIVE:
			bInteractive = TRUE;
			break;
		case REQUEST_RUN_LINENUMBER:
			do_run( runLineNumber );
			break;
		case REQUEST_RUN_FILE:
			do_new();
			b = do_merge( waRawLine );
			if( b ) {
				do_run( 0 );
			}
			break;
		case REQUEST_LOAD_FILE:
			do_new();
			do_merge( waRawLine );
			break;
		case REQUEST_MERGE_FILE:
			do_merge( waRawLine );
			break;
		case REQUEST_NEW:
			do_new();
			break;
		case REQUEST_EXIT_SYSTEM:
			return;
		}
		requestNextAction = REQUEST_NO_ACTION;
		if( bInteractive ) {
			if( runtimeOnly ) return;	/* �����^�C�����[�h�̂Ƃ��͐�΂ɃC���^���N�e�B�u���[�h�ɍs���Ȃ� */
			interactiveMain( NULL );
		} else {
			interpreterMain();
		}
		bForceToReturnSuper = FALSE;
	}
}

#ifdef WW
BOOL entranceUI()
{
#define MAX_ENT 15
	/* ���̊֐�����static��Turbo-C 2.0��SS!=DS����������邽�߂ɕK�v�Ȃ��� */
	static int i;
	struct stat statbuf;
	static char names[MAX_ENT][MAXFNAME];
	static int nent;
	static int c, select;

	ptextCLS();
	ptextString( "�����ׂ� V" );
	ptextString( myVersion );
	ptextNewline();
	ptextString( "����A�Ńt�@�C���I�� Y2�ŊJ���� START�Œ��~" );

	c = 0;
	nent = nument(NULL);

	for (i = 0; i < nent; i++) {
		int result;
		if( c >= MAX_ENT ) break;
		result = getent(NULL, i, &statbuf);
		if (result == E_FS_SUCCESS) {
			if (statbuf.count != -1) {
				/* Freya OS�̃o�O?�@����R�[�h */
				{
					static int p;
					int q;
					p = 0;
					q = 0;
					waRawLine[p++] = statbuf.info[q++];
					waRawLine[p++] = statbuf.info[q++];
					while( TRUE ) {
						if( statbuf.info[q] == '\0' ) break;
						waRawLine[p++] = statbuf.info[q++];
					}
					waRawLine[p++] = '\0';
					ptextLocate(1,c+2);
					ptextString( waRawLine );
				}
				{
					static int r;
					int q;
					q = 0;
					r = 0;
					while( TRUE ) {
						if( statbuf.name[q] == '\0' ) break;
						names[c][r++] = statbuf.name[q++];
					}
					names[c][r] = '\0';
				}
				c++;
			}
		}
	}
	/* �I���̊J�n */
	select = 0;
	while( TRUE ) {
		int key;
		for( i=0; i<c; i++ ) {
			ptextLocate(0,i+2);
			ptextString( " " );
		}
		ptextLocate(0,select+2);
		ptextString( "��" );
		key = key_wait();
		/* START */
		if( key & 0x02 ) return FALSE;	/* ���~ */
		/* A */
		if( key & 0x04 ) {
			strcpy( waRawLine, names[select] );
			requestNextAction = REQUEST_RUN_FILE;
			runtimeOnly = TRUE;
			return TRUE;
		}
		/* B */
		if( key & 0x08 ) return FALSE;	/* ���~ */
		/* X1 or Y1 */
		if( (key & 0x10) != 0 || (key & 0x100) != 0 ) {
			select = select-1;
			if( select < 0 ) {
				select = c-1;
			}
		}
		/* X3 or Y3 */
		if( (key & 0x40) != 0 || (key & 0x400) != 0 ) {
			select = select+1;
			if( select >= c ) {
				select = 0;
			}
		}
		if( (key & 0x200) != 0 ) {
			return TRUE;
		}
	}
}
#endif

int main( int argc, char *argv[] )
{
#ifdef WW
	BOOL b;
#endif
	runtimeOnly = FALSE;
	requestNextAction = REQUEST_NO_ACTION;
#ifdef WIN32
	createText();
#endif
#ifdef WW
	text_screen_init();
	b = entranceUI();
	if( b == FALSE ) return 0;
	if( !runtimeOnly ) {
		comm_set_baudrate(1);
		comm_open();
	}
#endif
	ptextCLS();
	if( requestNextAction == REQUEST_NO_ACTION ) {
		do_new();
	}
	if( !runtimeOnly ) {
		commonPrint(NULL,"�����ׂ� WonderWitch Tiny BASIC Environment Ver %s\n",myVersion);
		commonPrint(NULL,"Copyright 2000 (c) by Pie Dey Co.,Ltd.\n");
	}
	superMain();
#ifdef WIN32
	deleteText();
#endif
#ifdef WW
	if( !runtimeOnly ) {
		comm_close();
	}
#endif
	return 0;
}

/* end of wonbe.c */
