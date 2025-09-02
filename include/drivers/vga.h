#ifndef VGA_H
#define VGA_H

#include <stdint.h>

void vga_init();
void vga_clear();
void vga_putc(char c);
void vga_print(const char* str);
void vga_printf(const char* fmt, ...);
void vga_update_cursor(int row, int col);
uint8_t vga_get_color();

#endif