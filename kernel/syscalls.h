// The handlers for system-call interrupts.

#ifndef KERNEL_SYSCALL
#define KERNEL_SYSCALL

int process_syscall(int eax, int ebx, int ecx, int edx, int esi);

#endif