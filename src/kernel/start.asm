[bits 32]
[extern kernel_main]

section .text
global _start

_start:
    push ebx
    push eax
    call kernel_main

.hang:
    jmp .hang