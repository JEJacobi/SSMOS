#include "system.h"

// Wrappers for system calls.

/*
NOTES:
- Move the syscall ID into eax, as per the syscall document.
- Move any parameters or outputs into the listed registers (ebx, ecx, edx, esi).
- Trigger the syscall with software interrupt $0x80.
- Read return values off of eax.
- Functions with no return values should ignore the above, obviously.
*/

void *malloc(size_t size)
{
	void* ptr;
	asm volatile("movl $0x3, %%eax; int $0x80; movl %%eax, %0"
				:"=a"(ptr)		// Output ptr in eax.
				:"b"(size));	// Input through ebx.
	return ptr;
}

void *realloc(void *ptr, size_t newsz)
{
	void* newptr;
	asm volatile("movl $0x4, %%eax; int $0x80; movl %%eax, %0"
				:"=a"(newptr)
				:"b"(ptr), "c"(newsz)); 
				// Inputs are the old pointer in ebx, and the new desired size in ecx.
}

void free(void *ptr)
{
	asm volatile("movl $0x5, %%eax; int $0x80"
				: // No return value from free.
				:"b"(ptr)); // Only input is the old pointer to free in ebx.
}

void memcpy(void *dest, const void *src, size_t sz)
{
	asm volatile("movl $0x6, %%eax; int $0x80"
				:
				:"b"(dest), "c"(src), "d"(sz));
}

void memset(void *dest, char c, size_t sz)
{
	asm volatile("movl $0x7, %%eax; int $0x80"
				:
				:"b"(dest), "c"(c), "d"(sz));
}

void sleep(int x)
{

}

int system(const char *command)
{

}

void clear()
{
	asm volatile("movl $0x2, %%eax; int $0x80" : );
}

void ccolor(int pos, int n, char color)
{
	asm volatile("movl $0xC, %%eax; int $0x80"
				:
				:"b"(pos), "c"(n), "d"(color));
}

void putchar(int pos, char c, char color)
{
	asm volatile("movl $0x8, %%eax; int $0x80"
				:
				:"b"(pos), "c"(c), "d"(color));
}

void putstring(int pos, char *text, int c, char color)
{
	asm volatile("movl $0x9, %%eax; int $0x80"
				:
				:"b"(pos), "c"(text), "d"(c), "S"(color));
}

void print(int pos, char *text, char color)
{
	asm volatile("movl $0xA, %%eax; int $0x80"
				:
				:"b"(pos), "c"(text), "d"(color));
}

void printnum(int pos, int num, char color, int base)
{
	asm volatile("movl $0xB, %%eax; int $0x80"
				:
				:"b"(pos), "c"(num), "d"(color), "S"(base));
}