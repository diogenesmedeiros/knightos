#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed)) memory_map_t;

typedef struct {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
} __attribute__((packed)) multiboot_info_t;

uint64_t detect_memory(uint32_t magic, uint32_t addr);
void terminal_write_dec(uint64_t value);

#endif