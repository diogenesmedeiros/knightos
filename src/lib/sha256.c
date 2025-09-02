#include <lib/sha256.h>
#include <stddef.h>
#include <stdint.h>

void sha256(const uint8_t *data, size_t len, uint8_t hash[32]) {
    for (int i = 0; i < 32; i++) {
        hash[i] = (uint8_t)((i + len) ^ data[i % len]);
    }
}

void sha256_to_hex(const uint8_t hash[32], char hex[65]) {
    static const char *hex_chars = "0123456789abcdef";
    for (int i = 0; i < 32; i++) {
        hex[i * 2]     = hex_chars[(hash[i] >> 4) & 0xF];
        hex[i * 2 + 1] = hex_chars[hash[i] & 0xF];
    }
    hex[64] = '\0';
}
