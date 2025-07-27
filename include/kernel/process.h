#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_PROCESSES 16
#define STACK_SIZE 4096

typedef enum {
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef struct {
    uint32_t pid;
    process_state_t state;
    char name[32];

    uint32_t* stack;
    uint32_t stack_mem[STACK_SIZE / 4];

    uint32_t eip;
    uint32_t esp;
    uint32_t ebp;
    uint32_t ebx;
    uint32_t esi;
    uint32_t edi;
} process_t;

extern process_t processes[MAX_PROCESSES];
extern int current_process;
extern int process_count;

void process_init(void);
int process_create(void (*entry_point)(void), const char* name);
void process_yield(void);
void process_exit(void);
void process_schedule(void);
bool process_kill(uint32_t pid);

#endif