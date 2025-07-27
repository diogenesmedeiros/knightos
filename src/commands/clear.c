#include <kernel/terminal.h>

void cmd_clear(const char* _) {
    (void)_;
    terminal_clear();
}