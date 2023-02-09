CC = gcc
IFLAG = -I.

all: queue.o queuetest.o 
	$(CC) -o queue queuetest.o queue.o 

queuetest.o: queuetest.c
	$(CC) -c queuetest.c -o queuetest.o

queue.o: queue.c queue.h
	$(CC) $(IFLAG) -c queue.c -o queue.o
