
#ifndef MEMORY_H
#define MEMORY_H

#define PAGE_SIZE 4096
#define MAX_PAGES 32768
#define BITMAP_SET(b, i)   ((b)[(i) / 8] |=  (1 << ((i) % 8)))
#define BITMAP_CLEAR(b, i) ((b)[(i) / 8] &= ~(1 << ((i) % 8)))
#define BITMAP_TEST(b, i)  ((b)[(i) / 8] &   (1 << ((i) % 8)))

#include <stdint.h>

void init_physical_memory();
void* pmm_alloc();
void pmm_free(void* addr);
void memory_init();
uint64_t get_total_memory_bytes();
uint32_t pmm_count_free_pages();

#endif