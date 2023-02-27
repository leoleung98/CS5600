#ifndef PM_HEAP_H
#define PM_HEAP_H

#include <stddef.h>

#define PM_HEAP_SIZE (10*1024*1024)

// Allocates a block of memory of the given size from the persistent memory heap.
// Returns a pointer to the allocated block, or NULL if the allocation fails.
void* pm_malloc(size_t size);

// Frees a block of memory previously allocated with pm_malloc.
// The pointer ptr must be a pointer returned by a previous call to pm_malloc.
void pm_free(void* ptr);

#endif /* PM_HEAP_H */