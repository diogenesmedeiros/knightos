#include <kernel/shell.h>

static char input_buffer[MAX_INPUT];
static int input_pos = 0;

void shell_prompt() {
    const char* userLogged = user_whoami();

    if (strcmp(userLogged, "none") == 0) {
        terminal_print("[knightos]$ ");
    } else {
        terminal_print("[");
        terminal_print(userLogged);
        terminal_print("]");
        terminal_print("$ ");
    }
}

void shell_execute(const char* cmd) {
    command_execute(cmd);
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