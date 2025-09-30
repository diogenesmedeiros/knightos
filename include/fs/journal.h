#ifndef JOURNAL_H
#define JOURNAL_H

#include <stdint.h>
#include <fs/fs.h>

#define JOURNAL_SECTOR 10
#define MAX_JOURNAL_ENTRIES 16

#define JOURNAL_CREATE_FILE 1
#define JOURNAL_MKDIR 2
#define JOURNAL_RM 3

struct journal_entry {
    uint8_t type; // operação: 1=create file, 2=mkdir, 3=rm
    uint8_t target_sector;
    uint8_t parent_sector;
    char name[15];
    uint8_t committed; // 0=não, 1=sim
} __attribute__((packed));

void ata_read_journal(struct journal_entry* entries);
void ata_write_journal(struct journal_entry* entries);
int write_journal(uint8_t type, uint8_t parent_sector, uint8_t target_sector, const char* name);
void mark_journal_committed(int index);
void recover_journal(void);

#endif