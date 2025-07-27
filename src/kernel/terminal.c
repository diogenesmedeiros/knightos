#include <drivers/io.h>
#include <stdint.h>
#include <kernel/terminal.h>
#include <drivers/keyboard.h>

static uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static int terminal_row = 0;
static int terminal_col = 0;
static uint8_t terminal_color = 0x07;

static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t) c | ((uint16_t) color << 8);
}

void update_cursor(int row, int col) {
    uint16_t position = row * VGA_WIDTH + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}

char terminal_getchar() {
    return keyboard_get_char();
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
            terminal_putc('\b');
            terminal_putc(' ');
            terminal_putc('\b');
            continue;
        }
        buf[i++] = c;
        terminal_putc('*');
    }
    buf[i] = '\0';
    terminal_print("\n");
}

void terminal_clear() {
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[y * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_col = 0;
}

void terminal_init() {
    terminal_clear();
    terminal_row = 0;
    terminal_col = 0;
    update_cursor(0, 0);
}

void terminal_putc(char c) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            for (int y = 1; y < VGA_HEIGHT; y++) {
                for (int x = 0; x < VGA_WIDTH; x++) {
                    VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
                }
            }
            for (int x = 0; x < VGA_WIDTH; x++) {
                VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
            }
            terminal_row = VGA_HEIGHT - 1;
        }
        update_cursor(terminal_row, terminal_col);
        return;
    } else if (c == '\b') {
        if (terminal_col > 0) terminal_col--;
        VGA_BUFFER[terminal_row * VGA_WIDTH + terminal_col] = vga_entry(' ', terminal_color);
        update_cursor(terminal_row, terminal_col);
        return;
    }

    VGA_BUFFER[terminal_row * VGA_WIDTH + terminal_col] = vga_entry(c, terminal_color);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT) {
            for (int y = 1; y < VGA_HEIGHT; y++) {
                for (int x = 0; x < VGA_WIDTH; x++) {
                    VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
                }
            }
            for (int x = 0; x < VGA_WIDTH; x++) {
                VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
            }
            terminal_row = VGA_HEIGHT - 1;
        }
    }

    update_cursor(terminal_row, terminal_col);
}

void terminal_print(const char* str) {
    for (int i = 0; str[i]; i++) {
        terminal_putc(str[i]);
    }
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

    while (--i >= 0) {
        terminal_putc(buffer[i]);
    }
}

void terminal_write_hex(uint32_t value) {
    terminal_print("0x");

    for (int i = 7; i >= 0; i--) {
        uint8_t nibble = (value >> (i * 4)) & 0xF;
        if (nibble < 10)
            terminal_putc('0' + nibble);
        else
            terminal_putc('A' + (nibble - 10));
    }
}

void terminal_write_size(uint64_t bytes) {
    const char* unit;
    uint32_t int_part = 0;
    uint32_t decimal_part = 0;

    if (bytes >= (1ULL << 40)) {
        unit = "TB";
        uint64_t tb = 1ULL << 40;
        int_part = (uint32_t)(bytes >> 40);
        decimal_part = (uint32_t)(((bytes & (tb - 1)) * 100) >> 40);
    } else if (bytes >= (1ULL << 30)) {
        unit = "GB";
        uint64_t gb = 1ULL << 30;
        int_part = (uint32_t)(bytes >> 30);
        decimal_part = (uint32_t)(((bytes & (gb - 1)) * 100) >> 30);
    } else {
        unit = "MB";
        uint64_t mb = 1ULL << 20;
        int_part = (uint32_t)(bytes >> 20);
        decimal_part = (uint32_t)(((bytes & (mb - 1)) * 100) >> 20);
    }

    terminal_write_dec(int_part);
    terminal_putc('.');
    if (decimal_part < 10)
        terminal_putc('0');
    terminal_write_dec(decimal_part);
    terminal_print(" ");
    terminal_print(unit);
}

void terminal_backspace() {
    terminal_putc('\b');
    terminal_putc(' ');
    terminal_putc('\b');
}

void terminal_print_int(int value) {
    char buffer[32];
    itoa(value, buffer, 10);
    terminal_print(buffer);
}
