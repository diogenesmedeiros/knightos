#include <commands/command_whoami.h>

void cmd_whoami(const char* _) {
    (void)_;
    terminal_print("Logado como: ");
    terminal_print(user_whoami());
    terminal_putc('\n');
}