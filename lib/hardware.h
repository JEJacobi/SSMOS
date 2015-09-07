// Functions for direct I/O with hardware.

// Most are blatantly stolen from an assembly toolkit on osdev.org.

#ifndef LIB_HARDWARE
#define LIB_HARDWARE

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//
// Check if a certain bit is toggled.
//
inline bool check_bit(int var, int pos)
{
    return ((var) & (1<<(pos)));
}

//
// Set a bit in variable var and return it.
//
inline int set_bit(int var, int pos)
{
    return var |= (1 << pos);
}

//
// Toggle a bit in variable var and return it.
//
inline int toggle_bit(int var, int pos)
{
    return var ^ (1 << pos);
}

//
// Read a byte from the specified port.
//
inline uint8_t inb(short port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

//
// Read a word from the specified port.
//
inline uint16_t inw(short port)
{
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "d"(port));
    return ret;
}

//
// Write a byte to the specified port.
//
inline void outb(short port, uint8_t val)
{
    asm volatile ("outb %0, %1" : : "a"(val), "d"(port));   
}

//
// Write a word to the specified port.
//
inline void outw(short port, uint16_t val)
{
    asm volatile ("outw %0, %1" : : "a"(val), "d"(port));   
}

//
// Waits for the CPU to complete an I/O operation. Little bit finicky.
//
inline void io_wait()
{
    asm volatile (  "jmp 1f\n\t"
                    "1:jmp 2f\n\t"
                    "2:" );
}
#endif