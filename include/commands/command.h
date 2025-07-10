#ifndef COMMAND_H
#define COMMAND_H

#define MAX_COMMANDS 32

#include <lib/string.h>
#include <stddef.h>
#include <kernel/user.h>
#include <kernel/terminal.h>
#include <kernel/shell.h>
#include <commands/command_clear.h>
#include <commands/command_echo.h>
#include <commands/command_help.h>
#include <commands/command_login.h>
#include <commands/command_reboot.h>
#include <commands/command_shutdown.h>
#include <commands/command_useradd.h>
#include <commands/command_whoami.h>

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