CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99
LDFLAGS=

all: pm_heap.o pm_test.o
	$(CC) $(LDFLAGS) -o pm_test pm_heap.o pm_test.o

pm_heap.o: pm_heap.c pm_heap.h
	$(CC) $(CFLAGS) -c pm_heap.c

pm_test.o: pm_test.c pm_heap.h
	$(CC) $(CFLAGS) -c pm_test.c

run: all
	./pm_test

clean:
	rm -f *.o pm_test
