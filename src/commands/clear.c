#include <commands/command_clear.h>

void cmd_clear(const char* _) {
    (void)_;
    terminal_clear();
}