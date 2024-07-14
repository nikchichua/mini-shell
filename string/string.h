#include "string_struct.h"

#ifndef STRING_H
#define STRING_H
String newString();
void addChar(String *string, char character);
void freeString(String *string);
#endif