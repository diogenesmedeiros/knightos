#include "terminal.h"
#include <stdint.h>
#include "shell.h"

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

char scancode_to_char(uint8_t scancode) {
    const char* map = "??1234567890-=\b\tqwertyuiop[]\n?asdfghjkl;'`?\\zxcvbnm,./?? ";
    if (scancode > 58) return '?';
    return map[scancode];
}

void keyboard_poll() {
    static uint8_t last_scancode = 0;
    uint8_t scancode = inb(0x60);

    if (scancode & 0x80) {
        if ((scancode & 0x7F) == last_scancode) {
            last_scancode = 0;
        }
        return;
    }

    if (scancode == last_scancode) {
        return;
    }

    last_scancode = scancode;

    char c = scancode_to_char(scancode);
    if (c == '?') return;

    shell_handle_char(c);
}