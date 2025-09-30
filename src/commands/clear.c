#include <kernel/terminal.h>

void cmd_clear(const char* _) {
    (void)_;
    terminal_clear();
    terminal_print("KnightOS 0.0.2\nCopyright (c) Soft Knight. All rights reserved.\n\n");
}