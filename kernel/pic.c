#include "pic.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "hardware.h"

unsigned char master_mask;
unsigned char slave_mask;

void PIC_init()
{
	// Save interrupt masks.
	master_mask = (unsigned char)inb(MASTER_PIC_DATA);
	slave_mask = (unsigned char)inb(SLAVE_PIC_DATA);
	
	// Start the init sequence. ICW1.
	outb(MASTER_PIC_CMD, PIC_INIT + PIC_ICW4);
	io_wait();
	outb(SLAVE_PIC_CMD, PIC_INIT + PIC_ICW4);
	io_wait();
	
	// Set the PIC offsets. ICW2.
	outb(MASTER_PIC_DATA, MASTER_PIC_OFFSET);
	io_wait();
	outb(SLAVE_PIC_DATA, SLAVE_PIC_OFFSET);
	io_wait();
	
	// Set up the cascade between the two PICs. ICW3.
	outb(MASTER_PIC_DATA, MASTER_CASCADE);
	io_wait();
	outb(SLAVE_PIC_DATA, SLAVE_CASCADE);
	io_wait();
	
	// Set 80x86 mode for all PICs. ICW4.
	outb(MASTER_PIC_DATA, PIC_8086_MODE);
	io_wait();
	outb(SLAVE_PIC_DATA, PIC_8086_MODE);
	io_wait();
	
	// Lastly, restore the saved interrupt masks from earlier.
	outb(MASTER_PIC_DATA, master_mask);
	outb(SLAVE_PIC_DATA, slave_mask);
}

void send_EOI(unsigned char irq)
{
	if(irq >= 8) // Send the EOI to the slave if needed.
		outb(SLAVE_PIC_CMD, PIC_EOI);
	
	outb(MASTER_PIC_CMD, PIC_EOI); // But always send an EOI to the master PIC.
}