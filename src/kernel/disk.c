#include <kernel/disk.h>

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)(lba));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);

    while (!(inb(0x1F7) & 0x08));

    for (int i = 0; i < 256; i++) {
        ((uint16_t*)buffer)[i] = inw(0x1F0);
    }
}

void ata_write_sector(uint32_t lba, const uint8_t* buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));

    outb(0x1F2, 1);

    outb(0x1F3, (uint8_t)(lba));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));

    while (inb(0x1F7) & 0x80);

    outb(0x1F7, 0x30);

    while (!(inb(0x1F7) & 0x08));

    for (int i = 0; i < 256; i++) {
        outw(0x1F0, ((const uint16_t*)buffer)[i]);
    }

    while (inb(0x1F7) & 0x80);
}

uint64_t detect_disk_size() {
    outb(ATA_PRIMARY_IO + 6, 0xA0);
     
    while ((inb(ATA_PRIMARY_IO + 7) & 0x80));

    outb(ATA_PRIMARY_IO + 7, 0xEC);

    if (inb(ATA_PRIMARY_IO + 7) == 0) {
        terminal_print("No disks detected.\n");
        return 0;
    }

    while (!(inb(ATA_PRIMARY_IO + 7) & 0x08));

    uint16_t data[256];
    for (int i = 0; i < 256; i++) {
        data[i] = inw(ATA_PRIMARY_IO);
    }

    uint32_t total_sectors = (data[61] << 16) | data[60];
    uint64_t size_bytes = (uint64_t)total_sectors * 512;

    return size_bytes;
}