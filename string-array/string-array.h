#include "string-array_struct.h"
#include "../string/string.h"
#ifndef STRING_ARRAY_H
#define STRING_ARRAY_H
StringArray newStringArray();
void addString(StringArray *array, char *string);
void removeLastString(StringArray *array);
char **getStrings(StringArray *array);
StringArray getCommand(String *string);
void prependToFirst(StringArray *command, const char* prependString);
void replaceFirst(StringArray *array, const char* replacement);
bool isDirectory(StringArray *command);
String toString(StringArray *command);
void freeStringArray(StringArray *array);
#endif