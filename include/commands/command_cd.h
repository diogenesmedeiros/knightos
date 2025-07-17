#ifndef COMMAND_CD_H
#define COMMAND_CD_H

#include <commands/command.h>
#include <kernel/terminal.h>
#include <fs/fs.h>

void cmd_cd(const char* arg);

#endif