#include <kernel/kernel.h>

__attribute__((section(".multiboot"), used))
const struct multiboot_header_t multiboot_header = {
    MULTIBOOT_MAGIC,
    MULTIBOOT_FLAGS,
    MULTIBOOT_CHECKSUM,
    0, 0, 0, 0,
    (uint32_t)kernel_main
};

extern void register_default_commands();

void kernel_main(void) {
    user_init();
    register_default_commands();
    terminal_init();
    shell_prompt();

    while (1) {
        keyboard_poll();
    }
}