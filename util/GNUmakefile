#
#	Makefile for RuYi Technical Analysis
#
# switches:
#	define the ones you want in the CFLAGS definition...
#
#	TRACE		- turn on tracing/debugging code
#
#
ifeq	($(OPTIMIZE),)
OPTIMIZE=-O2
endif
ifeq	($(DEBUGS),)
DEBUGS= -g
endif

CFLAGS=$(DEBUGS) $(OPTIMIZE) -I../include

objs =	util.o crc.o skw.o chgext.o warn.o filepath.o ini.o foxint.o memwatch.o
srcs =	util.c crc.c skw.c chgext.c warn.c filepath.c ini.c foxint.c memwatch.c

all:	libmyutils.a

clean:
	rm -f *.o *.a

libmyutils.a:	$(objs)
	ar ru libmyutils.a $(objs)
	cp libmyutils.a ../lib

depend:
	sed "/^# Automatically/q" GNUmakefile >Makefile.1
	mv Makefile.1 GNUmakefile
	$(CC) $(CFLAGS) -MM $(srcs) >> GNUmakefile
	echo "# Makefile ends here" >> GNUmakefile

# Automatically generated dependencies
# Makefile ends here
