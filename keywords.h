/* WONBE predefined keyword IDs */
/* First Created: Nov.3,2000 by Pie Dey Co.,Ltd. */

/* This source code is distributed under GNU General Public License (GPL) */
/* see http://www.gnu.org/ about GPL */

#define	KEYWORD_IF		0x80
#define	KEYWORD_PRINT		0x81
#define	KEYWORD_LOCATE		0x82
#define	KEYWORD_CLS		0x83
#define	KEYWORD_GOTO		0x84
#define	KEYWORD_GOSUB		0x85
#define	KEYWORD_RETURN		0x86
#define	KEYWORD_FOR		0x87
#define	KEYWORD_NEXT		0x88
#define	KEYWORD_END		0x89
#define	KEYWORD_BREAK		0x8a
#define	KEYWORD_REM		0x8b
#define	KEYWORD_NEW		0x8c
#define	KEYWORD_LIST		0x8d
#define	KEYWORD_RUN		0x8e
#define	KEYWORD_CONT		0x8f
#define	KEYWORD_SAVE		0x90
#define	KEYWORD_LOAD		0x91
#define	KEYWORD_MERGE		0x92
#define	KEYWORD_RANDOMIZE	0x93
#define	KEYWORD_EXIT		0x94
#define	KEYWORD_DEBUG		0x95
#define	KEYWORD_WAITVB		0x96
#define	KEYWORD_FILES		0x97
#define	KEYWORD_PLAY		0x98
#define	KEYWORD_POKE		0x99
#define	KEYWORD_CALL		0x9a
#define	KEYWORD_INT			0x9b
#define	KEYWORD_TRON		0x9c
#define	KEYWORD_TROFF		0x9d

#define	KEYWORD_AND			0xa0
#define	KEYWORD_OR			0xa1
#define	KEYWORD_XOR			0xa2

#define	KEYWORD_NOT			0xb0
#define	KEYWORD_SCAN		0xc0
#define	KEYWORD_WAIT		0xc1
#define	KEYWORD_RND			0xc2
#define	KEYWORD_ABS			0xc3
#define	KEYWORD_TICK		0xc4
#define	KEYWORD_VARPTR		0xc5
#define	KEYWORD_VARSEG		0xc6
#define	KEYWORD_DSEG		0xc7
#define	KEYWORD_CSEG		0xc8
#define	KEYWORD_SSEG		0xc9
#define	KEYWORD_PEEK		0xca

#define	KEYWORD_SCAN_A		0xd0
#define	KEYWORD_SCAN_B		0xd1
#define	KEYWORD_SCAN_X1		0xd2
#define	KEYWORD_SCAN_X2		0xd3
#define	KEYWORD_SCAN_X3		0xd4
#define	KEYWORD_SCAN_X4		0xd5
#define	KEYWORD_SCAN_Y1		0xd6
#define	KEYWORD_SCAN_Y2		0xd7
#define	KEYWORD_SCAN_Y3		0xd8
#define	KEYWORD_SCAN_Y4		0xd9

#define	KEYWORD_AX			0xe0
#define	KEYWORD_BX			0xe1
#define	KEYWORD_CX			0xe2
#define	KEYWORD_DX			0xe3
#define	KEYWORD_SI			0xe4
#define	KEYWORD_DI			0xe5
#define	KEYWORD_DS			0xe6
#define	KEYWORD_ES			0xe7
#define	KEYWORD_DEFSEG		0xe8

#define	KEYWORD_THEN		0xf0
#define	KEYWORD_CHR			0xf1
#define	KEYWORD_TO			0xf2
#define	KEYWORD_STEP		0xf3

#define KEYWORDS_STATEMENT_FROM 0x80
#define KEYWORDS_STATEMENT_TO 0x9d
#define KEYWORDS_2OP_FROM 0xa0
#define KEYWORDS_2OP_TO 0xa2

/*
WONBE 中間言語フォーマット

　中間言語は1行を単位として構築される

　プログラムの終わりは0x00 0x00でターミネートされる

　1行の構造

WORD 行番号 (1-32767)
任意の中間コードの列
BYTE 0x0d 行末ターミネータ

　0x01～0x03を除けば0x0dを単純検索するだけで行末が発見できる

中間コード
0x00		(予約)
0x01		次にあるのは10進2バイト整数
0x02		次にあるのは16進2バイト整数
0x03		次にあるのは可変長文字列 0x00でターミネート
0x09		TAB
0x0d		行末ターミネータ
0x20～0x2f	そのコードの記号
0x30～0x39	?
0x3a～0x3f	そのコードの記号
0x40		@
0x41～0x5a	A～Z その名前の変数を示すシンボル
0x5b～0x60	?
0x61～0x7A	a～z その名前の変数を示すシンボル
0x7b～0x7f	?
0x80～0xff	キーワードの中間言語表現
*/

/* end of keywords.h */
