#include <fs/fs.h>
#include <kernel/terminal.h>
#include <kernel/disk.h>
#include <lib/string.h>

static char current_directory[256] = "/";
static uint8_t current_sector = 1;

const char* fs_get_current_directory() {
    return current_directory;
}

uint8_t fs_get_current_sector() {
    return current_sector;
}

void fs_set_current_directory(const char* path) {
    if (strcmp(path, "..") == 0) {
        int len = strlen(current_directory);
        if (len > 1) {
            for (int i = len - 2; i >= 0; i--) {
                if (current_directory[i] == '/') {
                    current_directory[i + 1] = '\0';
                    break;
                }
            }

            current_sector = 1;
        }
        return;
    }

    uint8_t sector[512];
    ata_read_sector(current_sector, sector);

    uint8_t num_dirs = sector[0];
    for (int i = 0; i < num_dirs; i++) {
        char* name = (char*)&sector[1 + i * 32];
        uint8_t sub_sector = sector[1 + i * 32 + 17];

        if (strncmp(name, path, 15) == 0) {
            if (strcmp(current_directory, "/") != 0) {
                strcat(current_directory, path);
                strcat(current_directory, "/");
            } else {
                strcpy(current_directory, "/");
                strcat(current_directory, path);
                strcat(current_directory, "/");
            }

            current_sector = sub_sector;
            return;
        }
    }

    terminal_print("Directory not found.\n");
}

int fs_find(const char* name) {
    uint8_t sector[512];
    uint8_t dir_sector = fs_get_current_sector();
    ata_read_sector(dir_sector, sector);

    int num_entries = sector[0];
    for (int i = 0; i < num_entries; i++) {
        uint8_t* entry = &sector[1 + i * 32];
        if (entry[16] == 0x02) {
            char entry_name[16] = {0};
            memcpy(entry_name, entry, 15);

            if (strncmp(entry_name, name, strlen(name)) == 0) {
                return entry[17];
            }
        }
    }
    return -1;
}