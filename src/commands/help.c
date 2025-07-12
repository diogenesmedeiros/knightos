#include <commands/command_help.h>

void cmd_help(const char* _) {
    (void)_;
    terminal_print("Available commands:\n");
    command_list();
}