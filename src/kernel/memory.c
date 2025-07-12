#include <kernel/memory.h>

uint64_t detect_memory(uint32_t magic, uint32_t addr) {
    if (magic != 0x2BADB002)
        return 0;

    multiboot_info_t* mbi = (multiboot_info_t*) addr;

    if (!(mbi->flags & (1 << 6)))
        return 0;

    uint64_t total_memory = 0;
    memory_map_t* mmap = (memory_map_t*) mbi->mmap_addr;

    while ((uint32_t)mmap < mbi->mmap_addr + mbi->mmap_length) {
        if (mmap->type == 1) {
            total_memory += (uint64_t)mmap->len;
        }
        mmap = (memory_map_t*)((uint32_t)mmap + mmap->size + 4);
    }

    return total_memory;
}