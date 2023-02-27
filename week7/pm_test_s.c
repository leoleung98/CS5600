#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "pm_heap_s.h"

#define NUM_THREADS 10
#define NUM_ITERATIONS 10000

void* thread_func(void* arg) {
    int i;
    void* ptr;

    // Allocate and free memory from the pm-heap in a loop
    for (i = 0; i < NUM_ITERATIONS; i++) {
        ptr = pm_malloc(16);
        if (ptr == NULL) {
            printf("Thread %ld: pm_malloc failed\n", (long)arg);
            return NULL;
        }
        pm_free(ptr);
    }

    return NULL;
}

int main() {
    int i;
    void* ptr;
    pthread_t threads[NUM_THREADS];

    // Allocate and free memory from the pm-heap in the main thread
    for (i = 0; i < NUM_ITERATIONS; i++) {
        ptr = pm_malloc(16);
        if (ptr == NULL) {
            printf("pm_malloc failed\n");
            return 1;
        }
        pm_free(ptr);
    }

    // Create multiple threads to allocate and free memory from the pm-heap
    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_func, (void*)(long)i) != 0) {
            printf("pthread_create failed\n");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            printf("pthread_join failed\n");
            return 1;
        }
    }

    printf("Test passed\n");
    return 0;
}
