#include <drivers/ata.h>
#include <kernel/terminal.h>
#include <lib/string.h>

void list_directory(uint8_t sector_number, int depth) {
    uint8_t sector[512];
    ata_read_sector(sector_number, sector);

    uint8_t num_entries = sector[0];

    for (int i = 0; i < num_entries; i++) {
        uint8_t* entry = &sector[1 + i * 32];
        char name[16] = {0};
        strncpy(name, (char*)entry, 15);

        uint8_t tipo = entry[16];
        uint8_t child_sector = entry[17];

        for (int j = 0; j < depth; j++) {
            terminal_print("  ");
        }

        terminal_print(" |-> ");
        terminal_print(name);

        if (tipo == 0x01) {
            terminal_print("/\n");
            if (child_sector != 0) {
                list_directory(child_sector, depth + 1);
            }
        } else if (tipo == 0x02) {
            const char* ext = strrchr(name, '.');
            if (ext) {
                terminal_print("  (");
                terminal_print(ext + 1);
                terminal_print(")");
            }
            terminal_print("\n");
        } else {
            terminal_print(" [Unknown type]\n");
        }
    }
}

void cmd_ls(const char* _) {
    list_directory(fs_get_current_sector(), 0);
}