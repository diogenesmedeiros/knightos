#include <terminal.h>

void cmd_echo(const char* args) {
    while (*args == ' ') args++;

    if (*args == '\0') {
        terminal_print("Digite algo para imprimir.\n");
    } else {
        terminal_print(args);
        terminal_putc('\n');
    }
    
}
