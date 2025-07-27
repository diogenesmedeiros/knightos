ENTRY(_start)

SECTIONS {
    . = 0x100000;

    kernel_start = .;

    .multiboot ALIGN(4K) : {
        KEEP(*(.multiboot))
    }

    .text ALIGN(4K) : {
        *(.text.startup)
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data)
        *(COMMON)
    }

    .bss : {
        *(.bss)
    }

    kernel_end = .;
}