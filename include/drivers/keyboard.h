#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <kernel/terminal.h>
#include <kernel/shell.h>
#include <drivers/io.h>

#define KEYBOARD_BUFFER_SIZE 256

char compose_dead_key(char dead, char base);
char scancode_to_char(uint8_t scancode);
void keyboard_poll();
void keyboard_put_char(char c);
char keyboard_get_char();
char keyboard_read_char_blocking();

#endif