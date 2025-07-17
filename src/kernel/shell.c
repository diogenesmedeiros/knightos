#include <kernel/shell.h>
#include <drivers/keyboard.h>

static char input_buffer[MAX_INPUT];
static int input_pos = 0;


void shell_prompt() {
    const char* user = user_whoami();
    const char* dir = fs_get_current_directory();

    terminal_print("[");
    
    if (strcmp(user, "none") != 0) {
        terminal_print(user);
    } else {
        terminal_print("guest");
    }

    terminal_print("@knightos:");
    terminal_print(dir);
    terminal_print("]$ ");
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

void shell_run() {
    shell_prompt();
    while (1) {
        keyboard_poll();
        if (!keyboard_has_char()) continue;

        char c = keyboard_get_char();
        shell_handle_char(c);
    }
}