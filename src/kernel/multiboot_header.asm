section .multiboot align=4
global multiboot_header

multiboot_header:
    dd 0x1BADB002
    dd 0x00000003
    dd -(0x1BADB002 + 0x00000003)
    dd 0
    dd 0
    dd 0
    dd 0
    dd kernel_main
extern kernel_main