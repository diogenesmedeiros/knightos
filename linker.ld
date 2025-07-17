ENTRY(_start)

SECTIONS {
    .multiboot : {
        KEEP(*(.multiboot))
    }

    . = 0x100000;

    .text : {
        *(.text)
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
}