CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c99
LDFLAGS=

all: pm_heap_s.o pm_test_s.o
	$(CC) $(LDFLAGS) -o pm_test_s pm_heap_s.o pm_test_s.o

pm_heap.o: pm_heap_s.c pm_heap_s.h
	$(CC) $(CFLAGS) -c pm_heap_s.c

pm_test.o: pm_test_s.c pm_heap_s.h
	$(CC) $(CFLAGS) -c pm_test_s.c

run: all
	./pm_test_s

clean:
	rm -f *.o pm_test_s
