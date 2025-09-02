#ifndef SHELL_H
#define SHELL_H

void shell_run();
void shell_prompt();
void shell_execute(const char* cmd);
void shell_handle_char(char c);
void shell_handle_special_key(int key);

#endif