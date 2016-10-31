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

CFLAGS=$(DEBUGS) $(OPTIMIZE) -Wall -fpic -I../include

LIB = libSDL_bgi.so
objs =	SDL_bgi.o
srcs =	SDL_bgi.c

all:	$(LIB)

clean:
	rm -f *.o *.a *.so

libSDL_bgi.so: $(OBJ)
	$(CC) -shared -o $(LIB) $(OBJ)
	cp $(LIB) ../lib

depend:
	sed "/^# Automatically/q" GNUmakefile >Makefile.1
	mv Makefile.1 GNUmakefile
	$(CC) $(CFLAGS) -MM $(srcs) >> GNUmakefile
	echo "# Makefile ends here" >> GNUmakefile

# Automatically generated dependencies
# Makefile ends here
