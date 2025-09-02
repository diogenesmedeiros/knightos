#ifndef ATA_H
#define ATA_H

#include <stdint.h>

void ata_read_sector(uint32_t lba, uint8_t* buffer);
void ata_write_sector(uint32_t lba, const uint8_t* buffer);
uint64_t detect_disk_size();

#endif