#include "command.h"
#include "string.h"
#include "user.h"
#include "stddef.h"
#include "terminal.h"
#include "command_clear.h"
#include "command_echo.h"
#include "command_help.h"
#include "command_login.h"
#include "command_reboot.h"
#include "command_shutdown.h"
#include "command_useradd.h"
#include "command_whoami.h"

#define MAX_COMMANDS 32

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

    if (input[i] != ' ') {
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

    terminal_print("Comando nao reconhecido.\n");
}

void command_list() {
    for (int i = 0; i < command_count; i++) {
        terminal_print("- ");
        terminal_print(commands[i].name);
        terminal_print(": ");
        terminal_print(commands[i].description);
        terminal_putc('\n');
    }
}

void register_default_commands() {
    command_register("help", cmd_help, "Mostra os comandos disponiveis");
    command_register("echo", cmd_echo, "Imprime uma mensagem");
    command_register("clear", cmd_clear, "Limpa a tela");
    command_register("shutdown", cmd_shutdown, "Desliga o sistema");
    command_register("reboot", cmd_reboot, "Reinicia o sistema");
    command_register("useradd", cmd_useradd, "Cria um novo usuario");
    command_register("login", cmd_login, "Faz login como usuario existente");
    command_register("whoami", cmd_whoami, "Exibe o usuario atual");
}