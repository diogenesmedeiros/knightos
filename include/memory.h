#include <stdint.h>

#ifndef MEMORY_H
#define MEMORY_H

void init_physical_memory(uint32_t mem_size);
void* pmm_alloc();
void  pmm_free(void* addr);

#endif