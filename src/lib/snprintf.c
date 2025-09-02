#include <stdarg.h>
#include <stdint.h>

int snprintf(char* buf, int size, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int pos = 0;

    for (int i = 0; fmt[i] && pos < size - 1; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 's': {
                    const char* s = va_arg(args, const char*);
                    for (int j = 0; s[j] && pos < size - 1; j++)
                        buf[pos++] = s[j];
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    buf[pos++] = c;
                    break;
                }
                default:
                    buf[pos++] = fmt[i];
                    break;
            }
        } else {
            buf[pos++] = fmt[i];
        }
    }

    buf[pos] = '\0';
    va_end(args);
    return pos;
}
