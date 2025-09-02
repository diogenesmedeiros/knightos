#ifndef CPU_H
#define CPU_H

#include <stdint.h>

void cpuid(uint32_t code, uint32_t* a, uint32_t* b, uint32_t* c, uint32_t* d);
void get_cpu_brand(char* buffer);

#endif
