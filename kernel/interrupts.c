#include "interrupts.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "hardware.h"

#include "graphics.h"
#include "output.h"
#include "terminal.h"

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtptr;

extern void load_idt();

void interrupts_init()
{
	// Set up the IDT pointer.
	idtptr.limit = (sizeof (struct idt_entry) * IDT_SIZE) - 1;
    idtptr.base = (unsigned int)(&idt[0]);

	// Add the interrupts:
	
	extern void* interrupt_handler_0x0; // Temp, but eventually will be divide by zero.
	add_interrupt(0, (int)(&interrupt_handler_0x0), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0xD;
	add_interrupt(13, (int)(&interrupt_handler_0xD), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	asm volatile ("xchgw %bx, %bx"); // TEMP BREAKPOINT
	
	load_idt(); // Finally, load the IDT with the calculated size and address of the first handler.
	enable_interrupts(); // And turn them back on after the bootloader disable.
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

void interrupt_handler()
{
	// TEMP
	static int i = 0;
	print(
		get_position(0, i),
		"INTERRUPT",
		get_color(LIGHT_GREEN, BLACK));
	i++;
}