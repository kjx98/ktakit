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
	study.o tick2m1.o trade.o study.o chart.o optimize.o
srcs =	advdec.c calc1.c candles.c dumpfxt.c dumpmt4.c formatn.c optimize.c \
	study.c tick2m1.c trade.c study.c chart.c optimize.c

all:	advdec calc1 candles trade study chart optimize

clean:
	rm -f *.o *.a

advdec:	advdec.o
	$(CC) -L../lib -o advdec $? -lktakit -lutil -lm

calc1:	calc1.o
	$(CC) -L../lib -o calc1 $? -lktakit -lutil -lm

trade:	trade.o
	$(CC) -L../lib -o trade $? -lktakit -lutil -lm

study:	study.o
	$(CC) -L../lib -o study $? -lktakit -lutil -lm

candles: candles.o
	$(CC) -L../lib -o candles $? -lktakit -lutil -lm

optimize: optimize.o
	$(CC) -L../lib -o optimize $? -lktakit -lutil -lm

chart:	chart.o
	$(CC) -L../lib -o chart $? -lktakit -lutil -lSDL_bgi -lSDL2 -lm

depend:
	sed "/^# Automatically/q" GNUmakefile >Makefile.1
	mv Makefile.1 GNUmakefile
	$(CC) $(CFLAGS) -MM $(srcs) >> GNUmakefile
	echo "# Makefile ends here" >> GNUmakefile

# Automatically generated dependencies
# Makefile ends here
