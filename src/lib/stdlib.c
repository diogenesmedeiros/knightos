#include "lib/stdlib.h"
#include <stddef.h>

static char heap[1024 * 1024];
static size_t heap_offset = 0;

void* malloc(size_t size) {
    if (heap_offset + size > sizeof(heap)) {
        return NULL;
    }
    void* ptr = &heap[heap_offset];
    heap_offset += size;
    return ptr;
}

void free(void* ptr) {
    (void)ptr;
}

char* itoa(int value, char* str, int base) {
    if (base < 2 || base > 16) {
        str[0] = '\0';
        return str;
    }

    char* ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;
    int sign = 0;

    if (value < 0 && base == 10) {
        sign = 1;
        value = -value;
    }

    do {
        tmp_value = value % base;
        *ptr++ = (tmp_value < 10) ? (tmp_value + '0') : (tmp_value - 10 + 'a');
        value /= base;
    } while (value);

    if (sign) {
        *ptr++ = '-';
    }

    *ptr-- = '\0';

    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }

    return str;
}