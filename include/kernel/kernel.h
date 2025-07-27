#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

struct multiboot_header_t {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
};

extern const struct multiboot_header_t multiboot_header;
extern uint64_t g_total_ram;

void kernel_main(uint32_t magic, uint32_t multiboot_info_addr);

#endif