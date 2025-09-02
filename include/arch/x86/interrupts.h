#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#define IRQ_TIMER     0
#define IRQ_KEYBOARD  1
#define IRQ_ETHERNET 11
#define IRQ_COUNT     16 

#include <stdint.h>
#include <stdbool.h>

typedef void (*irq_handler_t)(uint8_t irq, void* ctx);

void interrupts_init(void);

static inline void interrupts_enable(void) {
    __asm__ volatile ("sti");
}

static inline void interrupts_disable(void) {
    __asm__ volatile ("cli");
}

bool register_interrupt_handler(uint8_t irq, irq_handler_t handler);

bool unregister_interrupt_handler(uint8_t irq);

#endif