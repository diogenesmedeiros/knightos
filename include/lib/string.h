#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, int n);
int strlen(const char* str);

#endif
