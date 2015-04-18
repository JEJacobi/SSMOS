#include "interrupts.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "debug.h"
#include "hardware.h"
#include "graphics.h"
#include "output.h"
#include "terminal.h"

#include "syscalls.h"

struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtptr;

extern void load_idt();

void interrupts_init()
{
	// Set up the IDT pointer.
	idtptr.limit = (sizeof (struct idt_entry) * IDT_SIZE) - 1;
    idtptr.base = (unsigned int)(&idt[0]);
	
	//
	// Add the interrupts:
	//
	
	extern void* interrupt_handler_0x0; // Temp, but eventually will be divide by zero.
	add_interrupt(0, (int)(&interrupt_handler_0x0), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0x1;
	add_interrupt(1, (int)(&interrupt_handler_0x1), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0xD;
	add_interrupt(13, (int)(&interrupt_handler_0xD), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	// System calls:
	extern void* interrupt_handler_0x80;
	add_interrupt(128, (int)(&interrupt_handler_0x80), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	load_idt(); // Finally, load the IDT with the calculated size and address of the first handler.
	
	//TEMPORARILY OFF UNTIL PIC GETS HANDLED
	//enable_interrupts(); // And turn them back on after the bootloader disable.
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
	/*int num;
	int err;
	asm volatile("movl (%%esp), %0" : "=a"(err));
	asm volatile("addl $4, %%esp; movl (%%esp), %0; subl $4, %%esp" : "=a"(num)); 
	
	static int i = 0;
	print(
		get_position(0, i),
		"INTERRUPT",
		get_color(LIGHT_GREEN, BLACK));
	printnum(
		get_position(0, i + 1),
		num,
		get_color(LIGHT_GREEN, BLACK),
		16);
	printnum(
		get_position(0, i + 2),
		eax,
		get_color(LIGHT_GREEN, BLACK),
		16);
	printnum(
		get_position(0, i + 3),
		esp,
		get_color(LIGHT_GREEN, BLACK),
		16);
	printnum(
		get_position(0, i + 4),
		err,
		get_color(LIGHT_GREEN, BLACK),
		16);
	i++;*/
	
	if (num >= 0x0 && num < 0x20) // Handle exceptions.
	{
		return 0;
	}
	else if (num >= 0x20 && num < 0x30) // Handle PIC/IRQ mapped interrupts.
	{
		return 0;
	}
	else if (num == 0x80) // Handle aystem calls.
	{
		return process_syscall(eax, ebx, ecx, edx, esi);
	}
	else // Handle none of the above working.
	{
		haltdump("Unknown interrupt number.");
	}
}