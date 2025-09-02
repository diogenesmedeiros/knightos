#include <kernel/process.h>
#include <kernel/terminal.h>

void cmd_proc(const char* _) {
    terminal_print("PID - Name - State\n");
    for (int i = 0; i < process_count; i++) {
        if (processes[i].state != PROCESS_STATE_TERMINATED) {
            terminal_write_dec(processes[i].pid);
            terminal_print(" - ");
            terminal_print(processes[i].name);
            terminal_print(" - ");
            switch (processes[i].state) {
                case PROCESS_STATE_READY: terminal_print("READY\n"); break;
                case PROCESS_STATE_RUNNING: terminal_print("RUNNING\n"); break;
                case PROCESS_STATE_BLOCKED: terminal_print("BLOCKED\n"); break;
                default: terminal_print("UNKNOWN\n"); break;
            }
        }
    }
}

void cmd_kill_proc(const char* args) {
    if (!args || !*args) {
        terminal_print("Usage: kill <pid>\n");
        return;
    }

    int pid = atoi(args);
    if (pid < 0 || pid >= process_count) {
        terminal_print("Invalid PID\n");
        return;
    }

    if (process_kill((uint32_t)pid)) {
        terminal_print("Process killed\n");
    } else {
        terminal_print("Failed to kill process\n");
    }
}