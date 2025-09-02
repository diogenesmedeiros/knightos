#include "lib/libc_compat.h"

int isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

int isalpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isdigit(char c) {
    return c >= '0' && c <= '9';
}

int isalnum(char c) {
    return isalpha(c) || isdigit(c);
}