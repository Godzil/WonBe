INCLUDE=C:\vshare\ww\dev\WWitch\include
LIB=..\COMMON

LIBS=libww.lib runtime.lib

LINKER=..\..\vc15\bin\link
CC=..\..\vc15\bin\cl
ASM=..\..\vc15\bin\masm

CFLAGS=/ASw /O /Gs /DWW
AFLAGS=-Mx -DDISPLAY_MODE_JAPANESE1
LFLAGS=/NOD /NOI /MAP

all:	wonbe.fx

wonbe.fx:	wonbe.bin
	mkfent wonbe.cf

wonbe.bin:	wonbe.obj vc15_rt.obj
	$(LINKER) @<<
$(LFLAGS) vc15_rt.obj wonbe.obj,wonbe.sss,wonbe.map,$(LIBS);
<<
	echo 0000 >0000.sss
	exe2bin wonbe.sss wonbe.bin <0000.sss
	echo remove temp file
	del 0000.sss
	del wonbe.sss

wonbe.obj:	wonbe.c
	$(CC) -c $(CFLAGS) wonbe.c

vc15_rt.obj: vc15_rt.asm
	$(ASM) $(AFLAGS) vc15_rt.asm;

