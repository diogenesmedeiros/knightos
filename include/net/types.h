#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>
#include <stddef.h>

#define ETH_P_IP 0x0800
#define ICMP_ECHO 8

typedef unsigned int uint;

static inline uint16_t htons(uint16_t val) {
    return (val << 8) | (val >> 8);
}

static inline uint16_t ntohs(uint16_t val) {
    return htons(val);
}

static inline uint32_t htonl(uint32_t val) {
    return ((val & 0xFF) << 24) | ((val & 0xFF00) << 8) |
           ((val & 0xFF0000) >> 8) | ((val >> 24) & 0xFF);
}

static inline uint32_t ntohl(uint32_t val) {
    return htonl(val);
}

#endif
