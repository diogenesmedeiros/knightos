#include <kernel/interrupts.h>
#include <drivers/io.h>
#include <lib/string.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

#define IDT_SIZE 256
#define IRQ_BASE 32

extern void load_idt(void* idt_ptr);

typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

static idt_entry_t idt[IDT_SIZE];
static idt_ptr_t idt_descriptor;

static irq_handler_t irq_handlers[IRQ_COUNT] = { 0 };

/* Monta uma entrada da IDT */
static void set_idt_entry(int vector, void (*handler)(void), uint8_t flags) {
    uint32_t addr = (uint32_t)handler;
    idt[vector].offset_low = addr & 0xFFFF;
    idt[vector].selector = 0x08;
    idt[vector].zero = 0;
    idt[vector].type_attr = flags;
    idt[vector].offset_high = (addr >> 16) & 0xFFFF;
}

static void send_eoi(uint8_t irq) {
    if (irq >= 8) outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}

void irq_dispatcher(uint8_t irq) {
    if (irq < IRQ_COUNT && irq_handlers[irq]) {
        irq_handlers[irq](irq, NULL);
    }
    send_eoi(irq);
}

static void pic_remap(void) {
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, IRQ_BASE);
    outb(PIC2_DATA, IRQ_BASE + 8);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
}

void interrupts_init(void) {
    memset(idt, 0, sizeof(idt));

    extern void* irq_stub_table[IRQ_COUNT];

    for (int i = 0; i < IRQ_COUNT; i++) {
        set_idt_entry(IRQ_BASE + i, irq_stub_table[i], 0x8E);
    }

    idt_descriptor.base = (uint32_t)&idt;
    idt_descriptor.limit = sizeof(idt) - 1;

    pic_remap();
    load_idt(&idt_descriptor);
    interrupts_enable();
}

bool register_interrupt_handler(uint8_t irq, irq_handler_t handler) {
    if (irq >= IRQ_COUNT) return false;
    irq_handlers[irq] = handler;
    return true;
}

bool unregister_interrupt_handler(uint8_t irq) {
    if (irq >= IRQ_COUNT) return false;
    irq_handlers[irq] = 0;
    return true;
}