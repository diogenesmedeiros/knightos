#include <commands/command_login.h>

void cmd_login(const char* name) {
    while (*name == ' ') name++;
    if (*name == '\0') {
        terminal_print("Usage: login <name>\n");
        return;
    }

    int res = user_login(name);
    if (res == 0)
        terminal_print("Login successful.\n");
    else
        terminal_print("User not found.\n");
}