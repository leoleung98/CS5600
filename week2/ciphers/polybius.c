// Leo Liang
// 1/23/2023
// CS5700
// Cipher function

#include "polybius.h"
#include <string.h>
#include <ctype.h>

char* pbEncode(const char *plaintext, char table[5][5]) {
    // create temp string to store output cipertext
    char *temp = calloc(strlen(plaintext) * 2, 1);
    // check every char in the input string
    int l = 0;
    for (int i = 0; plaintext[i] != '\0'; i++) {
        // check polybius table
        if (((int)plaintext[i] >= 65 && (int)plaintext[i] <= 90) || ((int)plaintext[i] >= 97 && (int)plaintext[i] <= 122)) {
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
            temp[l++] = plaintext[i];
        }
    }
    // return the cipertext
    return temp;
}

char* pbDecode(const char *ciphertext, char table[5][5]) {
    char *temp = calloc(strlen(ciphertext), 1);
    for (int i = 0; ciphertext[i] != '\0'; i += 2) {
        if ((ciphertext[i] >= '0' && ciphertext[i] <= '9')) {
            int j = i + 1;
            temp[i / 2] = table[ciphertext[i]- '0' - 1][ciphertext[j]- '0' - 1];
        }
        else {
            temp[i / 2] = ' ';
        }
    }
    return temp;
}
