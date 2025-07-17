#ifndef TERMINAL_H
#define TERMINAL_H

#include <drivers/io.h>
#include <stdint.h>

void terminal_init();
void terminal_putc(char c);
void terminal_print(const char* str);
void terminal_write_dec(uint64_t value);
void terminal_write_hex(uint32_t value);
void terminal_write_size(uint64_t bytes);
void terminal_print_int(int value);
void update_cursor(int row, int col);
void terminal_read_password(char* buf, int max_len);
char terminal_getchar();

#endif