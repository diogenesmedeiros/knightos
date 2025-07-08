#include "terminal.h"
#include "shell.h"
#include "keyboard.h"
#include "memory.h"
#include "user.h"
#include "kernel.h"

__attribute__((section(".multiboot"), used))
const struct multiboot_header_t multiboot_header = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM,
    0, 0, 0, 0,
    (uint32_t)kernel_main
};

extern void register_default_commands();

void print_char(int pos, char c) {
    char* vga = (char*)0xb8000;
    vga[pos * 2] = c;
    vga[pos * 2 + 1] = 0x07;
}

void kernel_main(void) {
    init_physical_memory(128 * 1024 * 1024); // TODO: 128 MB de memoria ram
    user_init();
    register_default_commands();
    terminal_init();
    shell_prompt();

    while (1) {
        keyboard_poll();
    }
}