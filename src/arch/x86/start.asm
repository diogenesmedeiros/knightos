[bits 32]
global _start
extern kernel_main

section .text
_start:
    push ebx
    push eax
    call kernel_main
.hang:
    jmp .hang