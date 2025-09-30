#include <kernel/terminal.h>
#include <drivers/ata.h>
#include <lib/string.h>
#include <fs/fs.h>
#include <fs/journal.h>

void cmd_mkdir(const char* name) {
    if (!name || strlen(name) == 0) {
        terminal_print("Usage: mkdir <name>\n");
        return;
    }

    uint8_t sector[512];
    uint8_t dir_sector = fs_get_current_sector();
    ata_read_sector(dir_sector, sector);

    uint8_t num_dirs = sector[0];
    if (num_dirs >= 15) { // 512 bytes / 32 bytes por entrada - 1
        terminal_print("Directory limit reached.\n");
        return;
    }

    // Checa se o nome já existe
    for (int i = 0; i < num_dirs; i++) {
        if (strncmp((char*)&sector[1 + i * 32], name, 15) == 0) {
            terminal_print("Directory already exists.\n");
            return;
        }
    }

    uint8_t new_sector = 2 + num_dirs;
    int journal_idx = write_journal(JOURNAL_MKDIR, dir_sector, new_sector, name);
    if (journal_idx < 0) {
        terminal_print("Journal full, cannot create directory.\n");
        return;
    }

    uint8_t* entry = &sector[1 + num_dirs * 32];
    memset(entry, 0, 32); // limpa a entrada
    strncpy((char*)entry, name, 15); // nome
    entry[16] = 0x01; // tipo = diretório
    entry[17] = 2 + num_dirs; // atribui setor

    sector[0] = num_dirs + 1;
    ata_write_sector(dir_sector, sector);

    mark_journal_committed(journal_idx);
}