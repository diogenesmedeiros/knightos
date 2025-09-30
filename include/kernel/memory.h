#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

uint64_t detect_memory(uint32_t magic, uint32_t addr);

void memory_init(uint64_t total_ram, uint32_t kernel_start, uint32_t kernel_end);

uint32_t alloc_page(void);
void free_page(uint32_t addr);
void* kmalloc(size_t size);

#endif