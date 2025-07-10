#ifndef TERMINAL_H
#define TERMINAL_H

#include <drivers/io.h>
#include <stdint.h>

void terminal_init();
void terminal_putc(char c);
void terminal_print(const char* str);

#endif