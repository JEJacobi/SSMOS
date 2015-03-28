#include "interrupts.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "hardware.h"

struct idt_entry idt[IDT_SIZE];

bool check_interrupts_enabled()
{
	unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

void load_idt(void* base, uint16_t size)
{
    struct // Interrupt Descriptor Table pointer struct. Only needed by this function.
    {
        uint16_t length;
        uint32_t base;
    } __attribute__((packed)) idt_ptr;
 
    idt_ptr.length 	= size;
    idt_ptr.base 	= (uint32_t)base;
    asm volatile ("lidt (%0)" : : "r"(&idt_ptr)); // Another bit of assembly stolen from osdev.
}