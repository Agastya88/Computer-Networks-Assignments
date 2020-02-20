CFLAGS=-g -Wall -pedantic

util: util.c
	gcc $(CFLAGS) -o util util.c

.PHONY: clean
clean:
	rm -f *.o $(PROGS)
