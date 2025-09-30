#include <drivers/io.h>
#include <drivers/vga.h>
#include <lib/string.h>
#include <stdarg.h>

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* const VGA_BUFFER = (uint16_t*)VGA_ADDRESS;
static int vga_row = 0;
static int vga_col = 0;
static uint8_t vga_color = 0x07; // cinza claro sobre o preto

// Cria um valor de 16 bits para a célula VGA (ASCII + cor)
static uint16_t vga_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

// Atualiza cursor na tela via portas VGA
void vga_update_cursor(int row, int col) {
    uint16_t pos = row * VGA_WIDTH + col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

uint8_t vga_get_color() {
    return vga_color;
}

// Limpa a tela e reseta cursor
void vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_BUFFER[i] = vga_entry(' ', vga_color);

    vga_row = vga_col = 0;
    vga_update_cursor(0, 0);
}

// Inicializa driver
void vga_init() {
    vga_clear();
}

// Scroll, move linhas para cima e limpa última linha
static void vga_scroll() {
    if (vga_row < VGA_HEIGHT) return;
    memmove(VGA_BUFFER, VGA_BUFFER + VGA_WIDTH, (VGA_HEIGHT - 1) * VGA_WIDTH * sizeof(uint16_t));
    for (int x = 0; x < VGA_WIDTH; x++)
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', vga_color);
    vga_row = VGA_HEIGHT - 1;
}

// Define cor atual de escrita
void vga_set_color(uint8_t color) {
    vga_color = color;
}

// Escreve um caractere na tela com suporte a \n, \r, \b e scroll
void vga_putc(char c) {
    switch (c) {
        case '\n':
            vga_col = 0;
            vga_row++;
            break;
        case '\r':
            vga_col = 0;
            break;
        case '\b':
            if (vga_col > 0) {
                vga_col--;
                VGA_BUFFER[vga_row * VGA_WIDTH + vga_col] = vga_entry(' ', vga_color);
            } else if (vga_row > 0) {
                vga_row--;
                vga_col = VGA_WIDTH - 1;
                VGA_BUFFER[vga_row * VGA_WIDTH + vga_col] = vga_entry(' ', vga_color);
            }
            break;
        default:
            VGA_BUFFER[vga_row * VGA_WIDTH + vga_col] = vga_entry(c, vga_color);
            vga_col++;
            if (vga_col >= VGA_WIDTH) {
                vga_col = 0;
                vga_row++;
            }
            break;
    }

    vga_scroll();
    vga_update_cursor(vga_row, vga_col);
}

// Escreve string simples
void vga_print(const char* str) {
    while (*str) vga_putc(*str++);
}

void vga_vprintf(const char* fmt, va_list args) {
    char buffer[32];
    for (size_t i = 0; fmt[i]; i++) {
        if (fmt[i] == '%') {
            i++;
            switch (fmt[i]) {
                case 'c':
                    vga_putc((char)va_arg(args, int));
                    break;
                case 's':
                    vga_print(va_arg(args, char*));
                    break;
                case 'd': {
                    int val = va_arg(args, int);
                    int len = 0;
                    if (val == 0) buffer[len++] = '0';
                    else {
                        int v = val;
                        if (v < 0) {
                            vga_putc('-');
                            v = -v;
                        }
                        while (v > 0) {
                            buffer[len++] = '0' + (v % 10);
                            v /= 10;
                        }
                        for (int j = 0; j < len/2; j++) {
                            char t = buffer[j];
                            buffer[j] = buffer[len-1-j];
                            buffer[len-1-j] = t;
                        }
                    }
                    buffer[len] = 0;
                    vga_print(buffer);
                    break;
                }
                case 'x': {
                    int val = va_arg(args, int);
                    int len = 0;
                    for (int shift = 28; shift >= 0; shift -= 4) {
                        uint8_t nibble = (val >> shift) & 0xF;
                        if (len || nibble != 0) buffer[len++] = (nibble < 10) ? ('0'+nibble) : ('A'+nibble-10);
                    }
                    if (!len) buffer[len++] = '0';
                    buffer[len] = 0;
                    vga_print(buffer);
                    break;
                }
                default:
                    vga_putc('%');
                    vga_putc(fmt[i]);
                    break;
            }
        } else {
            vga_putc(fmt[i]);
        }
    }
}

void vga_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vga_vprintf(fmt, args);
    va_end(args);
}