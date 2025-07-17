#include <kernel/user.h>
#include <lib/sha256.h>
#include <lib/string.h>
#include <kernel/disk.h>

static User users[MAX_USERS];
static int user_count = 0;
static const User* current_user = 0;

void user_save_all() {
    ata_write_sector(USER_SECTOR, (uint8_t*)users);
}

void user_load_all() {
    ata_read_sector(USER_SECTOR, (uint8_t*)users);
    user_count = 0;
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].name[0] != '\0') user_count++;
    }
}

void user_init() {
    user_count = 0;
    current_user = 0;
    user_load_all();
}

int user_add(const char* name, const char* password) {
    if (user_count >= MAX_USERS) return -1;
    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0)
            return -2;
    }
    strncpy(users[user_count].name, name, MAX_USERNAME_LEN - 1);

    uint8_t hash[32];
    char hex[64];
    sha256((const uint8_t*)password, strlen(password), hash);
    sha256_to_hex(hash, hex);
    memcpy(users[user_count].password_hash, hex, 64);

    users[user_count].is_root = strcmp(name, "root") == 0 ? 1 : 0;

    user_count++;
    user_save_all();
    return 0;
}

int user_login(const char* name, const char* password) {
    uint8_t hash[32];
    char hex[64];
    sha256((const uint8_t*)password, strlen(password), hash);
    sha256_to_hex(hash, hex);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0 &&
            strncmp(users[i].password_hash, hex, 64) == 0) {
            current_user = &users[i];
            return 0;
        }
    }
    return -1;
}

const char* user_whoami() {
    return current_user ? current_user->name : "none";
}

int user_is_root() {
    return current_user && current_user->is_root;
}

int user_check_password(const char* name, const char* password) {
    uint8_t hash[32];
    char hex[64];
    sha256((const uint8_t*)password, strlen(password), hash);
    sha256_to_hex(hash, hex);

    for (int i = 0; i < user_count; i++) {
        if (strcmp(users[i].name, name) == 0 &&
            strncmp(users[i].password_hash, hex, 64) == 0) {
            return 1; // senha correta
        }
    }
    return 0;
}