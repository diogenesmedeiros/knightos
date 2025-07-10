#include <kernel/terminal.h>

static uint16_t* const VGA_BUFFER = (uint16_t*)0xB8000;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;
static int terminal_row = 0;
static int terminal_col = 0;
static uint8_t terminal_color = 0x07;

static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t) c | ((uint16_t) color << 8);
}

static void update_cursor(int row, int col) {
    uint16_t position = row * VGA_WIDTH + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
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
}

void terminal_putc(char c) {
    if (c == '\n') {
        terminal_col = 0;
        terminal_row++;
        return;
    } else if (c == '\b') {
        if (terminal_col > 0) terminal_col--;
        VGA_BUFFER[terminal_row * VGA_WIDTH + terminal_col] = vga_entry(' ', terminal_color);
        return;
    }

    VGA_BUFFER[terminal_row * VGA_WIDTH + terminal_col] = vga_entry(c, terminal_color);
    terminal_col++;
    if (terminal_col >= VGA_WIDTH) {
        terminal_col = 0;
        terminal_row++;
    }
    if (terminal_row >= VGA_HEIGHT) {
        terminal_row = 0;
    }

    update_cursor(terminal_row, terminal_col);
}

void terminal_print(const char* str) {
    for (int i = 0; str[i]; i++) {
        terminal_putc(str[i]);
    }
}