#include <kernel/cpu.h>

void cpu_halt() {
    __asm__ volatile ("hlt");
}

void cpu_cli() {
    __asm__ volatile ("cli");
}

void cpu_sti() {
    __asm__ volatile ("sti");
}

void cpu_cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    __asm__ volatile ("cpuid"
                      : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                      : "a"(code));
}

uint32_t cpu_read_cr0() {
    uint32_t value;
    __asm__ volatile ("mov %%cr0, %0" : "=r"(value));
    return value;
}

void cpu_write_cr0(uint32_t value) {
    __asm__ volatile ("mov %0, %%cr0" :: "r"(value));
}

uint32_t cpu_read_cr3() {
    uint32_t value;
    __asm__ volatile ("mov %%cr3, %0" : "=r"(value));
    return value;
}

void cpu_write_cr3(uint32_t value) {
    __asm__ volatile ("mov %0, %%cr3" :: "r"(value));
}

void cpu_breakpoint() {
    __asm__ volatile ("int $3");
}