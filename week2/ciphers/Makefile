CC = gcc
IFLAG = -I.

all: cipher.o polybius.o 
	$(CC) -o cipher cipher.o polybius.o 

cipher.o: cipher.c
	$(CC) -c cipher.c -o cipher.o

polybius.o: polybius.c polybius.h
	$(CC) $(IFLAG) -c polybius.c -o polybius.o
