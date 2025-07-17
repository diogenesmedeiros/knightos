#include <kernel/terminal.h>
#include <kernel/editor.h>
#include <commands/command_edit.h>

void cmd_edit(const char* args) {
    while (*args == ' ') args++;

    if (*args == '\0') {
        terminal_print("Uso: edit <nome_do_arquivo>\n");
        return;
    }

    editor_run(args);
}