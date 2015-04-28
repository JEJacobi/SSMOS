#include "interrupts.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "debug.h"
#include "hardware.h"
#include "graphics.h"
#include "irq.h"
#include "pic.h"
#include "output.h"
#include "terminal.h"
#include "string.h"
#include "syscalls.h"

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtptr;

extern void load_idt();

void interrupts_init()
{
	// Set up the IDT pointer.
	idtptr.limit = (sizeof (struct idt_entry) * IDT_SIZE) - 1;
    idtptr.base = (unsigned int)(&idt[0]);

	// Add the interrupts:
	
	//
	// Exceptions (what a mess):
	//
	
	extern void* interrupt_handler_0x0;
	add_interrupt(0, (int)(&interrupt_handler_0x0), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x1;
	add_interrupt(1, (int)(&interrupt_handler_0x1), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x2;
	add_interrupt(2, (int)(&interrupt_handler_0x2), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x3;
	add_interrupt(3, (int)(&interrupt_handler_0x3), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x4;
	add_interrupt(4, (int)(&interrupt_handler_0x4), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x5;
	add_interrupt(5, (int)(&interrupt_handler_0x5), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x6;
	add_interrupt(6, (int)(&interrupt_handler_0x6), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x7;
	add_interrupt(7, (int)(&interrupt_handler_0x7), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x8;
	add_interrupt(8, (int)(&interrupt_handler_0x8), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x9;
	add_interrupt(9, (int)(&interrupt_handler_0x9), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0xA;
	add_interrupt(10, (int)(&interrupt_handler_0xA), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0xB;
	add_interrupt(11, (int)(&interrupt_handler_0xB), KERNEL_SELECTOR, INTERRUPT_GATE);
		
	extern void* interrupt_handler_0xC;
	add_interrupt(12, (int)(&interrupt_handler_0xC), KERNEL_SELECTOR, INTERRUPT_GATE);
		
	extern void* interrupt_handler_0xD;
	add_interrupt(13, (int)(&interrupt_handler_0xD), KERNEL_SELECTOR, INTERRUPT_GATE);
		
	extern void* interrupt_handler_0xE;
	add_interrupt(14, (int)(&interrupt_handler_0xE), KERNEL_SELECTOR, INTERRUPT_GATE);
		
	extern void* interrupt_handler_0xF;
	add_interrupt(15, (int)(&interrupt_handler_0xF), KERNEL_SELECTOR, INTERRUPT_GATE);
		
	extern void* interrupt_handler_0x10;
	add_interrupt(16, (int)(&interrupt_handler_0x10), KERNEL_SELECTOR, INTERRUPT_GATE);
		
	extern void* interrupt_handler_0x11;
	add_interrupt(17, (int)(&interrupt_handler_0x11), KERNEL_SELECTOR, INTERRUPT_GATE);
		
	extern void* interrupt_handler_0x12;
	add_interrupt(18, (int)(&interrupt_handler_0x12), KERNEL_SELECTOR, INTERRUPT_GATE);
		
	extern void* interrupt_handler_0x13;
	add_interrupt(19, (int)(&interrupt_handler_0x13), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x14;
	add_interrupt(20, (int)(&interrupt_handler_0x14), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	// RESERVED //
	
	extern void* interrupt_handler_0x1E;
	add_interrupt(30, (int)(&interrupt_handler_0x1E), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	
	//
	// IRQ's (also a mess):
	//
	
	extern void* interrupt_handler_0x20;
	add_interrupt(32, (int)(&interrupt_handler_0x20), KERNEL_SELECTOR, INTERRUPT_GATE);

	extern void* interrupt_handler_0x21;
	add_interrupt(33, (int)(&interrupt_handler_0x21), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x22;
	add_interrupt(34, (int)(&interrupt_handler_0x22), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x23;
	add_interrupt(35, (int)(&interrupt_handler_0x23), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x24;
	add_interrupt(36, (int)(&interrupt_handler_0x24), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x25;
	add_interrupt(37, (int)(&interrupt_handler_0x25), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x26;
	add_interrupt(38, (int)(&interrupt_handler_0x26), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x27;
	add_interrupt(39, (int)(&interrupt_handler_0x27), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x28;
	add_interrupt(40, (int)(&interrupt_handler_0x28), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x29;
	add_interrupt(41, (int)(&interrupt_handler_0x29), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x2A;
	add_interrupt(42, (int)(&interrupt_handler_0x2A), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x2B;
	add_interrupt(43, (int)(&interrupt_handler_0x2B), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x2C;
	add_interrupt(44, (int)(&interrupt_handler_0x2C), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x2D;
	add_interrupt(45, (int)(&interrupt_handler_0x2D), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x2E;
	add_interrupt(46, (int)(&interrupt_handler_0x2E), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	extern void* interrupt_handler_0x2F;
	add_interrupt(47, (int)(&interrupt_handler_0x2F), KERNEL_SELECTOR, INTERRUPT_GATE);	
	
	
	//
	// System calls:
	//
	
	extern void* interrupt_handler_0x80;
	add_interrupt(128, (int)(&interrupt_handler_0x80), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	
	load_idt(); // Finally, load the IDT with the calculated size and address of the first handler.
}

void add_interrupt(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
	idt[num].selector = selector; // Copy over the direct values.
	idt[num].zero = 0;
	idt[num].attributes = flags;
	
	idt[num].offset_1 = base & 0xFFFF; // And split the base with some fancy bitwising.
	idt[num].offset_2 = (base >> 16) & 0xFFFF;
}

bool check_interrupts_enabled()
{
	unsigned long flags;
    asm volatile ( "pushf\n\t"
                   "pop %0"
                   : "=g"(flags) );
    return flags & (1 << 9);
}

int interrupt_handler(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax, int num, int err)
{	
	if (num >= 0x0 && num < 0x20) // Handle exceptions.
	{
		kprint(get_position(0, 0), "EXCEPTION!", get_color(LIGHT_GREEN, BLACK)); // TEMP
		kprintnum(get_position(0, 1), num, get_color(LIGHT_GREEN, BLACK), 16);
		kflip();
		return 0;
	}
	else if (num >= 0x20 && num < 0x30) // Handle PIC/IRQ mapped interrupts.
	{
		num -= MASTER_PIC_OFFSET; // Convert the interrupt vector to an IRQ;
		process_irq(num); // Handle the IRQ in the respective system.
		send_EOI(num); // Send an End Of Interrupt signal to the PIC.
		return 0; // And return.
	}
	else if (num == 0x80) // Handle aystem calls.
	{
		return process_syscall(eax, ebx, ecx, edx, esi);
	}
	else // Handle none of the above working.
	{
		haltdump("Unknown interrupt number.");
		return -1; // This shouldn't happen anyway, haltdump never returns.
	}
}