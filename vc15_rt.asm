;***
;vc15_rt.asm: Wonder Witch Start-up code for Visual C++ 1.5x
;

SRAM_SEG		equ	1000h

MAXPATHLEN		equ	64

INT_EXIT		equ	10h
INT_DISP		equ	12h
INT_TEXT		equ	13h

DISPLAY_CONTROL		equ	00h
SPRITE_SET_RANGE	equ	0bh
SCREEN_SET_SCROLL	equ	13h
SCREEN2_SET_WINDOW	equ	15h
SPRITE_SET_WINDOW	equ	17h
SCREEN_SET_VRAM		equ	21h
SPRITE_SET_VRAM		equ	22h

TEXT_SCREEN_INIT	equ	00h
TEXT_SET_MODE		equ	02h
TEXT_SET_SCREEN		equ	0eh

TEXT_MODE_ANK		equ	0
TEXT_MODE_ANK_SJIS	equ	1
TEXT_MODE_SJIS		equ	2

SCREEN1			equ	0
SCREEN2			equ	1

DCM_SCR1		equ	1
DCM_SCR2		equ	2

V_SPRITE_START		equ	0
V_SPRITE_COUNT		equ	0

ifdef DISPLAY_MODE_JAPANESE1
V_DISPLAY_CONTROL	equ	DCM_SCR2
V_TEXT_SCREEN		equ	SCREEN2
V_TEXT_MODE		equ	TEXT_MODE_ANK_SJIS
V_SCREEN1_VRAM		equ	3		; duplicated with screen2
V_SCREEN2_VRAM		equ	3
V_SPRITE_VRAM		equ	11
V_STACK_TOP		equ	01600h
endif

ifdef DISPLAY_MODE_JAPANESE2
V_DISPLAY_CONTROL	equ	DCM_SCR1 + DCM_SCR2
V_TEXT_SCREEN		equ	SCREEN2
V_TEXT_MODE		equ	TEXT_MODE_ANK_SJIS
V_SCREEN1_VRAM		equ	2
V_SCREEN2_VRAM		equ	3
V_SPRITE_VRAM		equ	7
V_STACK_TOP		equ	00e00h
endif

ifdef DISPLAY_MODE_ASCII1
V_DISPLAY_CONTROL	equ	DCM_SCR2
V_TEXT_SCREEN		equ	SCREEN2
V_TEXT_MODE		equ	TEXT_MODE_ANK
V_SCREEN1_VRAM		equ	6		; duplicated with screen2
V_SCREEN2_VRAM		equ	6
V_SPRITE_VRAM		equ	23
V_STACK_TOP		equ	02e00h
endif

ifdef DISPLAY_MODE_ASCII2
V_DISPLAY_CONTROL	equ	DCM_SCR1 + DCM_SCR2
V_TEXT_SCREEN		equ	SCREEN2
V_TEXT_MODE		equ	TEXT_MODE_ANK
V_SCREEN1_VRAM		equ	6
V_SCREEN2_VRAM		equ	5
V_SPRITE_VRAM		equ	19
V_STACK_TOP		equ	02600h
endif

_TEXT		segment byte public 'CODE'
_TEXT		ends

TEXT		segment byte public 'CODE'
TEXT		ends

_DATA		segment para public 'DATA'
dseg_start	proc	far
dseg_start	endp
_DATA		ends

DATA		segment para public 'DATA'
DATA		ends

CONST		segment para public 'CONST'
CONST		ends

_BSS		segment word public 'BSS'
bss_start	proc	far
bss_start	endp
_BSS		ends

BSS		segment word public 'BSS'
BSS		ends

c_common		segment word public 'BSS'
c_common		ends

_HEAP		segment para public 'HEAP'
heap_start	proc	far
heap_start	endp
_HEAP		ends

CGROUP		group   _TEXT, TEXT
DGROUP		group   _DATA, DATA, CONST, _BSS, BSS, c_common, _HEAP

     		assume  cs:CGROUP, ds:DGROUP, es:DGROUP

_TEXT		segment byte public 'CODE'
		extrn	_main	:near
		public	_load0
		public	_run0
		public	_exit0

_load0		proc	far
		push	ds

		push	di
		push	si
		push	cx
		push	bx

; calcurate initialized data size
		mov	cx, offset DGROUP:bss_start
		shr	cx, 1		; word alignmented

; copy initialized data into user ds
		mov	ax, cs
		sub	ax, CGROUP
;		add	ax, seg dseg_start
		add	ax, DGROUP
		mov	ds, ax
		mov	ax, SRAM_SEG
		mov	es, ax
		mov	si, offset DGROUP:dseg_start
		mov	di, si
	rep	movsw

