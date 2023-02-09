// Leo Liang
// 1/23/2023
// CS5700
// Create Cipher

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>

struct thread_args {
    const char *plaintext;
    const char *des;
    int number;
};

char* pbEncode(const char *plaintext, char table[5][5]) {
    // create temp string to store output cipertext
    char *temp = calloc(strlen(plaintext) * 4, 1);
    // check every char in the input string
    int l = 0;
    for (int i = 0; plaintext[i] != '\0'; i++) {
        if (((int)plaintext[i] >= 65 && (int)plaintext[i] <= 90) || ((int)plaintext[i] >= 97 && (int)plaintext[i] <= 122)) {
            // check polybius table
            for (int j = 0; j < 5; j++) {
                for (int k = 0; k < 5; k++) {
                    // return row and column if found
                    char value = toupper(plaintext[i]);
                    if (value == table[j][k]) {
                        temp[l++] = j + 1 + '0';
                        temp[l++] = k + 1 + '0'; 
                    }
                    // return row and column the same as 'I' if input is 'J'
                    else if (value == 'J') {
                        temp[l++] = 2 + '0';
                        temp[l++] = 4 + '0'; 
                    }
                }
            }
        }
        else {
            //add special char to the string
            temp[l++] = plaintext[i];
        }
    }
    // return the cipertext
    return temp;
}

void *run(void *arg) {
    // create polybius table as martix 
    char table[5][5] = {{'A','B','C','D','E'},{'F','G','H','I','K'},{'L','M','N','O','P'},{'Q','R','S','T','U'},{'V','W','X','Y','Z'}};
    struct thread_args *args = (struct thread_args *)arg;
    const char *plaintext = args->plaintext;
    const char *des = args->des;
    int number = args->number;

    // Create the directory if it doesn't exist
    struct stat st = {0};
    if (stat(des, &st) == -1) {
        mkdir(des, 0700);
    }

    char filename[100];
    snprintf(filename, 100, "%s/file_%d.txt", des, number);
    FILE *f = fopen(filename, "a");
    if (f == NULL) {
        printf("Error opening file!\n");
        return NULL;
    }

    // get the cipertext by using pbEncode function
    char *temp = pbEncode(plaintext, table);
    // output the result
    fprintf(f, "%s\n", temp);
    fclose(f);
    // free the space used by the cipertext
    free(temp);
    pthread_exit(NULL);
    return NULL;
}