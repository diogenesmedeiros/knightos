#include <kernel/kernel.h>
#include <kernel/terminal.h>
#include <kernel/cpu.h>
#include <drivers/ata.h>

void print_color_blocks() {
    for (int i = 0; i < 16; i++) {
        // Converte o número da cor para string para exibir
        char buf[4];
        itoa(i, buf, 10);
        terminal_print_color("#", i, 0x0);
    }

    terminal_print("\n");
}

void cmd_sysinfo(const char* _) {
    terminal_clear();

    const char* ascii_knight[] = {
        "  ,--. ,--.        ,--.       ,--.       ,--.   ,-----.  ,---.   ",
        "  |  .'   /,--,--, `--' ,---. |  ,---. ,-'  '-.'  .-.  ''   .-'  ",
        "  |  .   ' |      \\,--.| .-. ||  .-.  |'-.  .-'|  | |  |`.  `-.  ",
        "  |  |\\   \\|  ||  ||  |' '-' '|  | |  |  |  |  '  '-'  '.-'    | ",
        "  `--' '--'`--''--'`--'.`-  / `--' `--'  `--'   `-----' `-----'  ",
        "                       `---'                                      "
    };

    const char* info_lines[] = {
        "System information:",
        "CPU: ",
        "DISK: ",
        "RAM: "
    };

    uint64_t disk_size = detect_disk_size();
    char cpu_brand[49];
    get_cpu_brand(cpu_brand);

    int knight_lines = sizeof(ascii_knight) / sizeof(ascii_knight[0]);
    int info_lines_count = sizeof(info_lines) / sizeof(info_lines[0]);

    for (int i = 0; i < knight_lines; i++) {
        terminal_print_color(ascii_knight[i], 0x2, 0x0); // ciano sobre preto
        terminal_print("\n");
    }

    for (int i = 0; i < info_lines_count; i++) {
        terminal_print("  "); // indent opcional à direita do ASCII

        switch (i) {
            case 0:
                terminal_print_color(info_lines[0], 0x2, 0x0);
                break;
            case 1:
                terminal_print_color(info_lines[1], 0x2, 0x0);
                terminal_print(cpu_brand);
                break;
            case 2:
                terminal_print_color(info_lines[2], 0x2, 0x0);
                terminal_write_size(disk_size);
                break;
            case 3:
                terminal_print_color(info_lines[3], 0x2, 0x0);
                terminal_write_size(g_total_ram);
                break;
        }

        terminal_print("\n");
    }

    print_color_blocks();
    terminal_print("\n");
}
