#include <commands/command_useradd.h>

void cmd_useradd(const char* name) {
    while (*name == ' ') name++;
    if (*name == '\0') {
        terminal_print("Uso: useradd <nome>\n");
        return;
    }

    int res = user_add(name);
    if (res == 0)
        terminal_print("Usuario criado.\n");
    else if (res == -2)
        terminal_print("Usuario ja existe.\n");
    else
        terminal_print("Erro ao criar usuario.\n");
}