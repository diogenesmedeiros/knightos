#include <kernel/terminal.h>
#include <programs/editor.h>

void cmd_edit(const char* args) {
    while (*args == ' ') args++;

    if (*args == '\0') {
        terminal_print("Usage: edit <filename>\n");
        return;
    }

    editor_run(args);
}