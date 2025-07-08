ENTRY(kernel_main)

SECTIONS {
    . = 0x10000;

    .multiboot : {
        KEEP(*(.multiboot))
    }

    .text : {
        *(.multiboot)
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }
}
