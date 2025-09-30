#include <drivers/ata.h>
#include <kernel/terminal.h>
#include <lib/string.h>
#include <fs/journal.h>

static void rm_recursive(uint8_t sector_num) {
    if (sector_num == 0) return;

    uint8_t sector[512];
    ata_read_sector(sector_num, sector);

    uint8_t num_entries = sector[0];
    if (num_entries > 15) num_entries = 15;

    for (int i = 0; i < num_entries; i++) {
        uint8_t* entry = &sector[1 + i * 32];
        uint8_t tipo = entry[16];
        uint8_t entry_sector = entry[17];

        if (tipo == 0x01) {
            rm_recursive(entry_sector);
        } else if (tipo == 0x02) {
            if (entry_sector != 0) {
                uint8_t empty[512] = {0};
                ata_write_sector(entry_sector, empty);
            }
        }

        if (tipo == 0x01 && entry_sector != 0) {
            uint8_t empty[512] = {0};
            ata_write_sector(entry_sector, empty);
        }
    }

    uint8_t empty[512] = {0};
    ata_write_sector(sector_num, empty);
}

void create_directory(uint8_t parent_sector, uint8_t target_sector, const char* name) {
    uint8_t sector[512];
    ata_read_sector(parent_sector, sector);

    uint8_t* entry = &sector[1 + sector[0] * 32];
    memset(entry, 0, 32);
    strncpy((char*)entry, name, 15);
    entry[16] = 0x01;
    entry[17] = target_sector;

    sector[0] = sector[0] + 1;
    ata_write_sector(parent_sector, sector);

    uint8_t empty[512] = {0};
    ata_write_sector(target_sector, empty);
}

void create_file(uint8_t parent_sector, uint8_t target_sector, const char* name) {
    uint8_t sector[512];
    uint8_t buf[512] = {0};
    ata_read_sector(parent_sector, sector);

    uint8_t* entry = &sector[1 + sector[0] * 32];
    memset(entry, 0, 32);
    strncpy((char*)entry, name, 15);
    entry[16] = 0x02;
    entry[17] = target_sector;

    sector[0] = sector[0] + 1;
    ata_write_sector(parent_sector, sector);

    ata_write_sector(target_sector, buf);
}

void ata_read_journal(struct journal_entry* entries) {
    uint8_t buffer[512];
    ata_read_sector(JOURNAL_SECTOR, buffer);
    memcpy(entries, buffer, sizeof(struct journal_entry) * MAX_JOURNAL_ENTRIES);
}

void ata_write_journal(struct journal_entry* entries) {
    uint8_t buffer[512] = {0};
    memcpy(buffer, entries, sizeof(struct journal_entry) * MAX_JOURNAL_ENTRIES);
    ata_write_sector(JOURNAL_SECTOR, buffer);
}

int write_journal(uint8_t type, uint8_t parent_sector, uint8_t target_sector, const char* name) {
    struct journal_entry entries[MAX_JOURNAL_ENTRIES];
    ata_read_journal(entries);

    for (int i = 0; i < MAX_JOURNAL_ENTRIES; i++) {
        if (entries[i].type == 0) {
            entries[i].type = type;
            entries[i].parent_sector = parent_sector;
            entries[i].target_sector = target_sector;
            strncpy(entries[i].name, name, 14);
            entries[i].name[14] = '\0';
            entries[i].committed = 0;
            ata_write_journal(entries);
            return i;
        }
    }
    return -1;
}

void mark_journal_committed(int index) {
    struct journal_entry entries[MAX_JOURNAL_ENTRIES];
    ata_read_journal(entries);

    if (index >= 0 && index < MAX_JOURNAL_ENTRIES) {
        entries[index].committed = 1;
        ata_write_journal(entries);
    }
}

void recover_journal(void) {
    struct journal_entry entries[MAX_JOURNAL_ENTRIES];
    ata_read_journal(entries);

    for (int i = 0; i < MAX_JOURNAL_ENTRIES; i++) {
        struct journal_entry* e = &entries[i];

        if (e->type != 0 && e->committed == 0) {
            terminal_print("Recovering journal entry: ");
            terminal_print(e->name);
            terminal_print("\n");

            switch (e->type) {
                case JOURNAL_CREATE_FILE:
                    create_file(e->parent_sector, e->target_sector, e->name);
                    break;
                case JOURNAL_MKDIR:
                    create_directory(e->parent_sector, e->target_sector, e->name);
                    break;
                case JOURNAL_RM:
                    rm_recursive(e->target_sector);
                    break;
            }

            e->committed = 1;
        }
    }

    ata_write_journal(entries);
}