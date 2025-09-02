#include <knt/knightlang.h>
#include <kernel/terminal.h>
#include <lib/string.h>

void cmd_run_knt(const char* args) {
    char filename[64] = {0};

    while (*args == ' ') args++;

    int i = 0;
    while (*args && i < sizeof(filename) - 1) {
        filename[i++] = *args++;
    }
    filename[i] = '\0';

    if (filename[0] == '\0') {
        terminal_print("Usage: run <file.knt>\n");
        return;
    }

    run_knightlang_file(filename);
}
