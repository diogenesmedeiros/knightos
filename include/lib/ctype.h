#ifndef _CTYPE_H
#define _CTYPE_H

int isprint(int c) {
    return (c >= 32 && c <= 126);
}

int isspace(int c) {
    return c == ' ' || (c >= '\t' && c <= '\r');
}

int isdigit(int c) {
    return c >= '0' && c <= '9';
}

int isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isalnum(int c) {
    return isalpha(c) || isdigit(c);
}

int isxdigit(int c) {
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

int tolower(int c) {
    return (c >= 'A' && c <= 'Z') ? (c + 32) : c;
}

int toupper(int c) {
    return (c >= 'a' && c <= 'z') ? (c - 32) : c;
}

#endif