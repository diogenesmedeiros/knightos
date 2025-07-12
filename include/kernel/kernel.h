#ifndef KERNEL_H
#define KERNEL_H

#define MULTIBOOT_MAGIC    0x1BADB002
#define MULTIBOOT_FLAGS    0x00000003
#define MULTIBOOT_CHECKSUM -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

#include <stdint.h>
#include <kernel/terminal.h>
#include <kernel/shell.h>
#include <kernel/memory.h>
#include <kernel/user.h>
#include <kernel/cpu.h>
#include <kernel/memory.h>
#include <drivers/keyboard.h>
#include <lib/string.h>

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