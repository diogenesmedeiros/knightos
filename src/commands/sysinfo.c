#include <commands/command_sysinfo.h>

void cmd_sysinfo(const char* _) {
    
    terminal_print("System information:\n");
    
    // CPU Info
    char cpu_brand[49];
    get_cpu_brand(cpu_brand);

    terminal_print("    CPU: ");
    terminal_print(cpu_brand);
    
    // Disk
    uint64_t disk_size = detect_disk_size();

    terminal_print("\n");
    terminal_print("    DISK: ");
    terminal_write_size(disk_size);

    // Memory Ram
    terminal_print("\n");
    terminal_print("    RAM: ");
    terminal_write_size(g_total_ram);
    terminal_print("\n");
}