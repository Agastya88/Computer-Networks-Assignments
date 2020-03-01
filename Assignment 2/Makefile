CFLAGS=-g -Wall -pedantic

hubs_switches: hubs_switches.c
	gcc $(CFLAGS) -o hubs_switches hubs_switches.c

beb: beb.c
	gcc $(CFLAGS) -o beb beb.c

.PHONY: clean
clean:
	rm -f *.o $(PROGS)
