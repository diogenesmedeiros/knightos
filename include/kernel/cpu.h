#ifndef CPU_H
#define CPU_H

#include <stdint.h>

void cpu_halt();
void cpu_cli();
void cpu_sti();
void cpu_cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);
uint32_t cpu_read_cr0();
void cpu_write_cr0(uint32_t value);
uint32_t cpu_read_cr3();
void cpu_write_cr3(uint32_t value);
void cpu_breakpoint();

#endif
