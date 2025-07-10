#include <commands/command_help.h>

void cmd_help(const char* _) {
    (void)_;
    terminal_print("Comandos disponiveis:\n");
    command_list();
}