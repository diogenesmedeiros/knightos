#include <stdint.h>
#include <arch/x86/interrupts.h>

void irq_handler_wrapper(uint8_t irq_num) {
    irq_dispatcher(irq_num);
}