#include <commands/command_shutdown.h>

void cmd_shutdown(const char* _) {
    (void)_;
    outw(0x604, 0x2000);
}