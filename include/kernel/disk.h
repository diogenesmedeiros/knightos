#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <kernel/terminal.h>
#include <drivers/io.h>

#define ATA_PRIMARY_IO  0x1F0
#define ATA_PRIMARY_CTRL 0x3F6

void ata_read_sector(uint32_t lba, uint8_t* buffer);
void ata_write_sector(uint32_t lba, const uint8_t* buffer);
uint64_t detect_disk_size();

#endif