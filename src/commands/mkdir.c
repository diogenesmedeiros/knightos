#include <commands/command_mkdir.h>

void cmd_mkdir(const char* name) {
    if (!name || strlen(name) == 0) {
        terminal_print("Usage: mkdir <name>\n");
        return;
    }

    uint8_t sector[512];
    ata_read_sector(1, sector);

    uint8_t num_dirs = sector[0];
    if (num_dirs >= 31) {
        terminal_print("Directory limit reached.\n");
        return;
    }

    for (int i = 0; i < num_dirs; i++) {
        if (strncmp((char*)&sector[1 + i * 16], name, 16) == 0) {
            terminal_print("Directory already exists.\n");
            return;
        }
    }

    strncpy((char*)&sector[1 + num_dirs * 16], name, 16);
    sector[0] = num_dirs + 1;

    ata_write_sector(1, sector);
}