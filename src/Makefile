OPTFLAGS=-O2
CFLAGS=-Wall -std=c99 -D_POSIX_C_SOURCE=200809L -pedantic -g ${OPTFLAGS}
CC=gcc

OBJS=\
	common.o \
	forms.o \
	string_buf.o \
	neutral_buf.o \
	active_buf.o \
	primitives.o \
	boolean.o \
	io.o \
	trac64.o \
	ansi.o

all: trac64

trac64: ${OBJS}

help.h: help.txt
	xxd -i help.txt > help.h

clean:
	rm -f ${OBJS} trac64 TAGS

etags:
	etags *.c *.h

run: clean etags trac64
	./trac64

deps:
	gcc -MM *.c *.h > depend

include depend
