CFLAGS=-g -Wall -pedantic

PROGS=hexdump hexread

DEPS=util.h

all: hexdump hexread $(DEPS)

hexdump: util.o hexdump.o
	gcc $(CFLAGS) -o hexdump hexdump.o util.o

hexread: util.o hexread.o
	gcc $(CFLAGS) -o hexread hexread.o util.o 

hexdump.o: hexdump.c
	gcc $(CFLAGS) -c -o hexdump.o hexdump.c

hexread.o: hexread.c
	gcc $(CFLAGS) -c -o hexread.o hexread.c

util.o: util.c
	gcc $(CFLAGS) -c -o util.o util.c

.PHONY: clean
clean:
	rm -f *.o $(PROGS)
