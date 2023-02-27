/*
* pm_heap_s.c
* 
* Leo Liang / CS5600 / Northeastern University
* Spring 2023 / Feb 21, 2023
*
*/

#include "pm_heap_s.h"
#include <stddef.h>
#include <pthread.h>

static char pm_heap[PM_HEAP_SIZE];
size_t pm_heap_used = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* pm_malloc(size_t size) {
    void* result = NULL;
    size_t i, parent, child;

    pthread_mutex_lock(&mutex); // Acquire the mutex lock

    // Check for integer overflow 
    if (size > (size_t)-1 - pm_heap_used) {
        pthread_mutex_unlock(&mutex); // Release the mutex lock
        return NULL;
    }

    // Check if the pm-heap has enough space 
    if (pm_heap_used + size > PM_HEAP_SIZE) {
        pthread_mutex_unlock(&mutex); // Release the mutex lock
        return NULL;
    }

    // Allocate memory from the pm-heap 
    result = pm_heap + pm_heap_used;
    pm_heap_used += size;

    // Maintain the min-heap property 
    i = (pm_heap_used - size) / sizeof(size_t);
    while (i > 0) {
        parent = (i - 1) / 2;
        if (*((size_t*)(pm_heap + parent * sizeof(size_t))) > *((size_t*)(pm_heap + i * sizeof(size_t)))) {
            // Swap parent and child 
            child = i;
            i = parent;
            parent = (i - 1) / 2;

            *((size_t*)(pm_heap + child * sizeof(size_t))) = *((size_t*)(pm_heap + parent * sizeof(size_t)));
            *((size_t*)(pm_heap + parent * sizeof(size_t))) = size;
        } else {
            break;
        }
    }

    pthread_mutex_unlock(&mutex); // Release the mutex lock
    return result;
}

void pm_free(void* ptr) {
    size_t index, parent, child, min_child;
    size_t* p_index = (size_t*)((char*)ptr - sizeof(size_t));

    // Check if the pointer is valid and in the pm-heap 
    if (ptr == NULL || p_index < (size_t*)pm_heap || p_index >= (size_t*)(pm_heap + pm_heap_used)) {
        return;
    }

    // Lock the mutex before accessing the pm_heap
    pthread_mutex_lock(&mutex);

    // Find the index of the freed memory block 
    index = (size_t)(p_index - (size_t*)pm_heap) / sizeof(size_t);

    // Move the last memory block to the freed memory block 
    pm_heap_used -= sizeof(size_t);
    if (index < pm_heap_used / sizeof(size_t)) {
        *((size_t*)p_index) = *((size_t*)(pm_heap + pm_heap_used));
    }

    // Maintain the min-heap property 
    child = index;
    while (child > 0) {
        parent = (child - 1) / 2;
        if (*((size_t*)(pm_heap + parent * sizeof(size_t))) > *((size_t*)(pm_heap + child * sizeof(size_t)))) {
            // Swap parent and child 
            min_child = (child + 1 < pm_heap_used / sizeof(size_t) &&
                         *((size_t*)(pm_heap + (child + 1) * sizeof(size_t))) < *((size_t*)(pm_heap + child * sizeof(size_t)))) ?
                        child + 1 : child;
            *((size_t*)(pm_heap + parent * sizeof(size_t))) ^= *((size_t*)(pm_heap + min_child * sizeof(size_t)));
            *((size_t*)(pm_heap + min_child * sizeof(size_t))) ^= *((size_t*)(pm_heap + parent * sizeof(size_t)));
            *((size_t*)(pm_heap + parent * sizeof(size_t))) ^= *((size_t*)(pm_heap + min_child * sizeof(size_t)));
            child = parent;
        } else {
            break;
        }
    }

    // Unlock the mutex after accessing the pm_heap
    pthread_mutex_unlock(&mutex);
    return;
}
