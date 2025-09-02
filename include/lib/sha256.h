#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>

void sha256(const uint8_t* data, uint32_t len, uint8_t hash[32]);
void sha256_to_hex(const uint8_t hash[32], char output[64]);

#endif