// Blatantly stolen from an assembly toolkit on osdev.org.
#ifndef LIB_SERIAL
#define LIB_SERIAL

inline char inb(int port)
{
	char ret;
	asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
	return ret;
}

inline void outb(int port, char val)
{
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));	
}

#endif