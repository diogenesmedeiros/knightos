#include <kernel/user.h>
#include <kernel/terminal.h>
#include <lib/string.h>
#include <kernel/shell.h>

void cmd_sudo(const char* args) {
    if (!args || strlen(args) == 0) {
        terminal_print("Uso: sudo <comando>\n");
        return;
    }

    terminal_print("[SUDO] Senha para ");
    terminal_print(user_whoami());
    terminal_print(": ");
    char password[64];
    terminal_read_password(password, sizeof(password));

    if (!user_check_password("root", password)) {
        terminal_print("Senha incorreta.\n");
        return;
    }

    shell_execute(args);
}