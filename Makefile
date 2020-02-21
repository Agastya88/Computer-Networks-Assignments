CFLAGS=-g -Wall -pedantic

hexdump: hexdump.c
	gcc $(CFLAGS) -o hexdump hexdump.c

hexread: hexread.c
	gcc $(CFLAGS) -o hexread hexread.c

util: util.c
	gcc $(CFLAGS) -c -o util util.c

.PHONY: clean
clean:
	rm -f *.o $(PROGS)
