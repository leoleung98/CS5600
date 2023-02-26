/*
* pm_test.c
* 
* Leo Liang / CS5600 / Northeastern University
* Spring 2023 / Feb 21, 2023
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)

#define MIN_SIZE (4 * KB)
#define MAX_SIZE (64 * MB)
#define NUM_ALLOCS 10000

int main() {
    void *allocs[NUM_ALLOCS];
    size_t sizes[NUM_ALLOCS];
    size_t total_allocated = 0;
    size_t total_freed = 0;
    size_t total_remaining = 0;

    // Initialize random number generator
    srand(time(NULL));

    // Allocate memory
    for (int i = 0; i < NUM_ALLOCS; i++) {
        // Choose random size between MIN_SIZE and MAX_SIZE
        size_t size = MIN_SIZE + (rand() % (MAX_SIZE - MIN_SIZE + 1));
        sizes[i] = size;

        // Allocate memory
        void *ptr = malloc(size);
        allocs[i] = ptr;

        // Check for allocation failure
        if (ptr == NULL) {
            printf("Error: Allocation failed for size %lu\n", size);
            break;
        }

        total_allocated += size;
    }

    // Free memory
    for (int i = 0; i < NUM_ALLOCS; i++) {
        void *ptr = allocs[i];
        size_t size = sizes[i];
        if (ptr != NULL) {
            free(ptr);
            total_freed += size;
        }
    }

    // Calculate remaining memory
    total_remaining = total_allocated - total_freed;

    // Print results
    printf("Total memory allocated: %lu bytes\n", total_allocated);
    printf("Total memory freed: %lu bytes\n", total_freed);
    printf("Total remaining memory: %lu bytes\n", total_remaining);

    return 0;
}
