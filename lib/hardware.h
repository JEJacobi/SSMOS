// Functions for direct I/O with hardware.

// Most are blatantly stolen from an assembly toolkit on osdev.org.

#ifndef LIB_HARDWARE
#define LIB_HARDWARE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//
//	Check if a certain bit is toggled.
//
inline bool check_bit(int var, int pos)
{
	return ((var) & (1<<(pos)));
}

//
//	Read a byte from the specified port.
//
inline char inb(short port)
{
	char ret;
	asm volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
	return ret;
}

//
//	Write a byte to the specified port.
//
inline void outb(short port, char val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "d"(port));	
}

//
//	Waits for the CPU to complete an I/O operation. Little bit finicky.
//
inline void io_wait()
{
	asm volatile ( 	"jmp 1f\n\t"
					"1:jmp 2f\n\t"
					"2:" );
}
#endif