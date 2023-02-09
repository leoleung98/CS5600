// Leo Liang
// 1/23/2023
// CS5700
// Create Cipher

#include "polybius.h"
#include <math.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // create polybius table as martix 
    char table[5][5] = {{'A','B','C','D','E'},{'F','G','H','I','K'},{'L','M','N','O','P'},{'Q','R','S','T','U'},{'V','W','X','Y','Z'}};
    // get the input string from the terminal
    const char *plaintext = argv[2];
    // get the cipertext by using pbEncode function
    char *temp = pbEncode(plaintext, table);
    //
    //char *temp = pbDecode(plaintext, table);
    // print the result to terminal
    printf("ciphertext: %s\n", temp);
    // free the space used by the cipertext
    free(temp);
    return 0;
}