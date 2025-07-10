#include <kernel/user.h>

static User users[MAX_USERS];
static int user_count = 0;
static const User* current_user = 0;

void user_init() {
    user_count = 0;
    current_user = 0;
}

int user_add(const char* name) {
    if (user_count >= MAX_USERS) return -1;

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0)
            return -2;
    }

    strncpy(users[user_count].name, name, MAX_USERNAME_LEN - 1);
    users[user_count].name[MAX_USERNAME_LEN - 1] = '\0';
    user_count++;
    return 0;
}

int user_login(const char* name) {
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0) {
            current_user = &users[i];
            return 0;
        }
    }
    return -1;
}

const char* user_whoami() {
    return current_user ? current_user->name : "none";
}