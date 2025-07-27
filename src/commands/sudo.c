#include <kernel/user.h>
#include <kernel/terminal.h>
#include <lib/string.h>
#include <programs/shell.h>

void cmd_sudo(const char* args) {
    if (!args || strlen(args) == 0) {
        terminal_print("Usage: sudo <command>\n");
        return;
    }

    terminal_print("[SUDO] Password for ");
    terminal_print(user_whoami());
    terminal_print(": ");
    char password[64];
    terminal_read_password(password, sizeof(password));

    if (!user_check_password("root", password)) {
        terminal_print("Incorrect password.\n");
        return;
    }

    shell_execute(args);
}