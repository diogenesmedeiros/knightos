#include <kernel/terminal.h>

void cmd_echo(const char* args) {
    while (*args == ' ') args++;

    if (*args == '\0') {
        terminal_print("Type something to print.\n");
    } else {
        terminal_print(args);
        terminal_putc('\n');
    }
    
}
