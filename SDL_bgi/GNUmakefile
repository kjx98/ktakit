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

CFLAGS=$(DEBUGS) $(OPTIMIZE) -Wall -I../include

LIB = libSDL_bgi.a
objs =	SDL_bgi.o
srcs =	SDL_bgi.c

all:	$(LIB)

clean:
	rm -f *.o *.a *.so

libSDL_bgi.a: $(objs)
	ar ru libSDL_bgi.a $(objs)
	cp libSDL_bgi.a ../lib

depend:
	sed "/^# Automatically/q" GNUmakefile >Makefile.1
	mv Makefile.1 GNUmakefile
	$(CC) $(CFLAGS) -MM $(srcs) >> GNUmakefile
	echo "# Makefile ends here" >> GNUmakefile

# Automatically generated dependencies
# Makefile ends here
