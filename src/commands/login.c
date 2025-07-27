#include <kernel/terminal.h>
#include <kernel/user.h>
#include <drivers/keyboard.h>
#include <lib/string.h>
#include <commands/command.h>

void cmd_login() {
    keyboard_clear_buffer();

    char username[32];
    char password[32];
    
    while (1) {
        terminal_print("Username: ");
        int pos = 0;
        char c;
        while (1) {
            keyboard_poll();
            if (!keyboard_has_char()) continue;
            c = keyboard_get_char();
            if (c == '\n') {
                username[pos] = '\0';
                terminal_putc('\n');
                break;
            }
            if (pos < sizeof(username) - 1) {
                username[pos++] = c;
                terminal_putc(c);
            }
        }

        terminal_print("Password: ");
        pos = 0;
        while (1) {
            keyboard_poll();
            if (!keyboard_has_char()) continue;
            c = keyboard_get_char();
            if (c == '\n') {
                password[pos] = '\0';
                terminal_putc('\n');
                break;
            }
            if (pos < sizeof(password) - 1) {
                password[pos++] = c;
                terminal_putc('*');
            }
        }

        if (user_login(username, password) == 0) {
            terminal_print("Login successful.\n\n");
            return;
        } else {
            terminal_print("Invalid username or password. Try again.\n\n");
        }
    }
}