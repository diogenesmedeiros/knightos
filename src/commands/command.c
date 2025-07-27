#include <kernel/user.h>
#include <kernel/terminal.h>
#include <programs/shell.h>
#include <commands/command_clear.h>
#include <commands/command_echo.h>
#include <commands/command_help.h>
#include <commands/command_login.h>
#include <commands/command_reboot.h>
#include <commands/command_shutdown.h>
#include <commands/command_useradd.h>
#include <commands/command_whoami.h>
#include <commands/command_sysinfo.h>
#include <commands/command_ls.h>
#include <commands/command_mkdir.h>
#include <commands/command_cd.h>
#include <commands/command_format.h>
#include <commands/command_cat.h>
#include <commands/command_touch.h>
#include <commands/command_rm.h>
#include <commands/command_run_knt.h>
#include <commands/command_edit.h>
#include <commands/command_sudo.h>
#include <commands/command_allocpage.h>
#include <commands/command_proc.h>
#include <commands/command.h>

static Command commands[MAX_COMMANDS];
static int command_count = 0;

void command_register(const char* name, CommandFunc func, const char* desc) {
    if (command_count < MAX_COMMANDS) {
        commands[command_count++] = (Command){ name, func, desc };
    }
}

void command_execute(const char* input) {
    char cmd[64];
    const char* args = NULL;

    int i = 0;
    while (input[i] && input[i] != ' ' && i < 63) {
        cmd[i] = input[i];
        i++;
    }
    cmd[i] = '\0';

    if (input[i] == ' ') {
        args = input + i + 1;
        while (*args == ' ') args++;
    } else {
        args = "";
    }

    for (int j = 0; j < command_count; j++) {
        if (strncmp(cmd, commands[j].name, 5) == 0) {
            commands[j].func(args);
            return;
        }
    }

    if (input != NULL && input[0] != '\0') {
        terminal_print("Command not recognized.\n");
    }
}

void command_list() {
    for (int i = 0; i < command_count; i++) {
        terminal_print(commands[i].name);
        terminal_print("    ");
        terminal_print(commands[i].description);
        terminal_putc('\n');
    }
}

void register_default_commands() {
    command_register("help", cmd_help, "Shows available commands");
    command_register("echo", cmd_echo, "Print a message");
    command_register("clear", cmd_clear, "Clean the screen");
    command_register("shutdown", cmd_shutdown, "Turn off the system");
    command_register("reboot", cmd_reboot, "Restart the system");
    command_register("useradd", cmd_useradd, "Create a new user");
    command_register("login", cmd_login, "Logs in as an existing user");
    command_register("whoami", cmd_whoami, "Displays the current user");
    command_register("sysinfo", cmd_sysinfo, "Displays system information");
    command_register("mkdir", cmd_mkdir, "Creates a new folder");
    command_register("ls", cmd_ls, "Lists folders and files in the current directory");
    command_register("cd", cmd_cd, "Navigates into a folder");
    command_register("format", cmd_format, "Formats the disk");
    command_register("cat", cmd_cat, "Displays the contents of a file on the screen");
    command_register("touch", cmd_touch, "Creates a new empty file in the current directory");
    command_register("rm", cmd_rm, "Remove files and directories from the system");
    command_register("knt", cmd_run_knt, "Executes .knt language scripts or commands");
    command_register("edit", cmd_edit, "Opens a simple interactive terminal editor to input content");
    command_register("sudo", cmd_sudo, "Executes commands with superuser privileges");
    command_register("memtest", cmd_allocpage, "Allocates a page of memory");
    command_register("proc_list", cmd_proc, "-----");
    command_register("kill", cmd_kill_proc, "Kill a process by PID");
}