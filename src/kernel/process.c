#include <kernel/process.h>
#include <lib/string.h>

process_t processes[MAX_PROCESSES];
int current_process = -1;
int process_count = 0;

void process_init(void) {
    memset(processes, 0, sizeof(processes));
    current_process = -1;
    process_count = 0;
}

int process_create(void (*entry_point)(void), const char* name) {
    if (process_count >= MAX_PROCESSES) return -1;

    int pid = process_count;
    process_t* p = &processes[pid];

    p->pid = pid;
    p->state = PROCESS_STATE_READY;

    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';

    memset(p->stack_mem, 0, sizeof(p->stack_mem));

    p->stack = &p->stack_mem[(STACK_SIZE / 4) - 1];

    p->eip = (uint32_t)entry_point;
    p->esp = (uint32_t)p->stack;
    p->ebp = p->esp;

    p->ebx = 0;
    p->esi = 0;
    p->edi = 0;

    process_count++;
    return pid;
}

extern void context_switch(uint32_t** old_sp, uint32_t* new_sp);

void process_schedule(void) {
    int next = (current_process + 1) % process_count;

    for (int i = 0; i < process_count; i++) {
        int idx = (next + i) % process_count;
        if (processes[idx].state == PROCESS_STATE_READY) {
            next = idx;
            break;
        }
    }

    if (next == current_process) return;

    process_t* prev = (current_process >= 0) ? &processes[current_process] : NULL;
    process_t* next_p = &processes[next];

    if (prev) prev->state = PROCESS_STATE_READY;
    next_p->state = PROCESS_STATE_RUNNING;

    int old = current_process;
    current_process = next;


    if (prev) {
        context_switch(&prev->esp, next_p->esp);
    } else {
        asm volatile (
            "mov %0, %%esp\n"
            "jmp *%1\n"
            : : "r"(next_p->esp), "r"(next_p->eip)
        );
    }
}

void process_yield(void) {
    process_schedule();
}

void process_exit(void) {
    if (current_process < 0) return;

    processes[current_process].state = PROCESS_STATE_TERMINATED;

    process_schedule();
}

bool process_kill(uint32_t pid) {
    if (pid >= process_count) return false;

    process_t* p = &processes[pid];

    if (p->state == PROCESS_STATE_TERMINATED) return false;

    p->state = PROCESS_STATE_TERMINATED;

    if (current_process == (int)pid) {
        process_schedule();
    }

    return true;
}