; clear bss
		mov	cx, offset DGROUP:heap_start
		push	cx
		mov	di, offset DGROUP:bss_start
		sub	cx, di
		shr	cx, 1
		xor	ax, ax
	rep	stosw

; calcurate heap start position
; set argv starting position at heap start
		pop	es:[__argv]

		mov	dx, cs
		mov	ax, offset _run0

		pop	bx
		pop	cx
		pop	si
		pop	di

		pop	ds
		ret
_load0		endp

_run0		proc	far ;C, argc :word, argv :near ptr
		push	bp
		mov	bp,sp
		push	ds
		mov	dx, SRAM_SEG
		mov	es, dx
		mov	ds, dx
;		mov	ax, sp
;		mov	word ptr ds:[__sp], ax
;		mov	ax, ss
;		mov	word ptr ds:[__sp + 2], ax

;; setup SP according to the display modes

;;	display_control(0);
		mov	bx, 0
		mov	ah, DISPLAY_CONTROL
		int	INT_DISP

;;	screen_set_scroll(SCREEN1, 0, 0);
		mov	bx, 00000h
		mov	al, SCREEN1
		mov	ah, SCREEN_SET_SCROLL
		int	INT_DISP

;;	screen_set_scroll(SCREEN2, 0, 0);
		mov	bx, 00000h
		mov	al, SCREEN2
		mov	ah, SCREEN_SET_SCROLL
		int	INT_DISP

;;	screen2_set_window(0, 0, 255, 255)
		mov	cx, 0ffffh
		mov	bx, 00000h
		mov	ah, SCREEN2_SET_WINDOW
		int	INT_DISP

;;	sprite_set_window(0, 0, 255, 255)
		mov	cx, 0ffffh
		mov	bx, 00000h
		mov	ah, SPRITE_SET_WINDOW
		int	INT_DISP

;;	text_set_screen(V_TEXT_SCREEN);
		mov	al, V_TEXT_SCREEN
		mov	ah, TEXT_SET_SCREEN
		int	INT_TEXT

;;	text_set_mode(V_TEXT_MODE);
		mov	bx, V_TEXT_MODE
		mov	ah, TEXT_SET_MODE
		int	INT_TEXT

;;	screen_set_vram(SCREEN1, V_SCREEN1_VRAM)
		mov	bx, V_SCREEN1_VRAM
		mov	al, SCREEN1
		mov	ah, SCREEN_SET_VRAM
		int	INT_DISP

;;	screen_set_vram(SCREEN2, V_SCREEN2_VRAM)
		mov	bx, V_SCREEN2_VRAM
		mov	al, SCREEN2
		mov	ah, SCREEN_SET_VRAM
		int	INT_DISP

;;	sprite_set_vram(SCREEN2, V_SCREEN2_VRAM)
		mov	bx, V_SPRITE_VRAM
		mov	ah, SPRITE_SET_VRAM
		int	INT_DISP

;;	sprite_set_range(V_SPRITE_START, V_SPRITE_COUNT)
		mov	cx, V_SPRITE_COUNT
		mov	bx, V_SPRITE_START
		mov	ah, SPRITE_SET_RANGE
		int	INT_DISP

;;	display_control(V_DISPLAY_CONTROL);
		mov	bx, V_DISPLAY_CONTROL
		mov	ah, DISPLAY_CONTROL
		int	INT_DISP

		MOV	CX, [BP + 8]	; for argv	; mov	bx, argc
		MOV	BX, [BP + 6]	; for argc	; mov	cx, argv

		mov	ax, V_STACK_TOP
		mov	sp, ax
		push	cx	; argv
		push	bx	; argc
		call	_main
_exit00:
_exit0:
		int	INT_EXIT
		;mov	sp,bp
		;pop	bp
		;ret
_run0		endp

;_exit0:
;		mov	ax, SRAM_SEG
;		mov	es, ax
;		les	ax, dword ptr es:[__sp]
;		mov	sp, ax
;		mov	ax, es
;		mov	ss, ax
;		jmp	_exit00
_TEXT		ends

_DATA		segment para public 'DATA'

public	__acrtused		; trick to force in startup
	__acrtused = 9876h	; funny value not easily matched in SYMDEB

		public __id
		public __pid
		public __ppid
		public __pcbid
		public __ppcbid
		public __ilib
		public __proc
		public __cwfs
		public __currentdir
		public __argv
		public __heap
;dseg_start:
__id		db	'TCC', 0
__pid		dw	?
__ppid		dw	?
__pcbid		dw	?
__ppcbid	dw	?
__ilib		dd	?
__proc		dd	?
__cwfs		dd	?
__currentdir	db	MAXPATHLEN dup (?)
__argv		dw	?
__resource	dd	?
__heap		dw	?
_DATA		ends

		end

