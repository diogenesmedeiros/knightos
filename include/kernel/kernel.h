#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>

extern uint64_t g_total_ram;

void kernel_main(uint32_t magic, uint32_t multiboot_info_addr);

#endif