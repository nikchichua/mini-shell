#ifndef STRING_ARRAY_STRUCT_H
#define STRING_ARRAY_STRUCT_H
typedef struct StringArray {
    int size;
    int maxSize;
    char **strings;
} StringArray;
#endif