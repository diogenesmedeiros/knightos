#include <commands/command_useradd.h>
#include <kernel/terminal.h>
#include <user/user.h>
#include <lib/string.h>

void cmd_useradd(const char* args) {
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
        terminal_print("Usage: useradd <name> <password>\n");
        return;
    }

    int res = user_add(name, pass);
    if (res == 0)
        terminal_print("User created.\n");
    else if (res == -2)
        terminal_print("User already exists.\n");
    else
        terminal_print("Error creating user.\n");
}