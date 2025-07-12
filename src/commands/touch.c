#include <commands/command_touch.h>
#include <kernel/terminal.h>
#include <kernel/disk.h>
#include <kernel/fs.h>
#include <lib/string.h>

void cmd_touch(const char* name) {
    if (!name || strlen(name) == 0 || strlen(name) > 15) {
        terminal_print("Usage: touch <file.txt>\n");
        return;
    }

    uint8_t sector[512];
    uint8_t parent_sector = fs_get_current_sector();
    ata_read_sector(parent_sector, sector);

    uint8_t num_entries = sector[0];
    if (num_entries >= 15) {
        terminal_print("File/directory limit reached.\n");
        return;
    }

    for (int i = 0; i < num_entries; i++) {
        char* entry = (char*)&sector[1 + i * 32];
        if (strncmp(entry, name, 15) == 0) {
            terminal_print("File or directory already exists.\n");
            return;
        }
    }

    int data_sector = -1;
    uint8_t buf[512];
    for (int s = 2; s < 255; s++) {
        ata_read_sector(s, buf);
        int vazio = 1;
        for (int j = 0; j < 512; j++) {
            if (buf[j] != 0) {
                vazio = 0;
                break;
            }
        }
        if (vazio) {
            data_sector = s;
            break;
        }
    }

    if (data_sector == -1) {
        terminal_print("No free sectors.\n");
        return;
    }

    uint8_t* entry = &sector[1 + num_entries * 32];
    memset(entry, 0, 32);
    strncpy((char*)entry, name, 15);
    entry[15] = '\0';
    entry[16] = 0x02;
    entry[17] = data_sector;

    sector[0] = num_entries + 1;
    ata_write_sector(parent_sector, sector);

    memset(buf, 0, 512);
    ata_write_sector(data_sector, buf);
}