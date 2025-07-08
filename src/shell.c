#include "terminal.h"
#include "string.h"
#include "io.h"
#include "user.h"

#define MAX_INPUT 256

static char input_buffer[MAX_INPUT];
static int input_pos = 0;

static inline void shutdown() {
    outw(0x604, 0x2000);
}

static inline void reboot() {
    while ((inb(0x64) & 0x02) != 0);
    outb(0x64, 0xFE);
}

void shell_prompt() {
    terminal_print("knightos> ");
}

void shell_execute(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        terminal_print("Comandos disponiveis:\n");
        terminal_print("  help  - mostra comandos\n");
        terminal_print("  clear - limpa a tela\n");
        terminal_print("  echo <msg> - imprime mensagem\n");
    } else if (strcmp(cmd, "clear") == 0) {
        terminal_clear();
    } else if (strncmp(cmd, "echo", 5) == 0) {
        const char* args = cmd + 5;
        while (*args == ' ') args++;

        if (*args == '\0') {
            terminal_print("Digite algo para imprimir.\n");
        } else {
            terminal_print(args);
            terminal_putc('\n');
        }
        return;
    } else if (strcmp(cmd, "reboot") == 0) {
        terminal_print("Reiniciando...\n");
        reboot();
    } else if (strcmp(cmd, "shutdown") == 0) {
        terminal_print("Desligando...\n");
        shutdown();
    } else if (strncmp(cmd, "useradd", 7) == 0) {
        const char* name = cmd + 7;
        while (*name == ' ') name++;

        int res = user_add(name);
        if (res == 0) {
            terminal_print("Usuario criado.\n");
        } else if (res == -2) {
            terminal_print("Usuario ja existe.\n");
        } else {
            terminal_print("Erro ao criar usuario.\n");
        }
    } else if (strncmp(cmd, "login", 5) == 0) {
        const char* name = cmd + 5;
        while (*name == ' ') name++;

        int res = user_login(name);
        if (res == 0) {
            terminal_print("Login bem-sucedido.\n");
        } else {
            terminal_print("Usuario nao encontrado.\n");
        }
    } else if (strcmp(cmd, "whoami") == 0) {
        terminal_print("Logado como: ");
        terminal_print(user_whoami());
        terminal_putc('\n');
    } else {
        terminal_print("Comando nao reconhecido.\n");
    }
}

void shell_handle_char(char c) {
    if (c == '\n') {
        terminal_putc('\n');
        input_buffer[input_pos] = '\0';
        shell_execute(input_buffer);
        input_pos = 0;
        shell_prompt();
        return;
    }

    if (c == '\b') {
        if (input_pos > 0) {
            input_pos--;
            terminal_putc('\b');
            terminal_putc(' ');
            terminal_putc('\b');
        }
        return;
    }

    if (input_pos < MAX_INPUT - 1) {
        input_buffer[input_pos++] = c;
        terminal_putc(c);
    }
}