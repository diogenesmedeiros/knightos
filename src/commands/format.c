#include <commands/command_format.h>
#include <kernel/terminal.h>
#include <kernel/disk.h>
#include <lib/string.h>

void cmd_format(const char* _) {
    if (!user_is_root()) {
        terminal_print("Permissão negada: requer root.\n");
        return;
    }

    terminal_print("Formatting disk...\n");

    uint8_t empty[512] = {0};
    const uint32_t total_sectors = 254;

    empty[0] = 0;
    ata_write_sector(1, empty);

    for (uint32_t i = 2; i <= 254; i++) {
        ata_write_sector(i, empty);
    }

    terminal_print("\nDisk formatted successfully.\n");
}
