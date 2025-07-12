#include <commands/command_useradd.h>

void cmd_useradd(const char* name) {
    while (*name == ' ') name++;
    if (*name == '\0') {
        terminal_print("Usage: useradd <name>\n");
        return;
    }

    int res = user_add(name);
    if (res == 0)
        terminal_print("User created.\n");
    else if (res == -2)
        terminal_print("User already exists.\n");
    else
        terminal_print("Error creating user.\n");
}