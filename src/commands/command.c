#include <user/user.h>
#include <kernel/terminal.h>
#include <programs/shell.h>
#include <commands/command_all.h>

static Command commands[MAX_COMMANDS];
static int command_count = 0;

void command_register(const char* name, CommandFunc func, const char* desc) {
    if (command_count < MAX_COMMANDS) {
        commands[command_count++] = (Command){ name, func, desc };
    }
}

void command_execute(const char* input) {
    char cmd[64];
    const char* args = NULL;

    int i = 0;
    while (input[i] && input[i] != ' ' && i < 63) {
        cmd[i] = input[i];
        i++;
    }
    cmd[i] = '\0';

    if (input[i] == ' ') {
        args = input + i + 1;
        while (*args == ' ') args++;
    } else {
        args = "";
    }

    for (int j = 0; j < command_count; j++) {
        if (strncmp(cmd, commands[j].name, 5) == 0) {
            commands[j].func(args);
            return;
        }
    }

    if (input != NULL && input[0] != '\0') {
        terminal_print("Command not recognized.\n");
    }
}

void command_list() {
    for (int i = 0; i < command_count; i++) {
        terminal_print(commands[i].name);
        terminal_print("    ");
        terminal_print(commands[i].description);
        terminal_putc('\n');
    }
}


#define XCOMMAND(name, func, desc) { #name, func, desc },
static Command default_commands[] = {
    #include "commands.def"
};
#undef XCOMMAND

void register_default_commands() {
    int count = sizeof(default_commands) / sizeof(Command);
    for(int i = 0; i < count; i++) {
        command_register(
            default_commands[i].name,
            default_commands[i].func,
            default_commands[i].description
        );
    }
}