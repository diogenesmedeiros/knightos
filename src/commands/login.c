#include <commands/command_login.h>
#include <kernel/user.h>
#include <kernel/terminal.h>

void cmd_login(const char* args) {
    char name[MAX_USERNAME_LEN] = {0};
    char pass[MAX_PASSWORD_LEN] = {0};

    int i = 0;
    while (*args == ' ') args++;
    while (*args && *args != ' ' && i < MAX_USERNAME_LEN - 1)
        name[i++] = *args++;
    name[i] = '\0';

    while (*args == ' ') args++;
    i = 0;
    while (*args && i < MAX_PASSWORD_LEN - 1)
        pass[i++] = *args++;
    pass[i] = '\0';

    if (name[0] == '\0' || pass[0] == '\0') {
        terminal_print("Usage: login <name> <password>\n");
        return;
    }

    int res = user_login(name, pass);
    if (res == 0)
        terminal_print("Login successful.\n");
    else
        terminal_print("Invalid credentials.\n");
}