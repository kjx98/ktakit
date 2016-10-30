#
#	Makefile for RuYi Technical Analysis
#
# switches:
#	define the ones you want in the CFLAGS definition...
#
#	TRACE		- turn on tracing/debugging code
#
#

#.BEFORE
#    set include=$(%watcom)\h\nt;$(%include)

DEBUGS=

CFLAGS=$(DEBUGS) -I..\h -ot -bt=nt -5r -fp5 -s /d2 /wx -DWINDOWS

.c.obj: .AUTODEPEND
	wcc386 $(CFLAGS) $[*
	wlib ..\lib\wutils.lib -+$@

objs =	util.obj crc.obj skw.obj chgext.obj warn.obj filepath.obj des.obj foxint.obj memwatch.obj
srcs =	util.c crc.c skw.c chgext.c warn.c filepath.c des.c foxint.c memwatch.c

all:	$(objs)  #..\lib\wutils.lib

..\lib\wutils.lib:	 $(objs) .SYMBOLIC
        echo done

clean:	.SYMBOLIC
	del *.obj
