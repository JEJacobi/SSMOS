#include "interrupts.h"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "hardware.h"

#include "graphics.h"
#include "output.h"

struct idt_entry idt[IDT_SIZE];

void interrupts_init()
{
	extern void* interrupt_handler_0x0; // Temp, but eventually will be divide by zero.
	add_interrupt(0, (int)(&interrupt_handler_0x0), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	extern void* interrupt_handler_0xD;
	add_interrupt(13, (int)(&interrupt_handler_0xD), KERNEL_SELECTOR, INTERRUPT_GATE);
	
	/* TEST
	printnum(
		get_position(0,0),
		idt[0].offset_1,
		get_color(LIGHT_GREEN, BLACK),
		16);
	printnum(
		get_position(0,1),
		idt[0].offset_2,
		get_color(LIGHT_GREEN, BLACK),
		16);
	printnum(
		get_position(0,2),
		&interrupt_handler_0x0,
		get_color(LIGHT_GREEN, BLACK),
		16); */
	
	// Finally, load the IDT with the calculated size and address of the first handler.
	load_idt(&idt[0], (sizeof (struct idt_entry) * IDT_SIZE) - 1);
	asm volatile ("xchgw %bx, %bx");
	
	//asm volatile ("int $0");//temp
	
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

void load_idt(void* base, uint16_t size)
{
    struct // Interrupt Descriptor Table pointer struct. Only needed by this function.
    {
        uint16_t length;
        uint32_t base;
    } __attribute__((packed)) idt_ptr;
	
    idt_ptr.length 	= size;
    idt_ptr.base 	= (uint32_t)base;
    asm volatile (); // Another bit of assembly stolen from osdev.
}

void interrupt_handler()
{
	print(
		get_position(0, 0),
		"INTERRUPT!",
		get_color(LIGHT_GREEN, BLACK));
	flip();
	while (true) { }
}