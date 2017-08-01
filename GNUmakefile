#
#	Makefile for ktakit
#
# switches:
#	define the ones you want in the CFLAGS definition...
#
#	TRACE		- turn on tracing/debugging code
#
#
#
# Borland C++ tools
#
WITH_MATCH=
SUBDIRS=source util SDL_bgi samples
REALSUBDIRS=source util SDL_bgi samples
SYMSUBDIRS=include
CC=gcc
OPTIMIZE=-O2
OPTIMIZE +=-Wpedantic
# _REENTRANT for thread (no need for FreeBSD/Linux)
# AIX (RS6000) use -mthreads for pthread
USE_THREAD=
ifeq	($(USE_THREAD),y)
OPTIMIZE +=	-pthread
endif

export CC OPTIMIZE USE_THREAD
MAKEFILE=GNUmakefile

# We Use Compact Memory Model

all:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i -f $(MAKEFILE); done

depend:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i -f $(MAKEFILE) depend; done

links:
	rm -rf build
	mkdir build
	cp $(MAKEFILE) build
	[ -d build/lib ] || mkdir build/lib
	for i in $(REALSUBDIRS); do ./make_links $$i $(MAKEFILE); done
	cd build;for i in $(SYMSUBDIRS); do ln -sf ../$$i .; done

clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i -f $(MAKEFILE) clean; done
