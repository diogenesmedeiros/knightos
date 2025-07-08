#ifndef USER_H
#define USER_H

#define MAX_USERS 16
#define MAX_USERNAME_LEN 32

typedef struct {
    char name[MAX_USERNAME_LEN];
} User;

void user_init();
int user_add(const char* name);
int user_login(const char* name);
const char* user_whoami();

#endif