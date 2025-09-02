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
#include <kernel/process.h>

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

    g_total_ram = detect_memory(magic, multiboot_info_addr);

    memory_init(g_total_ram, (uint32_t)kernel_start, (uint32_t)kernel_end);

    user_init();
    register_default_commands();

    process_init();

    terminal_print("KnightOS 0.2a\nCopyright (c) Soft Knight. All rights reserved.\n\n");

    keyboard_clear_buffer();

    int shell_pid = process_create(shell_run, "Shell");
    if (shell_pid < 0)
    {
        terminal_print("failed to create shell process\n");
        while (1)
        {
        }
    }

    while (strcmp(user_whoami(), "none") == 0)
    {
        cmd_login();
    }

    process_schedule();

    while (1)
    {
        process_yield();
    }
}