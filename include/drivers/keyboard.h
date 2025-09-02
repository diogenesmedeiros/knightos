#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

#define KEYBOARD_BUFFER_SIZE 256
#define KEY_UP 1
#define KEY_DOWN 2

char compose_dead_key(char dead, char base);
char scancode_to_char(uint8_t scancode);
void keyboard_poll();
void keyboard_put_char(char c);
char keyboard_get_char();
char keyboard_read_char_blocking();
int keyboard_has_char();
int keyboard_get_special_key();
void keyboard_clear_buffer();

#endif