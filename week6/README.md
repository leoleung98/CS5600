Build a Custom Heap

This is a simple implementation to allocate and free memory in C programming language. The implementation includes three files:

pmalloc.c: This file contains the implementation of the custom memory allocator pm_alloc and pm_free functions.

pmalloc.h: This is the header file for pmalloc.c that includes the declarations of pmalloc and pmfree.

test.c: This file contains the test code to check the behavior of pmalloc and pmfree.


How to Use:

1. Run the make command to compile the code. The Makefile is set up to create an executable file named pm_test . 
"make"
2. Run the test executable file to execute the test code. 
"./pm_test" or "make run"
3. To clean up the compiled files, run the following command: 
"make clean"
