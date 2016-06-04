#include <stdint.h>

static void syscall(uint64_t syscall_no, uint64_t arg1, uint64_t arg2)
{
    __asm__ ("movq %0, %%rax\n\r"
             "movq %1, %%rbx\n\r"
             "movq %2, %%rcx\n\r"
             "int $48\n\r"
             :
             : "m"(syscall_no), "m"(arg1), "m"(arg2)
             : "rax", "rbx", "rcx", "memory");
}

void main(void) {
    syscall(0, (uint64_t)"Hello from elf!\n", sizeof("Hello from elf!\n") - 1);
    while(1);
}
