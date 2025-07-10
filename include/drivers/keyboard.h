#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <kernel/terminal.h>
#include <kernel/shell.h>
#include <drivers/io.h>

char compose_dead_key(char dead, char base);
char scancode_to_char(uint8_t scancode);
void keyboard_poll();

#endif