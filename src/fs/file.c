#include <fs/file.h>
#include <fs/fs.h>
#include <kernel/terminal.h>
#include <kernel/disk.h>
#include <lib/string.h>
#include <lib/stdlib.h>
#include <stdint.h>

char* read_file(const char* path) {
    const char* name = strrchr(path, '/');
    if (!name) name = path;
    else name++;

    int sector = fs_find(name);
    if (sector < 0) {
        terminal_print("Arquivo não encontrado: ");
        terminal_print(name);
        terminal_print("\n");
        return NULL;
    }

    uint8_t data_sector[512];
    ata_read_sector(sector, data_sector);

    char* content = malloc(512);
    if (!content) {
        terminal_print("Erro de memória.\n");
        return NULL;
    }

    int i;
    for (i = 0; i < 511 && data_sector[i]; i++) {
        content[i] = data_sector[i];
    }
    content[i] = '\0';

    return content;
}