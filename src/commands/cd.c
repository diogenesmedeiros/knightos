#include <commands/command.h>
#include <kernel/terminal.h>
#include <fs/fs.h>

void cmd_cd(const char* arg) {
    if (arg == 0 || *arg == '\0') {
        terminal_print("Usage: cd <directory>\n");
        return;
    }

    fs_set_current_directory(arg);
}