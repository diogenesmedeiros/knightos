#include <kernel/memory.h>

static uint8_t bitmap[MAX_PAGES / 8];
static uint32_t total_pages = 0;
static uint32_t next_free_page = 0;


void init_physical_memory(uint32_t mem_size) {
    total_pages = mem_size / PAGE_SIZE;
    for (uint32_t i = 0; i < total_pages / 8; i++) {
        bitmap[i] = 0x00;
    }

    // TODO: RESERVADO PARA O KERNEL
    for (uint32_t i = 0; i < 256; i++) {
        BITMAP_SET(bitmap, i);
    }

    next_free_page = 256;
}

void* pmm_alloc() {
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!BITMAP_TEST(bitmap, i)) {
            BITMAP_SET(bitmap, i);
            return (void*)(i * PAGE_SIZE);
        }
    }

    for (uint32_t i = 0; i < next_free_page; i++) {
        if (!BITMAP_TEST(bitmap, i)) {
            BITMAP_SET(bitmap, i);
            next_free_page = i + 1;
            return (void*)(i * PAGE_SIZE);
        }
    }

    return 0;
}

void pmm_free(void* addr) {
    uint32_t i = (uint32_t)addr / PAGE_SIZE;
    if (i >= total_pages) return;
    BITMAP_CLEAR(bitmap, i);
    if (i < next_free_page) {
        next_free_page = i;
    }
}

uint32_t pmm_count_free_pages() {
    uint32_t count = 0;
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!BITMAP_TEST(bitmap, i)) count++;
    }
    return count;
}