#ifndef STRING_STRUCT_H
#define STRING_STRUCT_H
typedef struct String {
    int size;
    int maxSize;
    char *characters;
} String;
#endif