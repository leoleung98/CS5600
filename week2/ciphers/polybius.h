// Leo Liang
// 1/23/2023
// CS5700
// Cipher header

// polybius.h
#include <stdlib.h>
#include <stdio.h>

char* pbEncode(const char *plaintext, char table[5][5]);
char* pbDecode(const char *ciphertext, char table[5][5]);