#include <commands/command_rm.h>
#include <fs/fs.h>
#include <kernel/terminal.h>
#include <kernel/disk.h>
#include <lib/string.h>

void cmd_rm(const char* name) {
    if (!name || strlen(name) == 0 || strlen(name) > 15) {
        terminal_print("Usage: rm <file_or_folder>\n");
        return;
    }

    uint8_t sector[512];
    uint8_t current = fs_get_current_sector();
    ata_read_sector(current, sector);

    uint8_t num_entries = sector[0];
    for (int i = 0; i < num_entries; i++) {
        uint8_t* entry = &sector[1 + i * 32];
        char* entry_name = (char*)name;

        if (strncmp(entry_name, name, 15) == 0) {
            uint8_t tipo = entry[16];
            uint32_t data_sector = entry[17];

            if (tipo == 0x01) {
                uint32_t dir_sector[512];
                ata_read_sector(data_sector, dir_sector);
                if (dir_sector[0] > 0) {
                    terminal_print("Directory is not empty.\n");
                    return;
                }
            }
            terminal_print("a");

            terminal_print("a");
            uint8_t empty[512] = {0};
            ata_write_sector(data_sector, empty);

            terminal_print("a");
            for (int j = i; j < num_entries - 1; j++) {
                memcpy(&sector[1 + j * 32], &sector[1 + (j + 1) * 32], 32);
            }

            terminal_print("a");
            memset(&sector[1 + (num_entries - 1) * 32], 0, 32);
            sector[0] = num_entries - 1;

            terminal_print("a");
            ata_write_sector(current, sector);

            terminal_print("Removed successfully.\n");
            return;
        }
    }

    terminal_print("File or directory not found.\n");
}