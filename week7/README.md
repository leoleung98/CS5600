Persistent Memory Heap
This project implements a persistent memory heap in C, which allows for allocation and deallocation of memory that persists across program executions. It includes a set of functions for allocating and freeing memory, as well as a test program to verify the functionality of the heap.

Files
The project consists of the following files:

pm_heap_s.c: Implementation of the persistent memory heap.
pm_heap_s.h: Header file for the persistent memory heap.
pm_test_s.c: Test program for the persistent memory heap.
Makefile: Makefile for building the test program.

Building and Running
To build the test program, simply run make in the project directory. This will generate an executable named pm_test_s. To run the test program, run make run. This will build the test program if necessary and execute it.

Functionality
The persistent memory heap provides two functions for allocating and freeing memory:

void* pm_malloc(size_t size): Allocates a block of memory of the given size from the persistent memory heap. Returns a pointer to the allocated block, or NULL if the allocation fails.
void pm_free(void* ptr): Frees a block of memory previously allocated with pm_malloc. The pointer ptr must be a pointer returned by a previous call to pm_malloc.
The test program verifies the functionality of the heap by allocating and freeing memory from multiple threads in a loop. If the program completes without errors, the heap is considered to be functioning correctly.


How to Use:

1. Run the make command to compile the code. The Makefile is set up to create an executable file named pm_test . 
"make"
2. Run the test executable file to execute the test code. 
"./pm_test_s" or "make run"
3. To clean up the compiled files, run the following command: 
"make clean"
