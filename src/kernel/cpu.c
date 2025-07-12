#include <kernel/cpu.h>

void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d) {
    __asm__ volatile ("cpuid"
                      : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                      : "a"(code));
}

void get_cpu_brand(char* buffer) {
    uint32_t a, b, c, d;

    for (int i = 0; i < 3; i++) {
        cpuid(0x80000002 + i, &a, &b, &c, &d);
        ((uint32_t*)buffer)[i * 4 + 0] = a;
        ((uint32_t*)buffer)[i * 4 + 1] = b;
        ((uint32_t*)buffer)[i * 4 + 2] = c;
        ((uint32_t*)buffer)[i * 4 + 3] = d;
    }

    buffer[48] = '\0';
}
