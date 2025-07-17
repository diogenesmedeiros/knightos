#ifndef USER_H
#define USER_H

#include <stdint.h>

#define MAX_USERS 32
#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 64
#define USER_SECTOR 50

typedef struct {
    char name[MAX_USERNAME_LEN];
    char password_hash[MAX_PASSWORD_LEN];
    int is_root;
} User;

void user_init();
int user_add(const char* name, const char* password);
int user_login(const char* name, const char* password);
const char* user_whoami();
void user_save_all();
void user_load_all();
int user_check_password(const char* name, const char* password);

#endif