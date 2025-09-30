#include <drivers/vga.h>
#include <stdarg.h>

void terminal_init() {
    vga_init();
}

void terminal_clear() {
    vga_clear();
}

void terminal_putc(char c) {
    vga_putc(c);
}

void terminal_print(const char* str) {
    vga_print(str);
}

void terminal_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vga_printf(fmt, args);
    va_end(args);
}

void update_cursor(int row, int col) {
    vga_update_cursor(row, col); 
}

void terminal_print_int(int value) {
    char buffer[32];
    itoa(value, buffer, 10);
    terminal_print(buffer);
}

/**
 * Imprime uma string com cor de foreground e background especificadas.
 * fg e bg devem ser valores de 0x0 a 0xF conforme a tabela VGA.
 */
void terminal_print_color(const char* str, uint8_t fg, uint8_t bg) {
    uint8_t prev_color = vga_get_color(); // você precisa criar vga_get_color()

    vga_set_color((bg << 4) | (fg));
    while (*str) vga_putc(*str++);

    vga_set_color(prev_color);
}

void terminal_write_dec(uint64_t value) {
    char buffer[21];
    int i = 0;
    if (value == 0) {
        terminal_putc('0');
        return;
    }
    while (value > 0 && i < 20) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    while (--i >= 0) terminal_putc(buffer[i]);
}

void terminal_write_hex(uint32_t value) {
    terminal_print("0x");
    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (value >> (i * 4)) & 0xF;
        terminal_putc(nibble < 10 ? '0' + nibble : 'A' + nibble - 10);
    }
}

void terminal_write_size(uint64_t bytes) {
    const char* unit;
    uint32_t int_part, decimal_part;

    if (bytes >= (1ULL << 40)) {
        unit = "TB";
        int_part = bytes >> 40;
        decimal_part = ((bytes & ((1ULL << 40) - 1)) * 100) >> 40;
    } else if (bytes >= (1ULL << 30)) {
        unit = "GB";
        int_part = bytes >> 30;
        decimal_part = ((bytes & ((1ULL << 30) - 1)) * 100) >> 30;
    } else {
        unit = "MB";
        int_part = bytes >> 20;
        decimal_part = ((bytes & ((1ULL << 20) - 1)) * 100) >> 20;
    }

    terminal_write_dec(int_part);
    terminal_putc('.');
    if (decimal_part < 10) terminal_putc('0');
    terminal_write_dec(decimal_part);
    terminal_print(" ");
    terminal_print(unit);
}

char terminal_getchar() {
    return keyboard_get_char();
}

void terminal_backspace() {
    terminal_putc('\b');
    terminal_putc(' ');
    terminal_putc('\b');
}

void terminal_read_password(char* buf, int max_len) {
    int i = 0;
    while (i < max_len - 1) {
        keyboard_poll();
        if (!keyboard_has_char()) continue;

        char c = terminal_getchar();
        if (c == '\n' || c == '\r') break;
        if (c == '\b' && i > 0) {
            i--;
            terminal_backspace();
            continue;
        }
        buf[i++] = c;
        terminal_putc('*');
    }
    buf[i] = '\0';
    terminal_putc('\n');
}