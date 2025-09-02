#include <kernel/terminal.h>
#include <kernel/memory.h>
#include <drivers/ata.h>
#include <fs/fs.h>
#include <lib/string.h>
#include <lib/utils.h>

void cmd_touch(const char* args) {
    if (!args || strlen(args) == 0) {
        terminal_print("Usage: touch <file.txt> [> content]\n");
        return;
    }

    char name[16] = {0};
    char content[497] = {0};

    const char* redirect = strchr(args, '>');
    if (redirect) {
        size_t name_len = redirect - args;
        if (name_len >= sizeof(name)) name_len = sizeof(name) - 1;
        strncpy(name, args, name_len);
        trim(name);

        redirect++;
        while (*redirect == ' ') redirect++;
        strncpy(content, redirect, sizeof(content) - 1);
        trim(content);
    } else {
        strncpy(name, args, sizeof(name) - 1);
        trim(name);
    }

    if (strlen(name) == 0 || strlen(name) > 15) {
        terminal_print("Usage: touch <file.txt> [> content]\n");
        return;
    }

    uint8_t sector[512];
    uint8_t buf[512];

    uint8_t parent_sector = fs_get_current_sector();
    ata_read_sector(parent_sector, sector);

    uint8_t num_entries = sector[0];
    if (num_entries >= 15) {
        terminal_print("File/directory limit reached.\n");
        return;
    }

    // Verifica se já existe
    for (int i = 0; i < num_entries; i++) {
        char* entry_name = (char*)&sector[1 + i * 32];
        if (strncmp(entry_name, name, 15) == 0) {
            terminal_print("File or directory already exists.\n");
            return;
        }
    }
    
    uint8_t* entry = &sector[1 + num_entries * 32];
    memset(entry, 0, 32);
    strncpy((char*)entry, name, 15);
    entry[16] = 0x02; // tipo arquivo
    entry[17] = fs_get_current_directory();

    sector[0] = num_entries + 1;
    ata_write_sector(parent_sector, sector);

    // Escreve conteúdo do arquivo
    memset(buf, 0, 512);
    if (strlen(content) > 0) {
        strncpy((char*)buf, content, 512);
    }
    ata_write_sector(fs_get_current_directory(), buf);

    terminal_print("File created successfully.\n");
}