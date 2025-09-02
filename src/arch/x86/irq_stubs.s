# irq_stubs.s — AT&T syntax (para i686-elf-as)

.global irq_stub_table
.global irq_common_stub

.section .text

irq_stub_table:
    .long irq_stub0, irq_stub1, irq_stub2, irq_stub3
    .long irq_stub4, irq_stub5, irq_stub6, irq_stub7
    .long irq_stub8, irq_stub9, irq_stub10, irq_stub11
    .long irq_stub12, irq_stub13, irq_stub14, irq_stub15

.macro IRQ_STUB n
    .global irq_stub\n
irq_stub\n:
    cli
    pushl $\n
    jmp irq_common_stub
.endm

IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15

irq_common_stub:
    pusha
    call irq_handler_wrapper
    popa
    add $4, %esp
    sti
    iret