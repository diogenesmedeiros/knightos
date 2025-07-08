#include "terminal.h"
#include "shell.h"
#include "keyboard.h"
#include "memory.h"
#include "user.h"

void kernel_main(void);

#define MULTIBOOT_MAGIC 0x1BADB002
#define MULTIBOOT_FLAGS 0x00000003
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

__attribute__((section(".multiboot"), used))
static const struct {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} multiboot_header = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM,
    0, 0, 0, 0,
    (uint32_t)kernel_main
};

void print_char(int pos, char c) {
    char* vga = (char*)0xb8000;
    vga[pos * 2] = c;
    vga[pos * 2 + 1] = 0x07;
}

void kernel_main(void) {
    init_physical_memory(16 * 1024 * 1024);
    user_init();
    terminal_init();
    shell_prompt();

    while (1) {
        keyboard_poll();
    }
}