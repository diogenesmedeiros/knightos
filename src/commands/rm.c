#include <fs/fs.h>
#include <kernel/terminal.h>
#include <drivers/ata.h>
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

void cmd_rm(const char* name) {
    if (!name || strlen(name) == 0 || strlen(name) > 15) {
        terminal_print("Usage: rm <file_or_folder>\n");
        return;
    }

    uint8_t current = fs_get_current_sector();
    uint8_t sector[512];
    ata_read_sector(current, sector);

    uint8_t num_entries = sector[0];

    for (int i = 0; i < num_entries; i++) {
        uint8_t* entry = &sector[1 + i * 32];
        char entry_name[16] = {0};
        memcpy(entry_name, entry, 15);

        if (strncmp(entry_name, name, 15) == 0) {
            uint8_t tipo = entry[16];
            uint8_t entry_sector = entry[17];

            int journal_idx = write_journal(JOURNAL_RM, 0, entry_sector, name);
            if (journal_idx < 0) {
                terminal_print("Journal full, cannot remove file/folder.\n");
                return;
            }

            if (tipo == 0x01) {
                rm_recursive(entry_sector);
            } else {
                uint8_t empty[512] = {0};
                ata_write_sector(entry_sector, empty);
            }

            for (int j = i; j < num_entries - 1; j++) {
                memcpy(&sector[1 + j * 32], &sector[1 + (j + 1) * 32], 32);
            }
            memset(&sector[1 + (num_entries - 1) * 32], 0, 32);
            sector[0] = num_entries - 1;
            ata_write_sector(current, sector);

            mark_journal_committed(journal_idx);
            return;
        }
    }

    terminal_print("File or directory not found.\n");
}