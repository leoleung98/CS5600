/*
* multiprocessing.c
* 
* Leo Liang / CS5600 / Northeastern University
* Spring 2023 / Feb 8, 2023
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "cipher.h"

// create arguments set for the cipher function
struct thread_args {
    const char *plaintext;
    const char *des;
    const char *inputname;
    int number;
};

int main(int argc, char *argv[]) {
    // read file name
    const char *plaintext = argv[1];

    // create a string array with maximum 100 strings
    char arr[100][100];

    // read from the selected file
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(argv[2], "r");
    if (fp == NULL) exit(EXIT_FAILURE);
    int i = 0;
    // record all sentences into the string
    while ((read = getline(&line, &len, fp)) != -1) { 
        strcpy(arr[i], line);
        i++;
    }

    // select process method
    if (strcmp(plaintext,"Q3") == 0) {
        printf("Q3 is working\n");
        pid_t pid;
        // create fork process
        int j = 0;
        while (j < i - 1) {
            pid = fork();
            if (pid == -1) {
                printf("Error in fork!\n");
                return 1;
            }
            if (pid == 0) {
            }
            else {
                break;
            }
            j++;
        }
        // run cipher function
        struct thread_args args = {arr[j], "Q3", argv[2], j + 1};
        run(&args);
        sleep(1);
    }
    // select thread method
    else if (strcmp(plaintext,"Q4") == 0) {
        printf("Q4 is working\n");
        pthread_t t[i];
        // create thread
        for (int j = 0; j < 31; j++) {
            struct thread_args args = {arr[j], "Q4", argv[2], j + 1};
            if (pthread_create(&t[j], NULL, run, &args)) {
                printf("Error creating thread\n");
                return 1;
            }
            usleep(1000);
        }   
        sleep(5);
        // wait till all thread finished
        for (int j = 0; j < 31; j++) {
            if (pthread_join(t[j], NULL)) {
                printf("Error joining thread\n");
                return 1;
            } 
            j++; 
        }
    }

    // close the readed file
    fclose(fp);
    if (line)
        free(line);
    exit(EXIT_SUCCESS);
    return 0;
}
