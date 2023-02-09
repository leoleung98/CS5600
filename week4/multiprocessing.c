#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "cipher.h"

struct thread_args {
    const char *plaintext;
    const char *des;
    int number;
};

int main(int argc, char *argv[]) {
    const char *plaintext = argv[1];

    char arr[100][100];
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(argv[2], "r");
    if (fp == NULL) exit(EXIT_FAILURE);
    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) { 
        strcpy(arr[i], line);
        i++;
    }

    if (strcmp(plaintext,"Q3") == 0) {
        printf("Q3 is working\n");
        int j = 1;
    }
    else if (strcmp(plaintext,"Q4") == 0) {
        printf("Q4 is working\n");
        pthread_t t[i];
        
        for (int j = 0; j < 31; j++) {
            struct thread_args args = {arr[j], "Q4", j + 1};
            if (pthread_create(&t[j], NULL, run, &args)) {
                printf("Error creating thread\n");
                return 1;
            }
        }   
        for (int j = 0; j < 31; j++) {
            if (pthread_join(t[j], NULL)) {
                printf("Error joining thread\n");
                return 1;
            } 
            j++; 
        }
    }

    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
    return 0;
}
