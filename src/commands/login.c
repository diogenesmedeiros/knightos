#include <commands/command_login.h>

void cmd_login(const char* name) {
    while (*name == ' ') name++;
    if (*name == '\0') {
        terminal_print("Uso: login <nome>\n");
        return;
    }

    int res = user_login(name);
    if (res == 0)
        terminal_print("Login bem-sucedido.\n");
    else
        terminal_print("Usuario nao encontrado.\n");
}