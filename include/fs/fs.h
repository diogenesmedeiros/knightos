#ifndef FS_H
#define FS_H

#include <lib/string.h>

const char* fs_get_current_directory();
void fs_set_current_directory(const char* path);
int fs_find(const char* name);

#endif