.code64
.section .text
.globl start_thread
.globl switch_threads

start_thread:
    popq %rax
    popq %rdi

    callq *%rax

    callq exit_thread

//; void switch_threads(void∗∗ old_sp, void∗ new_sp); // C signature
switch_threads: 
    pushq %rbp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15


    movq %rsp, (%rdi) 
    movq %rsi, %rsp   

    popq %r15 
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp

	sti

    ret
