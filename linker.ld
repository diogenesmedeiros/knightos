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
        KEEP(*(.rodata*))
        _binary_disk_img_start = .;
        *(.diskimg)
    }

    .bss : {
        *(.bss)
        *(COMMON)
    }
}
