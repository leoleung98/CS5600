CC = gcc
IFLAG = -I.

all: multiprocessing.o cipher.o 
	$(CC) -o multiprocessing multiprocessing.o cipher.o 

multiprocessing.o: multiprocessing.c
	$(CC) -c multiprocessing.c -o multiprocessing.o

cipher.o: cipher.c cipher.h
	$(CC) $(IFLAG) -c cipher.c -o cipher.o
