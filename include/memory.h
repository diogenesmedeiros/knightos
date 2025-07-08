#include <stdint.h>

#ifndef MEMORY_H
#define MEMORY_H

void init_physical_memory();
void* pmm_alloc();
void  pmm_free(void* addr);
void memory_init();
uint64_t get_total_memory_bytes();

#endif