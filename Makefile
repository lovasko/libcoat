CC=clang
CFLAGS=-O3 -fPIC -Isrc -ansi -pedantic -Wall
LDFLAGS=-fPIC -shared
INC_DIR=/usr/include
LIB_DIR=/usr/lib

all: coat.o
	$(CC) $(LDFLAGS) -o libcoat.so coat.o

coat.o:
	$(CC) $(CFLAGS) -c src/coat.c

install:
	cp -v libcoat.so "${LIB_DIR}"
	cp -v src/coat.h "${INC_DIR}"

clean:
	rm -v coat.o libcoat.so
