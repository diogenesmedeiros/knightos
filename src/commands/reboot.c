#include <commands/command_reboot.h>

void cmd_reboot(const char* _) {
    (void)_;
    while ((inb(0x64) & 0x02) != 0);
    outb(0x64, 0xFE);
}