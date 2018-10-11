CC=clang
CFLAGS=
LDFLAGS=-lncurses

all: misw

misw: misw.c Makefile
	${CC} ${CFLAGS} ${LDFLAGS} -o misw misw.c

clean:
	rm -f misw