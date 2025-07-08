#ifndef COMMAND_H
#define COMMAND_H

typedef void (*CommandFunc)(const char* args);
typedef struct {
    const char* name;
    CommandFunc func;
    const char* description;
} Command;

void command_register(const char* name, CommandFunc func, const char* desc);
void command_execute(const char* input);
void command_list(void);
void register_default_commands(void);

#endif