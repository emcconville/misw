CC=clang
CFLAGS=
LDFLAGS=-lncurses

all: misw

main.o: main.c
	${CC} ${CFLAGS} -c -o main.o main.c

misw.o: misw.c
	${CC} ${CFLAGS} -c -o misw.o misw.c

misw: main.o misw.o Makefile
	${CC} ${LDFLAGS} -o misw main.o misw.o

clean:
	rm -f misw

run: misw
	./misw

install:
	date
