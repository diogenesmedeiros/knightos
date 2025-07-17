#ifndef SHELL_H
#define SHELL_H

#define MAX_INPUT 256

#include <kernel/terminal.h>
#include <kernel/user.h>
#include <commands/command.h>
#include <lib/string.h>
#include <stddef.h>

void shell_prompt();
void shell_execute(const char* cmd);
void shell_handle_char(char c);
void shell_run();

#endif