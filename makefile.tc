
# start up routine
C0WW_JAPANESE2=..\common\c0wwjpn2.obj

C0WW=$(C0WW_JAPANESE2)

LIBWW=@..\common\libww.rsp

CFLAGS=-DWW -ms -zPCGROUP -zSDGROUP -zGDGROUP

all:	wonbe.fx

wonbe.fx:	wonbe.bin
	mkfent wonbe.cf

wonbe.bin:	wonbe.obj
	tlink /m /c $(C0WW) wonbe, wonbe, wonbe, $(LIBWW)
	exe2fbin wonbe.exe wonbe.bin

wonbe.obj:	wonbe.c
	tcc -c $(CFLAGS) $(DEFINES) -IC:\vshare\ww\dev\WWitch\include wonbe.c


