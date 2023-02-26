/*
* pm_heap.h
* 
* Leo Liang / CS5600 / Northeastern University
* Spring 2023 / Feb 21, 2023
*
*/
#ifndef PM_HEAP_H
#define PM_HEAP_H

#include <stddef.h>

#define PM_HEAP_SIZE (10*1024*1024)

extern size_t pm_heap_used;

void* pm_malloc(size_t size);
void pm_free(void* ptr);

#endif /* PM_HEAP_H */
