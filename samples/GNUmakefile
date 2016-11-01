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

objs =	advdec.o calc1.o candles.o dumpfxt.o dumpmt4.o formatn.o optimize.o \
	study.o tick2m1.o trade.o study.o chart.o optimize.o formatn.o \
	finance.o moving.o
srcs =	advdec.c calc1.c candles.c dumpfxt.c dumpmt4.c formatn.c optimize.c \
	study.c tick2m1.c trade.c study.c chart.c optimize.c formatn.c \
	finance.c moving.c
libs =  ../lib/libktakit.a # ../lib/libmyutils.a
all:	advdec calc1 candles trade study chart optimize formatn moving finance

clean:
	rm -f *.o *.a

advdec:	advdec.o $(libs)
	$(CC) -L../lib -o advdec $@.o -lktakit -lm

calc1:	calc1.o $(libs)
	$(CC) -L../lib -o calc1 $@.o -lktakit -lm

trade:	trade.o $(libs)
	$(CC) -L../lib -o trade $@.o -lktakit -lm

study:	study.o $(libs)
	$(CC) -L../lib -o study $@.o -lktakit -lm

candles: candles.o $(libs)
	$(CC) -L../lib -o candles $@.o -lktakit -lm

optimize: optimize.o $(libs)
	$(CC) -L../lib -o optimize $@.o -lktakit -lm

formatn: formatn.o $(libs)
	$(CC) -L../lib -o formatn $@.o -lktakit -lm

moving:	moving.o $(libs)
	$(CC) -L../lib -o moving $@.o -lktakit -lm

finance: finance.o $(libs)
	$(CC) -L../lib -o finance $@.o -lktakit -lm

chart:	chart.o $(libs)
	$(CC) -L../lib -o chart $@.o -lktakit -lSDL_bgi -lSDL2 -lm

depend:
	sed "/^# Automatically/q" GNUmakefile >Makefile.1
	mv Makefile.1 GNUmakefile
	$(CC) $(CFLAGS) -MM $(srcs) >> GNUmakefile
	echo "# Makefile ends here" >> GNUmakefile

# Automatically generated dependencies
# Makefile ends here
