#include "system.h"

// Wrappers for system calls.

/*
NOTES:
- pusha/popa at the beginning and end of all asm syscalls, avoid clobbered registers.
- Move the syscall ID into eax, as per the syscall document.
- Move any parameters or outputs into the listed registers (ebx, ecx, edx, esi).
- Trigger the syscall with software interrupt $0x80.
- Read return values off of eax.
*/

void *malloc(size_t size)
{

}

void *realloc(void *ptr, size_t newsz)
{

}

void free(void *ptr)
{

}

void *memcpy(void *dest, const void *src, size_t sz)
{

}

void *memset(void *dest, char c, size_t sz)
{

}

void sleep(int x)
{

}

int system(const char *command)
{

}

void clear()
{
	asm volatile("pusha; movl $0x2, %%eax; int $0x80; popa" : );
}

void ccolor(int pos, int n, char color)
{

}

void putchar(int pos, char c, char color)
{

}

void putstring(int pos, char *text, int c, char color)
{

}

void print(char *string, char color)
{

}

void printnum(int pos, int num, char color, bool hex)
{

}