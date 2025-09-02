#include <lib/time.h>
#include <stdint.h>

void delay(uint32_t ticks) {
    for (volatile uint32_t i = 0; i < ticks; i++) {
        for (volatile uint32_t j = 0; j < 1000; j++) {}
    }
}