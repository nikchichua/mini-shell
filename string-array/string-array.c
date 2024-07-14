#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../string/string.h"
#include "string-array_struct.h"

StringArray newStringArray() {
    StringArray array;
    array.size = 0;
    array.maxSize = 10;
    array.strings = malloc(sizeof(char *) * (array.maxSize + 1));
    return array;
}

void addString(StringArray *array, char *string) {
    if(array->size >= array->maxSize) {
        array->maxSize *= 2;
        array->strings = realloc(array->strings, sizeof(char *) * (array->maxSize + 1));
    }
    char *newString = malloc(sizeof(char) * (strlen(string) + 1));
    strcpy(newString, string);
    array->strings[array->size] = newString;
    array->size++;
}

void removeLastString(StringArray *array) {
    free(array->strings[array->size - 1]);
    array->strings[array->size - 1] = NULL;
    array->size--;
}

char **getStrings(StringArray *array) {
    return array->strings;
}

StringArray getCommand(String *string) {
    StringArray array = newStringArray();
    char *pch;
    pch = strtok(string->characters, " ");
    int i = 0;
    while (pch != NULL) {
        addString(&array, pch);
        pch = strtok(NULL, " ");
        i++;
    }
    array.strings[array.size] = NULL;
    return array;
}

void prependToFirst(StringArray *array, const char* prependString) {
    char **string = &array->strings[0];
    size_t originalSize = strlen(*string);
    size_t additionalSize = strlen(prependString);
    *string = realloc(*string, sizeof(char) * (originalSize + additionalSize + 1));
    memmove(*string + additionalSize, *string, originalSize + 1);
    memcpy(*string, prependString, additionalSize);
}

void replaceFirst(StringArray *array, const char* replacement) {
    free(array->strings[0]);
    array->strings[0] = malloc(sizeof(char) * (strlen(replacement) + 1));
    strcpy(array->strings[0], replacement);
}

bool isDirectory(StringArray *array) {
    if((array->strings[0][0] == '/') ||
       (array->strings[0][0] == '.' && array->strings[0][1] == '/') ||
       (array->strings[0][0] == '.' && array->strings[0][1] == '.' && array->strings[0][2] == '/')) {
        return true;
    }
    return false;
}

String toString(StringArray *array) {
    String string = newString();
    for(int i = 0; i < array->size; i++) {
        for(int j = 0; j < strlen(array->strings[i]); j++) {
            addChar(&string, array->strings[i][j]);
        }
        if(i != array->size - 1) {
            addChar(&string, ' ');
        }
    }
    return string;
}

void freeStringArray(StringArray *array) {
    for (int i = 0; i < array->size; ++i) {
        free(array->strings[i]);
    }
    free(array->strings);
}