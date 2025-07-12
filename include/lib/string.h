#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, int n);
int strlen(const char* str);
int memcmp(const void* s1, const void* s2, size_t n);
void itoa(int value, char* str, int base);
char* strrchr(const char* s, int c);

#endif
