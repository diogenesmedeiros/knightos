#include <kernel/terminal.h>
#include <user/user.h>
#include <commands/command.h>
#include <lib/string.h>
#include <stddef.h>
#include <drivers/keyboard.h>
#include <programs/shell.h>
#include <kernel/process.h>
#include <drivers/io.h>

#define MAX_INPUT 256
#define HISTORY_SIZE 32
#define CMD_MAX_LEN 128

static char input_buffer[MAX_INPUT];
static int input_pos = 0;

static char command_history[HISTORY_SIZE][CMD_MAX_LEN];
static int history_count = 0;
static int history_index = -1;

static void load_command_from_history(int index) {
    if (index < 0 || index >= history_count) return;
    terminal_print("\r");
    for (int i = 0; i < MAX_INPUT; i++) terminal_putc(' ');
    terminal_print("\r");

    strncpy(input_buffer, command_history[index % HISTORY_SIZE], MAX_INPUT);
    input_buffer[MAX_INPUT - 1] = '\0';
    input_pos = strlen(input_buffer);

    terminal_print(input_buffer);
}

void save_command_to_history(const char* cmd) {
    if (strlen(cmd) == 0) return;

    if (history_count > 0 && strcmp(command_history[(history_count - 1) % HISTORY_SIZE], cmd) == 0)
        return;

    strncpy(command_history[history_count % HISTORY_SIZE], cmd, CMD_MAX_LEN);
    command_history[history_count % HISTORY_SIZE][CMD_MAX_LEN - 1] = '\0';
    history_count++;
    history_index = -1;
}

void shell_prompt() {
    const char* user = user_whoami();
    const char* dir = fs_get_current_directory();

    terminal_print_color("[", 0x4, 0x0);
    // Usuário
    if (strcmp(user, "none") != 0) {
        terminal_print_color(user, 0x2, 0x0);
    } else {
        terminal_print_color("guest", 0x2, 0x0);
    }
    terminal_print_color("]", 0x4, 0x0);

    terminal_print_color("@", 0xE, 0x0);

    // Nome do PC
    terminal_print_color("[", 0x4, 0x0);
    terminal_print_color("knightos", 0x7, 0x0);
    terminal_print_color("]", 0x4, 0x0);

    // Diretório
    terminal_print_color(":", 0x08, 0x0);
    terminal_print_color(dir, 0x08, 0x0);
    terminal_print_color("$ ", 0x08, 0x0);
}


void shell_execute(const char* cmd) {
    command_execute(cmd);
}

void shell_handle_special_key(int key) {
    if (key == KEY_UP) {
        if (history_count == 0) return;

        if (history_index == -1)
            history_index = history_count - 1;
        else if (history_index > 0)
            history_index--;

        load_command_from_history(history_index);
    } else if (key == KEY_DOWN) {
        if (history_index == -1) return;

        history_index++;
        if (history_index >= history_count)
            history_index = -1;

        if (history_index == -1) {
            terminal_print("\r");
            for (int i = 0; i < MAX_INPUT; i++) terminal_putc(' ');
            terminal_print("\r");

            input_pos = 0;
            input_buffer[0] = '\0';
            shell_prompt();
        } else {
            load_command_from_history(history_index);
        }
    }
}

static bool shell_handle_line(const char* line) {
    if (strcmp(line, "exit") == 0) {
        while ((inb(0x64) & 0x02) != 0);
        outb(0x64, 0xFE);
    }
    shell_execute(line);
    return false;
}

void shell_handle_char(char c) {
    if (c == '\n') {
        terminal_putc('\n');
        input_buffer[input_pos] = '\0';

        save_command_to_history(input_buffer);

        bool should_exit = shell_handle_line(input_buffer);
        input_pos = 0;

        if (should_exit) {
            process_exit();
        } else {
            shell_prompt();
        }
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

        int special = keyboard_get_special_key();
        if (special) {
            shell_handle_special_key(special);
            continue;
        }

        if (!keyboard_has_char()) continue;

        char c = keyboard_get_char();
        shell_handle_char(c);
    }
}