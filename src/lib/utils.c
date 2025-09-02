#include <lib/string.h>
#include <lib/utils.h>

void trim(char* str) {
    if (!str || *str == '\0') return;

    while (*str == ' ') str++;

    char* end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\n' || *end == '\r')) {
        *end-- = '\0';
    }

    memmove(str, str, strlen(str) + 1);
}

int atoi(const char* str) {
    int result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}
