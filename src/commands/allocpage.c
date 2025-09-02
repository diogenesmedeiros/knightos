#include <kernel/terminal.h>
#include <kernel/memory.h>

void cmd_allocpage(const char* args) {
    uint32_t addr = alloc_page();
    if (addr == 0) {
        terminal_print("Failed to allocate memory.\n");
        return;
    }

    char* mem = (char*)addr;
    strcpy(mem, "KnightOS test string");

    terminal_print("Memory written to 0x");
    terminal_write_hex(addr);
    terminal_print(": ");
    terminal_print(mem);
    terminal_print("\n");

    free_page(addr);
    terminal_print("Page released.\n");
}