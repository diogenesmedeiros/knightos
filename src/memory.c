#include "memory.h"

#define PAGE_SIZE 4096
#define MAX_PAGES 32768

static uint8_t bitmap[MAX_PAGES / 8];
static uint32_t total_pages = 0;

#define BITMAP_SET(b, i)   ((b)[(i) / 8] |=  (1 << ((i) % 8)))
#define BITMAP_CLEAR(b, i) ((b)[(i) / 8] &= ~(1 << ((i) % 8)))
#define BITMAP_TEST(b, i)  ((b)[(i) / 8] &   (1 << ((i) % 8)))

void init_physical_memory(uint32_t mem_size) {
    total_pages = mem_size / PAGE_SIZE;
    for (uint32_t i = 0; i < total_pages / 8; i++) {
        bitmap[i] = 0x00;
    }

    // TODO: RESERVADO PARA O KERNEL
    for (uint32_t i = 0; i < 256; i++) {
        BITMAP_SET(bitmap, i);
    }
}

void* pmm_alloc() {
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!BITMAP_TEST(bitmap, i)) {
            BITMAP_SET(bitmap, i);
            return (void*)(i * PAGE_SIZE);
        }
    }
    return 0;
}

void pmm_free(void* addr) {
    uint32_t i = (uint32_t)addr / PAGE_SIZE;
    BITMAP_CLEAR(bitmap, i);
}