#include <stdlib.h>
#include "string.h"

String newString() {
    String string;
    string.size = 0;
    string.maxSize = 10;
    string.characters = malloc(sizeof(char) * (string.maxSize + 1));
    string.characters[string.size] = '\0';
    return string;
}

void addChar(String *string, char character) {
    int *size = &string->size;
    int *maxSize = &string->maxSize;
    if (*size >= *maxSize) {
        *maxSize *= 2;
        string->characters = realloc(string->characters, sizeof(char) * (*maxSize + 1));
    }
    string->characters[*size] = character;
    string->characters[++*size] = '\0';
}

void freeString(String *string) {
    free(string->characters);
}