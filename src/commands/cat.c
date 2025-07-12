#include <commands/command_cat.h>
#include <kernel/fs.h>
#include <kernel/terminal.h>
#include <kernel/disk.h>
#include <lib/string.h>
#include <commands/command_clear.h>

void cmd_cat(const char* name) {
    cmd_clear(NULL);
    uint8_t sector[512];
    ata_read_sector(fs_get_current_sector(), sector);

    uint8_t num = sector[0];
    for (int i = 0; i < num; i++) {
        char* entry = (char*)&sector[1 + i * 32];
        uint8_t type = entry[16];
        uint8_t data_sector = entry[17];

        if (type == 0x02 && strncmp(entry, name, 15) == 0) {
            uint8_t data[512];
            ata_read_sector(data_sector, data);
            terminal_print((char*)data);
            terminal_print("\n");
            return;
        }
    }

    terminal_print("File not found.\n");
}
