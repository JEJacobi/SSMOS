// Interrupt Descriptor Table structure and functions.

#ifndef KERNEL_INTERRUPTS
#define KERNEL_INTERRUPTS

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// 32-bit IDT Entry attributes:

// Format:
// P | DPL | S | (Task|Interrupt|Trap) - 32 bit
// 1 | 0 0 | 0 |  0101   1110    1111

// Notes:
// (S) should usually be zero.
// (P) should usually be one.
// DPL governs the highest calling privilege level
// that can use the interrupt.

#define TASK_GATE		0x85 // 10000101
#define INTERRUPT_GATE	0x8E // 10001110
#define TRAP_GATE		0x8F // 10001111

#define IDT_SIZE		256

#define KERNEL_SELECTOR	0x10

struct idt_entry
{
	uint16_t	offset_1;	// Lower part of the handler's address.
	uint16_t 	selector;	// Interrupt function selector. DPL must equal 0.
	uint8_t		zero;		// Must be zero.
	uint8_t		attributes;	// Attributes that indicate what kind of interrupt the entry is.
	uint16_t	offset_2;	// The upper part of the handler's address.
} __attribute__ ((packed));

struct idt_ptr
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

// IDT:
extern struct idt_entry idt[];

inline void disable_interrupts() 	{ asm volatile ("cli"); } // Disable interrupts via assembly.
inline void enable_interrupts()		{ asm volatile ("sti"); } // Enable interrupts via assembly.

void interrupts_init();						// Initialize the IDT and PIC.
void add_interrupt(	uint8_t num,			// Add and format an interrupt into the IDT.
					uint32_t base,
					uint16_t selector,
					uint8_t flags);
bool check_interrupts_enabled(); 			// Check that interrupts are enabled.

// Awful looking parameter list is due to interrupt_handler reading the format of both
// the custom interrupt common_handler and the pusha instruction to save the registers.

int interrupt_handler(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax, int eflags, int num, int err);

// In other words, DO NOT CHANGE OR EVERYTHING BREAKS.

#endif