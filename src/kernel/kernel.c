#include <stdint.h>
#include <kernel/terminal.h>
#include <programs/shell.h>
#include <kernel/memory.h>
#include <user/user.h>
#include <kernel/cpu.h>
#include <kernel/memory.h>
#include <drivers/keyboard.h>
#include <lib/string.h>
#include <kernel/kernel.h>
#include <stdbool.h>
#include <drivers/rtl8139.h>
#include <config/info.h>

extern uint8_t kernel_start[], kernel_end[];

extern void register_default_commands();

uint64_t g_total_ram = 0;

/**
 * Inicializa terminal, detecta memória, inicializa gerenciador de memória
 * Configura ambiente do usuário e comandos padrão
 * Loop de login até autenticação bem-sucedida, então executa shell
 */
void kernel_main(uint32_t magic, uint32_t multiboot_info_addr)
{
    terminal_init();
    rtl8139_init();
    dhcp_init();

    g_total_ram = detect_memory(magic, multiboot_info_addr);

    memory_init(g_total_ram, (uint32_t)kernel_start, (uint32_t)kernel_end);

    user_init();
    register_default_commands();

    keyboard_clear_buffer();

    while (strcmp(user_whoami(), "none") == 0)
    {
        const char* ascii_knight[] = {
            "  ,--. ,--.        ,--.       ,--.       ,--.   ,-----.  ,---.   ",
            "  |  .'   /,--,--, `--' ,---. |  ,---. ,-'  '-.'  .-.  ''   .-'  ",
            "  |  .   ' |      \\,--.| .-. ||  .-.  |'-.  .-'|  | |  |`.  `-.  ",
            "  |  |\\   \\|  ||  ||  |' '-' '|  | |  |  |  |  '  '-'  '.-'    | ",
            "  `--' '--'`--''--'`--'.`-  / `--' `--'  `--'   `-----' `-----'  ",
            "                       `---'                                      "
        };
        
        int knight_lines = sizeof(ascii_knight) / sizeof(ascii_knight[0]);

        for (int i = 0; i < knight_lines; i++) {
            terminal_print_color(ascii_knight[i], 0x2, 0x0); // ciano sobre preto
            terminal_print("\n");
        }

        cmd_login();
    }

    terminal_print("KnightOS 0.0.2\nCopyright (c) Soft Knight. All rights reserved.\n\n");

    while (1)
    {
        handle_icmp_reply();
        shell_run();
    }
}