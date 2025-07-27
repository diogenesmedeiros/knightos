#include <kernel/terminal.h>
#include <kernel/user.h>

void cmd_whoami(const char* _) {
    (void)_;
    terminal_print("Logged in as: ");
    terminal_print(user_whoami());
    terminal_putc('\n');
}