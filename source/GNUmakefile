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

objs =	taarray.o tacalc.o tacandle.o tacsi.o tact.o tadate.o tadj.o tafile.o \
	tafin.o tafs.o taio.o tastr.o  tatc.o tatrade.o tatt.o tatxt.o
srcs =	taarray.c tacalc.c tacandle.c tacsi.c tact.c tadate.c tadj.c tafile.c \
	tafin.c tafs.c taio.c tastr.c  tatc.c tatrade.c tatt.c tatxt.c

all:	libktakit.a

clean:
	rm -f *.o *.a

libktakit.a:	$(objs)
	ar ru libktakit.a $(objs)
	cp libktakit.a ../lib

depend:
	sed "/^# Automatically/q" GNUmakefile >Makefile.1
	mv Makefile.1 GNUmakefile
	$(CC) $(CFLAGS) -MM $(srcs) >> GNUmakefile
	echo "# Makefile ends here" >> GNUmakefile

# Automatically generated dependencies
# Makefile ends here
