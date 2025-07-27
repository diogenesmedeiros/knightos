.global context_switch
.type context_switch, @function

context_switch:
    push %ebp
    mov %esp, %ebp

    movl 8(%ebp), %eax
    movl %esp, %edx
    movl %edx, (%eax)

    movl 12(%ebp), %eax
    movl %eax, %esp

    pop %ebp
    ret