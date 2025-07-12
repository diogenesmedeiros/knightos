#include <commands/command_cd.h>

void cmd_cd(const char* arg) {
    if (arg == 0 || *arg == '\0') {
        terminal_print("Usage: cd <directory>\n");
        return;
    }

    fs_set_current_directory(arg);

    terminal_print(fs_get_current_directory());
    terminal_print("\n");
}