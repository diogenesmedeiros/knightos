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

uint64_t g_total_ram = 0;

void kernel_main(uint32_t magic, uint32_t multiboot_info_addr) {
    g_total_ram = detect_memory(magic, multiboot_info_addr);
    
    user_init();
    register_default_commands();
    terminal_init();
    terminal_print("KnightOS 0.1a\nCopyright (c) Soft Knight. All rights reserved.");
    terminal_print("\n\n\n");
    
    shell_prompt();

    while (1) {
        keyboard_poll();
    }
}